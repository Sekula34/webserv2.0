#include "ConnectionDispatcher.hpp"
#include "SocketManager.hpp"
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
#include "../Utils/Logger.hpp"
#include "../Response/Response.hpp"

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

bool	ConnectionDispatcher::_checkReceiveError(Client& client, int n, int peek)
{
	if (n <= 0 || peek < 0)
	{
		clientsRemoveFd(&client);
		Data::epollRemoveFd(client.getFd());
		delete &client;
		if (n < 0 || peek < 0)
			std::cout << "error: receive" << std::endl;
		return (false);
	}
	return (true);
}

void	ConnectionDispatcher::_checkEndHeader(Client& client, int n)
{
	if (n < MAXLINE - 1 || client.getMessage().find("\r\n\r\n") != std::string::npos)
	{
		//std::cout << std::endl << client->getMessage() << std::endl;
		client.setReadHeader(false);
		client.setWriteClient(true);
	}
}

void	ConnectionDispatcher::_concatMessageAndPeek(Client* client, int n, int & peek)
{
	if (n > 0)
	{
		client->addRecvLineToMessage();
		if (n == MAXLINE - 1 && client->getMessage().find("\r\n\r\n") == std::string::npos)
			peek = recv(client->getFd(), client->getRecvLine(), MAXLINE, MSG_PEEK | MSG_DONTWAIT);
	}
}

bool	ConnectionDispatcher::readFd(int fd, Client & client, int & n, int idx)
{

	if (Data::setEvents()[idx].events & EPOLLIN)
	{
		client.clearRecvLine();
		n = recv(fd, client.getRecvLine(), MAXLINE - 1, MSG_DONTWAIT);
		return (true);
	}
	if (!client.checkTimeout())
	{
		// REMOVE THIS CLIENT INSTANCE FROM CLIENTS MAP
		clientsRemoveFd(&client);
		Data::epollRemoveFd(client.getFd());
		delete &client;
	}
	return (false);
}

void	ConnectionDispatcher::clientsRemoveFd(Client* client)
{
	// WRITING TO CLIENT FD IS FROM NOW ON FORBIDDEN FOR THIS CLIENT INSTANCE
	_clients[client->getFd()]->setWriteClient(false);
	
	// REMOVE THIS CLIENT INSTANCE FROM CLIENTS MAP
	_clients.erase(client->getFd());
}

bool	ConnectionDispatcher::readHeader(Client& client,  int idx)
{
	int			n = 0;
	int			peek = 0;

	// ON CONSTRUCTION READHEAD IS TRUE AND IS SET TO FALSE WHEN HEADER COMPLETELY READ
	if (!client.getReadHeader())
		return (true);

	// CHECK IF WE ARE ALLOWED TO READ FROM CLIENT. IF YES READ, IF NO -> RETURN
	// ALSO REMOVES CLIENT ON TIMEOUT
	if (!readFd(client.getFd(), client, n, idx))
		return (false);

	// SUCCESSFUL RECIEVE -> ADDING BUFFER FILLED BY RECIEVE TO THE MESSAGE STRING
	// peek in order to rule out the possibility to block on next read
	_concatMessageAndPeek(&client, n, peek);

	// UNSUCCESSFUL RECIEVE AND INCOMPLETE HEADER 
	// REMOVE CLIENT FROM CLIENTS AND EPOLL. DELETE CLIENT. LOG ERR MSG
	if (!_checkReceiveError(client, n, peek))
		return (false);


// IF return of receive is smaller than buffer -> SET READHEADER FLAG TO FALSE
	_checkEndHeader(client, n);
	return (true);
}

void	ConnectionDispatcher::writeClient(Client& client,  int idx)
{
	if (Data::setEvents()[idx].events & EPOLLOUT)
	{
		bool result = client.getResponse()->sendResponse();
		if(result == true)
			Logger::info("The response  was sent successfully", true);
		else
			Logger::warning("Sending response had some error");
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
		bool found = true;
		const ServerSettings*	clientServer = _serversInfo.getClientServer(client);
		if (!clientServer)
			return ;

		ClientHeader* clientHeader = client.header;
		std::string ServerLocation = clientServer->getLocationURIfromPath(clientHeader->urlSuffix->getPath());
		std::vector<LocationSettings>::const_iterator it = clientServer->fetchLocationWithUri(ServerLocation, found);
		// Logger::warning(it->getLocationUri(), true);
		// Logger::warning("", true);
		// std::cout << found << std::endl;
		if (found == true && it->getLocationUri() == "/cgi-bin/")  //this can be changed in cofig maybe
		{ 
			Logger::warning("Cgi checked and it exist on this location", true);
			client.setCgi(new CgiProcessor(client));
		}
	}
}

void	ConnectionDispatcher::_runCgi(Client& client)
{
	_checkCgi(client);
	if (!client.getCgi() || !client.cgiRunning)
		return ;
	client.getCgi()->process();
}


void	ConnectionDispatcher::_handleClient(Client& client, int idx)
{
	// READ_HEADER RETURNS FALSE WHEN ERR WHILE READING HEADER -> CLIENT IS DELETED
	if (!readHeader(client, idx))
		return ;
	
	// HANDLE HEADER
	if(client.getReadHeader() == false)
		client.createClientHeader();

	// HANDLE BODY
	if(client.header != NULL)
	{
		if(client.header->isBodyExpected() == true)
		{
			// READ BODY
			// PROCESS BODY
		}

		//run cgi if cgi on and only if there is no error in client so far
		_runCgi(client);
		if (client.getCgi() && client.cgiRunning)
			return ;

		// PROCESS ANSWER
		_processAnswer(client);
	}

	// WRITE PROCESSED ANSWER TO CLIENT
	if (client.getWriteClient())
		writeClient(client, idx);
}

void ConnectionDispatcher::_processAnswer(Client& client)
{
	Logger::info("Process answer for client: ");std::cout <<client.getId() << std::endl;  
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
		if (it->second->getCgi() && !it->second->getCgi()->killedChild)
			it->second->getCgi()->killChild();
	}
}


bool	ConnectionDispatcher::_catchEpollErrorAndSignal()
{
	if (_nfds == -1 || flag)
	{
		if(flag == 1)
		{
			flag++;
			Logger::info("Turn off procedure triggered", true);
		}
		if (flag && _clients.size() != 0)
			return(_shutdownCgiChildren(), true);
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
