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
	close(epollfd);
}

void ConnectionDispatcher::epoll_add_listen_sock(int listen_sock)
{
	// STRUCT NEEDED FOR EPOLL TO SAVE FLAGS INTO (SETTINGS)
	struct epoll_event	ev; 

	// SETTING UP EV EVENTS 'SETTINGS' STRUCT FOR LISTEN_SOCKET
	ev.events = EPOLLIN | EPOLLOUT;				
	ev.data.fd = listen_sock;

	// ADDING LISTEN_SOCKET TO EPOLL WITH THE EV 'SETTINGS' STRUCT
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) 
		throw std::runtime_error("epoll_ctl_add - listen socket error");
}

/* CREATE CLIENT FD BY CALLING ACCEPT ON LISTEN SOCKET, CREATE CLIENT INSTANCE
ADD INSTANCE TO CLIENTS MAP. MAP KEY: CLIENT FD, MAP VALUE: CLIENT INSTANCE POINTER */
void	ConnectionDispatcher::epoll_add_client(int epollfd, int listen_socket)
{
	struct sockaddr client_addr;
	struct epoll_event	ev;
	socklen_t addrlen;
	int	client_fd;

	// ACCEPT RETURNS CLIENT FD
	client_fd = accept(listen_socket, &client_addr, &addrlen);
	if (client_fd == -1)
		throw std::runtime_error("accept error");

	// CREATE NEW CLIENT INSTANCE WITH CLIENT FD CONSTRUCTOR
	Client * newClient = new Client(client_fd, epollfd, _envp, client_addr);
	newClient->setAddrlen(addrlen);

	// IF CLIENT FD ALREADY EXISTS IN MAP, THEN SET NOWRITE IN THE CLIENT INSTANCE.
	// WE DON'T IMMEADIATELY DELETE CLIENT BECAUSE IT MIGHT BE PROCESSING.
	// IN THAT CASE THE  CLIENT INSTANCE NEEDS TO FINISH ITS JOB
	// AND HAS TO BE DELETED -> THIS CASE IS NOT YET TAKEN CARE OF
	// RIGHT NOW MEMORY LEAK!!! ADD TO GRAVEYARD VECTOR??
	if (clients.find(client_fd) != clients.end())
		clients[client_fd]->setWriteClient(false);
	clients[client_fd] = newClient;

	// ADD CLIENT FD TO THE LIST OF FDS THAT EPOLL IS WATCHING FOR ACTIVITY
	ev.events = EPOLLIN | EPOLLOUT;
	ev.data.fd = client_fd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
		throw std::runtime_error("epoll_ctl - conn_socket error");
}

void	ConnectionDispatcher::epoll_remove_client(struct epoll_event* events, std::map<int, Client*> & clients, Client* client)
{
	// WRITING TO CLIENT FD IS FROM NOW ON FORBIDDEN FOR THIS CLIENT INSTANCE
	clients[client->getFd()]->setWriteClient(false);
	
	// REMOVE THIS CLIENT INSTANCE FROM CLIENTS MAP
	clients.erase(client->getFd());

	// REMOVE THE FD OF THIS CLIENT INSTANCE FROM EPOLLS WATCH LIST
	epoll_ctl(client->getEpollFd(), EPOLL_CTL_DEL, client->getFd(), events);
	
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

bool	ConnectionDispatcher::read_client(struct epoll_event* events, std::map<int, Client *> & clients, Client * client, int & n, int idx)
{

	if (events[idx].events & EPOLLIN)
	{
		n = recv(client->getFd(), client->getRecvLine(), MAXLINE - 1, MSG_DONTWAIT);
		return (true);
	}
	if (!client->check_timeout())
	{
		epoll_remove_client(events, clients, client);
		delete client;
	}
	return (false);
}

bool	ConnectionDispatcher::read_header(struct epoll_event* events, std::map<int, Client *> & clients, Client* client,  int idx)
{
	int			n = 0;
	int			peek = 0;

	// ON CONSTRUCTION READHEAD IS TRUE AND IS SET TO FALSE WHEN HEADER COMPLETELY READ
	if (!client->getReadHeader())
		return (true);

	// CHECK IF WE ARE ALLOWED TO READ FROM CLIENT. IF YES READ, IF NO -> RETURN
	// ALSO REMOVES CLIENT ON TIMEOUT
	if (!read_client(events, clients, client, n, idx))
		return (false);

	// SUCCESSFUL RECIEVE -> ADDING BUFFER FILLED BY RECIEVE TO THE MESSAGE STRING
	if (n > 0)
	{
		client->addRecvLineToMessage();
		if (n == MAXLINE && client->getMessage().find("\r\n\r\n") == std::string::npos)
			peek = recv(client->getFd(), client->getRecvLine(), MAXLINE, MSG_PEEK | MSG_DONTWAIT);
	}

	// UNSUCCESSFUL RECIEVE AND INCOMPLETE HEADER 
	// REMOVE CLIENT FROM CLIENTS AND EPOLL. DELETE CLIENT. LOG ERR MSG
	if (n <= 0 || peek < 0)
	{
		epoll_remove_client(events, clients, client);
		delete client;
		if (n < 0 || peek < 0)
			std::cout << "error: recieve from client, incomplete header" << std::endl;
		return (false);
	}

	// IF END OF HEADER DETECTED IN MESSAGE -> SET READHEADER FLAG TO FALSE
	if (n <= MAXLINE && client->getMessage().find("\r\n\r\n") != std::string::npos)
	{
		//std::cout << std::endl << client->getMessage() << std::endl;
		client->setReadHeader(false);
		client->setWriteClient(true);
	}
	return (true);
}

void	ConnectionDispatcher::write_client(struct epoll_event* events, std::map<int, Client *> & clients, Client* client,  int idx)
{
	//std::string	answer = "HTTP/1.1 200 OK\r\n\r\nWebserv 0.0\n";

	if (events[idx].events & EPOLLOUT)
	{
		//write(client->getFd(), answer.c_str(), answer.size());
		bool result = client->getResponse()->sendResponse();
		if(result == true)
			Logger::info("The response  was sent successfully", true);
		else
			Logger::warning("Sending response had some error");
		epoll_remove_client(events, clients, client);
		delete client;
	}
}

void	ConnectionDispatcher::_check_cgi(Client* client)
{
	if (!client->cgi_checked)
	{
		client->cgi_checked= true;
		bool found = true;
		const ServerSettings*	clientServer = _serversInfo.getClientServer(*client);
		if (!clientServer)
			return ;

		ClientHeader* clientHeader = client->header;
		std::string ServerLocation = clientServer->getLocationPartOfUrl(clientHeader->getRequestedUrl());
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

void	ConnectionDispatcher::handle_client(struct epoll_event* events, std::map<int, Client *> & clients, int idx)
{

	// CHECK WHETHER CLIENT FD CAN BE FOUND IN CLIENTS MAP AND RETURN CLIENT POINTER
	Client* client = find_client_in_clients(events[idx].data.fd, clients);

	// READ_HEADER RETURNS FALSE WHEN ERR WHILE READING HEADER -> CLIENT IS DELETED
	if (!read_header(events, clients, client, idx))
		return ;
	//Logger::info("Client message is "); std::cout << client->getMessage() << std::endl;
	// if (client->getCgi())
	// 	if (client->getCgi()->process())
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
		_check_cgi(client);
		// IF RUN_CGI RETURNS TRUE WE ARE WAITING FOR CHILD TO RETURN
		if (run_cgi(client))
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
		write_client(events, clients, client, idx);
}

void ConnectionDispatcher::_processAnswer(Client& client)
{
	Logger::info("Process answer for client: ");std::cout <<client.getId() << std::endl;  
	const ServerSettings* const responseServer = _serversInfo.getClientServer(client);

	//const ServerSettings& responseServer = _serversInfo.getServerByPort(client.header->getHostPort(), client.header->getHostName());
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
		Logger::error("Trying to create response for clinet that already have one", true);
		return;
	}
	Response* response = new Response(client, responseServer);
	client.setResponse(response);
	Logger::info("Response created ", true);
}


void ConnectionDispatcher::_addServerSocketsToEpoll(void)
{
	epollfd = epoll_create(1);
	if (epollfd == -1)					
		throw std::runtime_error("epoll create failed");
	std::vector<int> listenFd = _sockets.getAllListenFd();
	for(size_t i = 0; i < listenFd.size(); i++)
	{
		epoll_add_listen_sock(listenFd[i]);
	}
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


bool ConnectionDispatcher::_acceptClient(size_t idx)
{
	for(size_t i = 0; i < _sockets.getAllListenFd().size(); i++)
	{
		if (events[idx].data.fd == _sockets.getAllListenFd()[i])
		{
			epoll_add_client(epollfd, _sockets.getAllListenFd()[i]);
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
		if(flag)
		{
			Logger::info("Turn off procedure triggered"); std::cout<<std::endl;
			break;
		}
		nfds = epoll_wait(epollfd, events, MAX_EVENTS, MAX_WAIT);
		if (nfds == -1)
		{
			Logger::error("Epoll wait failed");
			break;
		}
		for (size_t idx = 0; idx < static_cast<size_t>(nfds); ++idx)
		{
			if(_acceptClient(idx) == true)
				continue;
			handle_client(events, clients, idx);
		}
	}

}
