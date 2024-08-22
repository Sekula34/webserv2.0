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


volatile sig_atomic_t flag = 0 ;

void handle_sigint(int sig)
{
	flag = 1;
	(void) sig;
	Logger::warning("CTRL + C cathced, Server is turning off"); std::cout <<std::endl;
	//std::cout << "Called custom ctrl + c function" << std::endl;
}

ConnectionDispatcher::ConnectionDispatcher(SocketManager& sockets, ServersInfo& serversInfo, char** envp)
:_sockets(sockets), _serversInfo(serversInfo), _envp(envp)
{

}
ConnectionDispatcher::ConnectionDispatcher(ConnectionDispatcher& source)
:_sockets(source._sockets), _serversInfo(source._serversInfo)
{

}

ConnectionDispatcher& ConnectionDispatcher::operator=(ConnectionDispatcher& source)
{
	_sockets = source._sockets;
	_serversInfo = source._serversInfo;
	return(*this);
}

ConnectionDispatcher::~ConnectionDispatcher()
{
	std::map<int, Client*>::iterator it = clients.begin();
	for(; it != clients.end(); it++)
		delete it->second;
	close(Data::getEpollFd());
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
	Client * newClient = new Client(clientfd, Data::getEpollFd(), &_child_sockets, client_addr, &(_sockets.getAllSockets()));
	newClient->setAddrlen(addrlen);

	// IF CLIENT FD ALREADY EXISTS IN MAP, THEN SET NOWRITE IN THE CLIENT INSTANCE.
	// WE DON'T IMMEADIATELY DELETE CLIENT BECAUSE IT MIGHT BE PROCESSING.
	// IN THAT CASE THE  CLIENT INSTANCE NEEDS TO FINISH ITS JOB
	// AND HAS TO BE DELETED -> THIS CASE IS NOT YET TAKEN CARE OF
	// RIGHT NOW MEMORY LEAK!!! ADD TO GRAVEYARD VECTOR??
	if (clients.find(clientfd) != clients.end())
		clients[clientfd]->setWriteClient(false);
	clients[clientfd] = newClient;

	// ADD CLIENT FD TO THE LIST OF FDS THAT EPOLL IS WATCHING FOR ACTIVITY
	Data::epollAddFd(clientfd);
	
}

Client* ConnectionDispatcher::find_client_in_clients(int client_fd, std::map<int, Client *> & clients)
{
	std::map<int, Client*>::iterator it = clients.find(client_fd);
	if (it == clients.end())
	{
		std::cout << "no client with fd: " << client_fd
			<< " can be found in clients map! FATAL ERROR!"<< std::endl;
		throw std::runtime_error("error in find_client_in_clients function");
	}
	return (it->second);
}

bool	ConnectionDispatcher::_checkReceiveError(Client* client, int n, int peek)
{
	if (n <= 0 || peek < 0)
	{
		clients_remove_fd(client);
		Data::epollRemoveFd(client->getFd());
		delete client;
		if (n < 0 || peek < 0)
			std::cout << "error: receive" << std::endl;
		return (false);
	}
	return (true);
}

void	ConnectionDispatcher::_checkEndHeader(Client* client, int n)
{
	if (n < MAXLINE - 1)
	{
		//std::cout << std::endl << client->getMessage() << std::endl;
		client->setReadHeader(false);
		client->setWriteClient(true);
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

bool	ConnectionDispatcher::read_fd(int fd, Client * client, int & n, int idx)
{

	if (Data::setEvents()[idx].events & EPOLLIN)
	{
		client->clearRecvLine();
		n = recv(fd, client->getRecvLine(), MAXLINE - 1, MSG_DONTWAIT);
		return (true);
	}
	if (!client->check_timeout())
	{
		// REMOVE THIS CLIENT INSTANCE FROM CLIENTS MAP
		clients_remove_fd(client);
		Data::epollRemoveFd(client->getFd());
		delete client;
	}
	return (false);
}

void	ConnectionDispatcher::clients_remove_fd(Client* client)
{
	// WRITING TO CLIENT FD IS FROM NOW ON FORBIDDEN FOR THIS CLIENT INSTANCE
	clients[client->getFd()]->setWriteClient(false);
	
	// REMOVE THIS CLIENT INSTANCE FROM CLIENTS MAP
	clients.erase(client->getFd());
}

bool	ConnectionDispatcher::read_header(Client* client,  int idx)
{
	int			n = 0;
	int			peek = 0;

	// ON CONSTRUCTION READHEAD IS TRUE AND IS SET TO FALSE WHEN HEADER COMPLETELY READ
	if (!client->getReadHeader())
		return (true);

	// CHECK IF WE ARE ALLOWED TO READ FROM CLIENT. IF YES READ, IF NO -> RETURN
	// ALSO REMOVES CLIENT ON TIMEOUT
	if (!read_fd(client->getFd(), client, n, idx))
		return (false);

	// SUCCESSFUL RECIEVE -> ADDING BUFFER FILLED BY RECIEVE TO THE MESSAGE STRING
	// peek in order to rule out the possibility to block on next read
	_concatMessageAndPeek(client, n, peek);

	// UNSUCCESSFUL RECIEVE AND INCOMPLETE HEADER 
	// REMOVE CLIENT FROM CLIENTS AND EPOLL. DELETE CLIENT. LOG ERR MSG
	if (!_checkReceiveError(client, n, peek))
		return (false);

	// IF return of receive is smaller than buffer -> SET READHEADER FLAG TO FALSE
	_checkEndHeader(client, n);
	return (true);
}

void	ConnectionDispatcher::write_client(Client* client,  int idx)
{
	if (Data::setEvents()[idx].events & EPOLLOUT)
	{
		bool result = client->getResponse()->sendResponse();
		if(result == true)
			Logger::info("The response  was sent successfully", true);
		else
			Logger::warning("Sending response had some error");
		clients_remove_fd(client);
		Data::epollRemoveFd(client->getFd());
		delete client;
	}
}

void	ConnectionDispatcher::_check_cgi(Client* client)
{
	if (!client->cgi_checked && client->getErrorCode() == 0)
	{
		client->cgi_checked= true;
		bool found = true;
		const ServerSettings*	clientServer = _serversInfo.getClientServer(*client);
		if (!clientServer)
			return ;

		ClientHeader* clientHeader = client->header;
		std::string ServerLocation = clientServer->getLocationURIfromPath(clientHeader->urlSuffix->getPath());
		std::vector<LocationSettings>::const_iterator it = clientServer->fetchLocationWithUri(ServerLocation, found);
		// Logger::warning(it->getLocationUri(), true);
		// Logger::warning("", true);
		// std::cout << found << std::endl;
		if (found == true && it->getLocationUri() == "/cgi-bin/")  //this can be changed in cofig maybe
		{ 
			Logger::warning("Cgi checked and it exist on this location", true);
			client->setCgi(new CgiProcessor(client));
		}
	}

}

bool	ConnectionDispatcher::run_cgi(Client* client)
{
	if (client->getCgi())
		if (client->getCgi()->process())
			return (true);
	return (false);
}

bool	ConnectionDispatcher::_isChildSocket(int fd)
{
	std::map<int, Client*>::iterator it = clients.begin();
	for (; it != clients.end(); it++)
	{
		if (it->second->socket_fromchild == fd)
			return (true);
		if (it->second->socket_tochild == fd)
			return (true);
	}
	return (false);
}

void	ConnectionDispatcher::_prepareChildSockets()
{
	std::map<int, Client*>::iterator it = clients.begin(); 
	for (; it != clients.end(); it++)
	{
		if (it->second->socketstatus_fromchild == DELETE)
		{
			Data::epollRemoveFd(it->second->socket_fromchild);
			close(it->second->socket_fromchild);
			// it->second->socket_fromchild = -1;
			it->second->socketstatus_fromchild = DELETED;
		}
		if (it->second->socketstatus_tochild == DELETE)
		{
			Data::epollRemoveFd(it->second->socket_tochild);
			close(it->second->socket_tochild);
			// it->second->socket_tochild = -1;
			it->second->socketstatus_tochild = DELETED;
		}
	  	if (it->second->socketstatus_fromchild == ADD)
		{
			Data::epollAddFd(it->second->socket_fromchild);
			it->second->socketstatus_fromchild = NONE;
		}
	  	if (it->second->socketstatus_tochild == ADD)
		{
			Data::epollAddFd(it->second->socket_tochild);
			it->second->clearMessage();
			it->second->socketstatus_tochild = NONE;
		}
	}
}

Client*	ConnectionDispatcher::findSocketClient(int socket)
{

	std::map<int, Client*>::iterator it = clients.begin(); 
	for (; it != clients.end(); it++)
	{
		if (it->second->socket_fromchild == socket)
			return (it->second);
		if (it->second->socket_tochild == socket)
			return (it->second);
	}
	return (NULL);
}

bool	ConnectionDispatcher::_handleChildSocket(int socket, size_t idx)
{
	// std::cout << "looking for socket: " << socket << std::endl;

	if (!_isChildSocket(socket))
		return (false);

	int n = 0;
	

	// if allowed to write -> write into socket

	// if no Client for this FD in map then return fatal error
	//
	Client* client = findSocketClient(socket);
	if (!client)
		return (std::cout << "no client for this socket, FATAL ERROR!", false);
	// client->clearMessage();
	
 	// write(_sockets[0], _client->getClientBody().c_str(), _client->getClientBody().size());
	if (!client->hasWrittenToCgi && Data::setEvents()[idx].events & EPOLLOUT && client->socket_tochild == socket)
	{
 		write(socket, "check this out\n", 15);
		client->hasWrittenToCgi = true;
		client->unsetsocket_tochild();
		return (true);
	}
	if (socket == client->socket_tochild)
		return (true);

	if (!client->hasReadFromCgi)
	{
		// CHECK IF WE ARE ALLOWED TO READ FROM CLIENT. IF YES READ, IF NO -> RETURN
		// ALSO REMOVES CLIENT ON TIMEOUT
		if (!read_fd(socket, client, n, idx))
			return (true);
		std::cout << "bytes read from child socket: " << n << std::endl;

		// if something was read it adds it to _message, peek if readsize = buffersize
		// to prevent block
		// _concatMessageAndPeek(client, n, peek);
		if (n > 0)
		{
			client->addRecvLineToCgiMessage();
			// if (n == MAXLINE - 1 && client->getCgiMessage().find("\r\n\r\n") == std::string::npos)
			// 	peek = recv(client->getFd(), client->getRecvLine(), MAXLINE, MSG_PEEK | MSG_DONTWAIT);
		}

		if (n < 0)
		{
			std::cout << "error: received, in handleChildSocket n: " << n << std::endl;
			return (true);
		}
		
		
		if (n < MAXLINE - 1)
		{
			client->hasReadFromCgi = true;
		}
	}
	if (client->waitreturn)
	{
		client->unsetsocket_fromchild();
		client->_cgi_output = client->getCgiMessage();
	}
	return (true);
}

void	ConnectionDispatcher::_handleClient(int idx)
{

	// CHECK WHETHER CLIENT FD CAN BE FOUND IN CLIENTS MAP AND RETURN CLIENT POINTER
	Client* client = find_client_in_clients(Data::setEvents()[idx].data.fd, clients);

	// READ_HEADER RETURNS FALSE WHEN ERR WHILE READING HEADER -> CLIENT IS DELETED
	if (!read_header(client, idx))
		return ;
	//Logger::info("Client message is "); std::cout << client->getMessage() << std::endl;
	// if (client->getCgi())
	// 	if (client->getCgi()->process()):
	// 		return ;
	
	// PROCESS HEADER
	if(client->getReadHeader() == false)
		client->createClientHeader();
	if(client->header != NULL)
	{
		if(client->header->isBodyExpected() == true)
		{
			// READ BODY
			// PROCESS BODY
		}

		//check cgi only if there is no error in client so far
		_check_cgi(client);


		run_cgi(client);
		if ( client->getCgi() && client->socketstatus_fromchild != DELETED)
			return ;
		

		// if cgi request -> run CGI PROCESSOR
		// if child not returned yet return;
		//
		// as soon as child returned write output of cgi script into string  
		//std::cout << "client address is : " << client << std::endl;
		// PROCESS ANSWER
		_processAnswer(*client);
	}

	// WRITE PROCESSED ANSWER TO CLIENT
	if (client->getWriteClient())
		write_client(client, idx);
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
	// create epoll fd
	Data::setEpollFd(epoll_create(1));

	// add server sockets to epoll listener
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


void ConnectionDispatcher::mainLoopEpoll()
{
	Logger::info("my pid is: "); std::cout << getpid() << std::endl;
	signal(SIGINT, handle_sigint);
	_addServerSocketsToEpoll();
	int nfds;
	while(true)
	{
		_prepareChildSockets();
		nfds = epoll_wait(Data::getEpollFd(), Data::setEvents(), MAX_EVENTS, MAX_WAIT);
		if (nfds == -1)
		{
			if(flag)
				Logger::info("Turn off procedure triggered", true);
			else
				Logger::error("Epoll wait failed", true);
			break;
		}
		for (size_t idx = 0; idx < static_cast<size_t>(nfds); ++idx)
		{
			if(_handleServerSocket(idx) == true)
				continue;
			if (_handleChildSocket(Data::setEvents()[idx].data.fd, idx))
				continue;
			_handleClient(idx);
		}
	}
}
