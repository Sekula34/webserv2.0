#include <cstddef>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <sys/select.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <csignal>
#include <fcntl.h>
#include "ConnectionDispatcher.hpp"
#include "SocketManager.hpp"
#include "../Client/Message.hpp"
#include "../Client/Node.hpp"
#include "../Utils/Logger.hpp"
#include "../Parsing/ParsingUtils.hpp"
#include "../Response/Response.hpp"
#include "../Parsing/ParsingUtils.hpp"

#define MAX_WAIT		-1 // 0: epoll runs in nonblocking way but CPU runs at 6,7 % 

volatile sig_atomic_t flag = 0 ;

void handle_sigint(int sig)
{
	flag = 1;
	(void) sig;
	Logger::warning("CTRL + C caught, Server is turning off"); std::cout <<std::endl;
	//std::cout << "Called custom ctrl + c function" << std::endl;
}

ConnectionDispatcher::ConnectionDispatcher(SocketManager& sockets, ServersInfo& serversInfo):
_sockets(sockets),
_clients(Data::getClients()),
_serversInfo(serversInfo),
_epollfd(Data::getEpollFd()),
_nfds(Data::getNfds())
{}

ConnectionDispatcher::ConnectionDispatcher(ConnectionDispatcher& source):
_sockets(source._sockets),
_clients(Data::getClients()),
_serversInfo(source._serversInfo),
_epollfd(Data::getEpollFd()),
_nfds(Data::getNfds())
{}

ConnectionDispatcher& ConnectionDispatcher::operator=(ConnectionDispatcher& source)
{
	_sockets = source._sockets;
	_serversInfo = source._serversInfo;
	return(*this);
}

ConnectionDispatcher::~ConnectionDispatcher()
{
	close(Data::getEpollFd());
	std::map<int, Client*>::iterator it = _clients.begin();
	for(; it != _clients.end(); it++)
		delete it->second;
}

/* CREATE CLIENT FD BY CALLING ACCEPT ON LISTEN SOCKET, CREATE CLIENT INSTANCE
ADD INSTANCE TO CLIENTS MAP. MAP KEY: CLIENT FD, MAP VALUE: CLIENT INSTANCE POINTER */
void	ConnectionDispatcher::_epoll_accept_client(int listen_socket)
{
	struct sockaddr client_addr;
	socklen_t addrlen = sizeof(client_addr);
	int	clientfd;

	// ACCEPT RETURNS CLIENT FD
	clientfd = accept(listen_socket, &client_addr, &addrlen);
	if (clientfd == -1)
		throw std::runtime_error("accept error");

	// CREATE NEW CLIENT INSTANCE WITH CLIENT FD CONSTRUCTOR
	Client * newClient = new Client(clientfd, client_addr, addrlen);

	// IF CLIENT FD ALREADY EXISTS IN MAP, THEN SET NOWRITE IN THE CLIENT INSTANCE.
	// WE DON'T IMMEADIATELY DELETE CLIENT BECAUSE IT MIGHT BE PROCESSING.
	// IN THAT CASE THE  CLIENT INSTANCE NEEDS TO FINISH ITS JOB
	// AND HAS TO BE DELETED -> THIS CASE IS NOT YET TAKEN CARE OF
	// RIGHT NOW MEMORY LEAK!!! ADD TO GRAVEYARD VECTOR??
	if (_clients.find(clientfd) != _clients.end())
		_clients[clientfd]->setWriteClient(false);
	_clients[clientfd] = newClient;

	// ADD CLIENT FD TO THE LIST OF FDS THAT EPOLL IS WATCHING FOR ACTIVITY
	Data::epollAddFd(clientfd);
	
}

Client* ConnectionDispatcher::_isClient(int fd)
{
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return (NULL);
	return (it->second);
}

Client* ConnectionDispatcher::findClientInClients(int client_fd)
{
	std::map<int, Client*>::iterator it = _clients.find(client_fd);
	if (it == _clients.end())
	{
		std::cout << "no client with fd: " << client_fd
			<< " can be found in clients map! FATAL ERROR!"<< std::endl;
		throw std::runtime_error("error in find_client_in_clients function");
	}
	return (it->second);
}

void	ConnectionDispatcher::clientsRemoveFd(Client* client)
{
	// WRITING TO CLIENT FD IS FROM NOW ON FORBIDDEN FOR THIS CLIENT INSTANCE
	client->setWriteClient(false);
	
	// REMOVE THIS CLIENT INSTANCE FROM CLIENTS MAP
	_clients.erase(client->getFd());
}

bool	ConnectionDispatcher::_checkReceiveError(Client& client, int n, int peek)
{
	// this checks every time we go through loop. Maybe not necessary
	if (client.getClientMsg() && client.getClientMsg()->getHeader()
		&& client.getClientMsg()->getChain().begin()->getState() == COMPLETE)
	{
		client.setErrorCode(client.getClientMsg()->getHeader()->getHttpStatusCode()); 
	}

	if (n <= 0 || peek < 0 || client.getClientMsg()->getState() == ERROR)
	{
		if (n == 0)
			Logger::warning("reading 0 bytes from client, deleting client", true);
		// we are closing right now on n == 0 but we should not if keep-alive is on!!!!!!
		clientsRemoveFd(&client);
		Data::epollRemoveFd(client.getFd());
		if (n < 0 || peek < 0)
			Logger::error("receiving from Client", true);
		if (client.getClientMsg()->getState() == ERROR)
			Logger::error("Invalid Request", true);
		delete &client;
		return (false);
	}
	return (true);
}

void	ConnectionDispatcher::_peek(Client* client, int n, int & peek)
{
	if (n > 0)
	{
		// if (n == MAXLINE && client->getClientMsg()->getChain().begin()->getState() == INCOMPLETE)
		if (n == MAXLINE && client->getClientMsg()->getState() == INCOMPLETE)
			peek = recv(client->getFd(), client->getRecvLine(), MAXLINE, MSG_PEEK | MSG_DONTWAIT);
	}
}

bool	ConnectionDispatcher::readFd(int fd, Client & client, int & n, int idx)
{
	if (Data::setEvents()[idx].events & EPOLLIN)
	{
		client.clearRecvLine();
		n = recv(fd, client.getRecvLine(), MAXLINE, MSG_DONTWAIT | MSG_NOSIGNAL);
		// std::cout << "trying to read n: " << n << std::endl;
		return (true);
	}

	// handeling the case where we receive a Request from Client that has no proper delimiter
	if (!(Data::setEvents()[idx].events & EPOLLIN) && client.getClientMsg()
		&& client.getClientMsg()->getState() == INCOMPLETE
		&& client.getClientMsg()->getIterator()->getStringUnchunked().size() != 0)
	{
		// if we are at header, set header to complete and parse the header 
		if (client.getClientMsg()->getIterator()->getType() == HEADER && !client.getClientMsg()->getHeader())
		{
			client.getClientMsg()->_createHeader();
			client.getClientMsg()->_headerInfoToNode();
		}
		client.getClientMsg()->getIterator()->setState(COMPLETE);
		client.getClientMsg()->setState(COMPLETE);
	}

	// if no activity from Client then delete Client
	if (!client.checkTimeout())
	{
		clientsRemoveFd(&client);
		Data::epollRemoveFd(client.getFd());
		delete &client;
	}
	return (false);
}

void	ConnectionDispatcher::writeClient(Client& client,  int idx)
{
	if (Data::setEvents()[idx].events & EPOLLOUT)
	{
		if(client.getResponse()->sendResponse())
		{
			Logger::info("Finished sending message", true);
			clientsRemoveFd(&client);
			Data::epollRemoveFd(client.getFd());
			delete &client;
		}
	}
	else if (client.getResponse()->getBytesSent() != 0)
	{
		Logger::error("failed to send full Response to client", true);
		clientsRemoveFd(&client);
		Data::epollRemoveFd(client.getFd());
		delete &client;
	}
}

void	ConnectionDispatcher::_checkCgi(Client& client)
{
	if (!client.cgiChecked && client.getErrorCode() == 0)
	{
		client.cgiChecked= true;
		client.cgiChecked = true;
		bool found = true;
		const ServerSettings*	clientServer = _serversInfo.getClientServer(client);
		if (!clientServer)
			return ;

		bool locFound = false;
		std::vector<LocationSettings>::const_iterator location = _setCgiLocation(client, *clientServer, locFound);
		if(locFound)
		{
			Logger::info("Cgi checked and it exist on this location :", false); std::cout << location->getLocationUri() << std::endl;
			_parseCgiURLInfo(*location, client);
			RequestHeader* clientHeader = static_cast<RequestHeader*>(client.getClientMsg()->getHeader());
			std::string ServerLocation = clientServer->getLocationURIfromPath(clientHeader->urlSuffix->getPath());
			std::vector<LocationSettings>::const_iterator it = clientServer->fetchLocationWithUri(ServerLocation, found);
			if (found == true && it->getLocationUri() == "/cgi-bin/")  //this can be changed in cofig maybe
			{ 
				Logger::warning("Cgi checked and it exist on this location", true);
				client.setCgi(new CgiProcessor(client));
			}
		}
	}
}

void ConnectionDispatcher::_parseCgiURLInfo(const LocationSettings& cgiLocation,Client& client)
{
	Logger::info("Called cgi parse url", true);
	std::string fileName = ParsingUtils::getFileNameFromUrl( static_cast<RequestHeader *>(client.getClientMsg()->getHeader())->urlSuffix->getPath(), cgiLocation.getLocationUri());
	std::string scriptName = ParsingUtils::extractUntilDelim(fileName, "/");
	if(scriptName == "")
		scriptName = fileName;
	if(static_cast<RequestHeader*>(client.getClientMsg()->getHeader())->urlSuffix->setCgiScriptName(scriptName) == false)
	{
		Logger::warning("Implemted  some error code that is not correct");
		client.setErrorCode(400);
		return;
	}
	std::string scriptPath = cgiLocation.getLocationUri() + fileName;
	_setCgiPathInfo(static_cast<RequestHeader*>(client.getClientMsg()->getHeader())->urlSuffix->getPath(), scriptPath, client);
	Logger::info("Script name and file extension are setted and path info are setted ", false);
	std::cout << static_cast<RequestHeader*>(client.getClientMsg()->getHeader())->urlSuffix->getCgiScriptName() << " " << static_cast<RequestHeader*>(client.getClientMsg()->getHeader())->urlSuffix->getCgiScriptExtension() << " ";
	std::cout << static_cast<RequestHeader*>(client.getClientMsg()->getHeader())->urlSuffix->getCgiPathInfo() << std::endl;
}

void ConnectionDispatcher::_setCgiPathInfo(const std::string& urlpath, const std::string scriptPath, Client& client)
{
	size_t pathInfoPos = urlpath.find(scriptPath);
	if(pathInfoPos == std::string::npos)
	{
		Logger::error("Set cgi path this should never happend");
		return;
	}
	std::string pathInfo = urlpath.substr(pathInfoPos + scriptPath.size());
	if(_isCgiPathInfoValid(pathInfo) == false)
	{
		Logger::error("not valid cgi path");
		client.setErrorCode(400);
		return;
	}
	static_cast<RequestHeader*>(client.getClientMsg()->getHeader())->urlSuffix->setCgiPathInfo(pathInfo);
}

bool ConnectionDispatcher::_isCgiPathInfoValid(std::string pathInfo)
{
	if(pathInfo.find("/..") != std::string::npos || pathInfo.find("~") != std::string::npos)
	{
		return false;
	}
	return true;
}

std::vector<LocationSettings>::const_iterator ConnectionDispatcher::_setCgiLocation(Client& client, const ServerSettings& cgiServer, bool& foundLoc)
{
	foundLoc = false;
	RequestHeader* clientHeader =  static_cast<RequestHeader*>(client.getClientMsg()->getHeader());
	if(clientHeader == NULL)
	{
		Logger::error("prepare for cgi client header is NULL this should not happen ever", true);
		foundLoc = false;
		return cgiServer.getServerLocations().end();
	}
	std::string ServerLocation = cgiServer.getLocationURIfromPath(clientHeader->urlSuffix->getPath());
	bool found = true;
	std::vector<LocationSettings>::const_iterator it = cgiServer.fetchLocationWithUri(ServerLocation, found);
	if(found == false || it->isCgiLocation() == false)
	{
		foundLoc = false;
		return cgiServer.getServerLocations().end();
	}
	foundLoc = true;
	return it;
}

void	ConnectionDispatcher::_runCgi(Client& client)
{
	_checkCgi(client);
	if (!client.getCgi() || !client.cgiRunning)
		return ;
	client.getCgi()->process();
}

bool	ConnectionDispatcher::readClient(Client& client,  int idx)
{
	int			n = 0;
	int			peek = 0;

	// allocate new Message instance if necessary
	if (!client.getClientMsg())
		client.setClientMsg(new Message(true));

	if (client.getClientMsg()->getState() == COMPLETE)
		return (true);

	// CHECK IF WE ARE ALLOWED TO READ FROM CLIENT. IF YES READ, IF NO -> RETURN
	// ALSO REMOVES CLIENT ON TIMEOUT
	if (!readFd(client.getFd(), client, n, idx))
		return (false);

	// ADD BUFFER TO THE MESSAGE CLASS INSTANCE
	if (n > 0)
		client.getClientMsg()->bufferToNodes(client.getRecvLine(), n);

	// UNSUCCESSFUL RECIEVE OR ERR IN MESSAGE 
	// REMOVE CLIENT FROM CLIENTS AND EPOLL. DELETE CLIENT. LOG ERR MSG
	if (!_checkReceiveError(client, n, peek))
		return (false);

	return (true);
}

void	ConnectionDispatcher::_handleClient(Client& client, int idx)
{
	// READ_HEADER RETURNS FALSE WHEN ERR WHILE READING HEADER -> CLIENT IS DELETED
	bool b = readClient(client, idx);
	if (!b || !client.getClientMsg() || client.getClientMsg()->getState() == INCOMPLETE)
		return ;

	//run cgi if cgi on and only if there is no error in client so far
	_runCgi(client);
	if (client.getCgi() && client.cgiRunning)
		return ;

	// PROCESS ANSWER
	_processAnswer(client);

	// WRITE PROCESSED ANSWER TO CLIENT
	if (client.getResponse())
		writeClient(client, idx);
}

void ConnectionDispatcher::_processAnswer(Client& client)
{
	Logger::info("Process answer for client: ");std::cout << client.getId() << std::endl;  
	const ServerSettings* const responseServer = _serversInfo.getClientServer(client);
	Logger::info("Resposible server is ", true);
	if(responseServer != NULL)
	{
		Logger::info("Server found, ID: "); std::cout << responseServer->getServerId() << std::endl;
	}
	else
		Logger::warning("NO Server found");
	_createAndDelegateResponse(client, responseServer);
}

void ConnectionDispatcher:: _createAndDelegateResponse(Client& client, const ServerSettings* responseServer)
{
	Response* clientRespone = client.getResponse();
	if(clientRespone != NULL)
	{
		Logger::error("Trying to create response for client that already has one", true);
		return;
	}
	Response* response = new Response(client, responseServer);
	client.setResponse(response);
	Logger::info("Response created ", true);
}


void ConnectionDispatcher::_addServerSocketsToEpoll(void)
{
	std::vector<int> socketFds = Data::getServerSocketFds();
	for(size_t i = 0; i < socketFds.size(); i++)
		Data::epollAddFd(socketFds[i]);
}

void ConnectionDispatcher::_notStuckMessage(void) const
{
		static size_t counter = 0;
		if(counter < 10000)
	   		std::cout << "Waiting for new request." << std::flush;
		else if(counter < 20000)
			std::cout << "Waiting for new request.." << std::flush;
		else  
			std::cout << "Waiting for new request..." << std::flush;

        // Sleep for a short duration
		usleep(1);

        // Move the cursor back to the beginning of the line
        std::cout << "\r";

        // Clear the line (by overwriting with spaces)
        std::cout << std::string(27, ' ') << std::flush;

        // Move the cursor back to the beginning of the line again
        std::cout << "\r";
		counter++;
		if(counter > 30000)
			counter = 0;
}

bool ConnectionDispatcher::_handleServerSocket(size_t idx)
{
	for(size_t i = 0; i < _sockets.getAllListenFd().size(); i++)
	{
		if (Data::setEvents()[idx].data.fd == _sockets.getAllListenFd()[i])
		{
			_epoll_accept_client(_sockets.getAllListenFd()[i]);
			return true;
		}
	}
	return false;
}

void	ConnectionDispatcher::_shutdownCgiChildren()
{
	std::map<int, Client*>::iterator it = _clients.begin();
	for (; it != _clients.end(); it++)
	{
		if (it->second->getCgi() && !it->second->getCgi()->sentSigterm)
			it->second->getCgi()->terminateChild();
	}
}

bool	ConnectionDispatcher::_catchEpollErrorAndSignal()
{
	if (_nfds == -1 || flag)
	{
		if(flag == 1)
		{
			signal(SIGINT, SIG_IGN);
			flag++;
			Logger::info("Turn off procedure triggered", true);
			_shutdownCgiChildren();
		}
		if (flag && _clients.size() != 0)
			return (true);
		if (!flag)
			Logger::error("Epoll wait failed", true);
		return (false);
	}
	return (true);
}

void ConnectionDispatcher::mainLoopEpoll()
{
	Client* client;

	Logger::info("my pid is: "); std::cout << getpid() << std::endl;
	signal(SIGINT, handle_sigint);
	_addServerSocketsToEpoll();
	while(true)
	{
		if (!_catchEpollErrorAndSignal())
			break;
		_nfds = epoll_wait(_epollfd, Data::setEvents(), MAX_EVENTS, MAX_WAIT);
		for (size_t idx = 0; idx < static_cast<size_t>(_nfds) && _nfds != -1; ++idx)
		{
			if (_handleServerSocket(idx) == true)
				continue;
			if ((client = _isClient(Data::setEvents()[idx].data.fd)) != NULL)
	   			_handleClient(*client, idx);
		}
	}
}
