#include "ConnectionDispatcher.hpp"
#include "Socket.hpp"
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
//#include "../Response/ServerResponse.hpp"
#include "../Utils/Logger.hpp"


//#include "../Parsing/ParsingUtils.hpp"

volatile sig_atomic_t flag = 0 ;

void handle_sigint(int sig)
{
	flag = 1;
	(void) sig;
	Logger::warning("CTRL + C cathced, Server is turning off"); std::cout <<std::endl;
	//std::cout << "Called custom ctrl + c function" << std::endl;
}

ConnectionDispatcher::ConnectionDispatcher(SocketManager& sockets, ServersInfo& serversInfo)
:_sockets(sockets), _serversInfo(serversInfo)
{
	_selectTimeout.tv_sec = 0;
	_selectTimeout.tv_usec = 0;
}
ConnectionDispatcher::ConnectionDispatcher(ConnectionDispatcher& source)
:_sockets(source._sockets), _serversInfo(source._serversInfo), _selectTimeout(source._selectTimeout),
_readSetMaster(source._readSetMaster), _writeSetMaster(source._writeSetMaster), _errorSetMaster(source._errorSetMaster)
{

}

ConnectionDispatcher& ConnectionDispatcher::operator=(ConnectionDispatcher& source)
{
	_sockets = source._sockets;
	_serversInfo = source._serversInfo;
	_selectTimeout = source._selectTimeout;
	return(*this);
}

ConnectionDispatcher::~ConnectionDispatcher()
{
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
	struct epoll_event	ev;
	int	client_fd;

	// ACCEPT RETURNS CLIENT FD
	client_fd = accept(listen_socket, (struct sockaddr *) NULL, NULL);
	if (client_fd == -1)
		throw std::runtime_error("accept error");

	// CREATE NEW CLIENT INSTANCE WITH CLIENT FD CONSTRUCTOR
	Client * newClient = new Client(client_fd, epollfd);

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
		std::cout << std::endl << client->getMessage() << std::endl;
		client->setReadHeader(false);
		client->setWriteClient(true);
	}
	return (true);
}

void	ConnectionDispatcher::write_client(struct epoll_event* events, std::map<int, Client *> & clients, Client* client,  int idx)
{
	std::string	answer = "HTTP/1.1 200 OK\r\n\r\nWebserv 0.0\n";

	if (events[idx].events & EPOLLOUT)
	{
		write(client->getFd(), answer.c_str(), answer.size());
		epoll_remove_client(events, clients, client);
		delete client;
	}
}


void	ConnectionDispatcher::handle_client(struct epoll_event* events, std::map<int, Client *> & clients, int idx)
{

	// CHECK WHETHER CLIENT FD CAN BE FOUND IN CLIENTS MAP AND RETURN CLIENT POINTER
	Client* client = find_client_in_clients(events[idx].data.fd, clients);

	// READ_HEADER RETURNS FALSE WHEN ERR WHILE READING HEADER -> CLIENT IS DELETED
	if (!read_header(events, clients, client, idx))
		return ;
	//Logger::info("Client message is "); std::cout << client->getMessage() << std::endl;
	
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
		//std::cout << "client address is : " << client << std::endl;
		//_processAnswer(*client);
		// PROCESS ANSWER
		//_generateClientResponse(0);
	}

	// WRITE PROCESSED ANSWER TO CLIENT
	if (client->getWriteClient())
		write_client(events, clients, client, idx);
}

void ConnectionDispatcher::_processAnswer(Client& client)
{
	Logger::info("Process answer for client", true);
	std::cout << "Client address is " << &client << std::endl;
	const ServerSettings& responseServer = _serversInfo.getServerByPort(client.header->getHostPort(), client.header->getHostName());
	Logger::info("Resposible server is ", true);
	std::cout << responseServer << std::endl;
}


std::vector<Socket> ConnectionDispatcher::_getAllReadyToReadSockets()
{
	std::vector<Socket> toReturn;
	std::vector<int> listenFD = _sockets.getAllListenFd();
	for(size_t i = 0 ; i < listenFD.size(); i++)
	{
		if(FD_ISSET(listenFD[i], &_readSetTemp))
		{
			Socket &oneSocket = _sockets.getSocketByFd(listenFD[i]);
			toReturn.push_back(oneSocket);
		}
	}
	return toReturn;
	
}

std::vector<int> ConnectionDispatcher::_getReadyToReadCommunicationFds()
{
	std::vector<int> toReturn;
	for(size_t i = 0 ; i < _communicationFds.size(); i++)
	{
		//std::cout << "I am here with fd "  <<_communicationFds[i] << std::endl;
		if(FD_ISSET(_communicationFds[i], &_readSetTemp))
		{
			//std::cout << "Putting " << _communicationFds[i] << "to read set" << std::endl;
			toReturn.push_back(_communicationFds[i]);
		}
	}
	return toReturn;
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

// void ConnectionDispatcher::_clientFdCheck(int communicationFd)
// {
// 	std::vector<int>::iterator it = std::find(_communicationFds.begin(), _communicationFds.end(), communicationFd);
// 	if(it != _communicationFds.end())
// 	{
// 		_clientHeaders.removeClient(communicationFd);
// 		_communicationFds.erase(it);
// 	}
// }

// void ConnectionDispatcher::_handleAllReadySockets(std::vector<Socket>& readySockets)
// {
// 	if(readySockets.empty() == true)
// 	{
// 		std::cerr<< "No ReadySockets to handle" << std::endl;
// 		return;
// 	}
// 	for(size_t i =0; i < readySockets.size(); i++)
// 	{
// 		Socket& ready = readySockets[i];
// 		Logger::info("Ready socket is ");
// 		//std::cout << "Ready socket is " << std::endl;
// 		std::cout << ready << std::endl;
// 		int communicationSocket = ready.getCommunicationSocket();
// 		//check if communication socket already exist in vector, if 
// 		//it exist that means that client closed connection and i should removed that client first
// 		_clientFdCheck(communicationSocket); 
// 		_communicationFds.push_back(communicationSocket);
// 		FD_SET(communicationSocket, &_readSetMaster);
// 		//std::cout << "put fd: " << communicationSocket << " in set for reading" << std::endl;
// 	}
// }



// void ConnectionDispatcher::_generateClientResponse(int communictaionFD)
// {
// 	ClientHeader& header(_clientHeaders.getClientHeader(communictaionFD));
// 	Logger::info("error code is :"); std::cout << header.getErrorCode() << std::endl;
// 	const ServerSettings& serverRef = _serversInfo.getServerByPort(header.getHostPort(), header.getHostName());

// 	Response respones(header, serverRef);
// 	Logger::info("Created Response with client header and serverRef", true);
// 	respones.sendResponse();
// }

// void ConnectionDispatcher::_handleAllReadyToReadCommunicationFds
// (std::vector<int>& readReadyClientFds)
// {
// 	if(readReadyClientFds.empty() == true)
// 	{
// 		std::cout << "0 Ready clients to read" << std::endl;
// 		return;
// 	}
// 	for(size_t i = 0; i < readReadyClientFds.size(); i++)
// 	{
// 		//procitaj fd i spremi ga u request
// 		//ocitaj host i nadi odgovarajuci server 
// 		//pospremi Serve i commmunication socket u klasu il negdje
// 		//makni taj communication iz readReady i metni ga u write ready


// 		int communicationSocket = readReadyClientFds[i];
// 		//TO DO. Maybe should try catch _clientHeader readClientHeader
// 		//cath INVALID HEADER EXCEPTION AND GENERATE ERROR RESPONSE
// 		ReadStatus status =  _clientHeaders.readClientHeader(communicationSocket);
// 		if(status == ERROR)
// 		{
// 			std::cout << "ERROR while reading. CLIENT CLOSED connection OR ERRRO" << std::endl;
// 			_removeClient(communicationSocket);
// 		}
// 		if(status == DONE)
// 		{
// 			Logger::info("Fully read client with fd: "); std::cout << communicationSocket << std::endl;
// 			_generateClientResponse(communicationSocket);
// 			//generate response 
// 			//save response to vector
// 			//add fd to ready to write
// 			//
// 			FD_CLR(communicationSocket, &_readSetMaster);
// 		}
// 		std::cout << "FERTIG" << std::endl;
// 		_removeClient(communicationSocket);
// 		//close(communicationSocket);
// 	}

// }

/*
	findwhichfd is ready
	if one of socket fd is ready
		//socket ready Function
		//accept connection
		//store it somewhere
		//set it to readFDMaseter
*/
// void ConnectionDispatcher::_handleReadyFd(void)
// {
	
// 	//how i can now that ther is nothing to be read form socket fd

// 	std::vector<Socket> readySockets = _getAllReadyToReadSockets();
// 	if(readySockets.size() != 0)
// 	{
// 		_handleAllReadySockets(readySockets);
// 		//handle all Sockets;
// 	}
// 	std::vector<int> readyReadClientFd = _getReadyToReadCommunicationFds();
// 	if(readyReadClientFd.size() != 0)
// 	{
// 		_handleAllReadyToReadCommunicationFds(readyReadClientFd);
// 		//ParsingUtils::printVector(readyReadClientFd);
// 	}
// }

int ConnectionDispatcher::_getMaxFd(void) const 
{
	int maxFD = _sockets.getMaxSocketFd();
	for(size_t i = 0 ; i<_communicationFds.size(); i++)
	{
		if(_communicationFds[i] > maxFD)
			maxFD = _communicationFds[i];
	}
	return maxFD;
}

void ConnectionDispatcher::_closeAllClients(void)
{
	std::cout << "Call function _removeAllClients" << std::endl;
	//size_t vectorSize = _communicationFds.size();
	for(size_t i = 0; i < _communicationFds.size(); i++)
	{
		int fdToClose = _communicationFds[i];
		close(fdToClose);
	}
	//_removeClient(5);
	// for(size_t i=0; i < _communicationFds.size(); i++)
	// {
	// 	std::cout << "Removing client" << _communicationFds[i] << std::endl;
	// 	_removeClient(_communicationFds[i]);
	// }
}

// void ConnectionDispatcher::_removeClient(int clientFd)
// {
// 	std::vector<int>::iterator it = std::find(_communicationFds.begin(),
// 			_communicationFds.end(), clientFd);
// 	_clientHeaders.removeClient(clientFd);
// 	if(it != _communicationFds.end())
// 	{
// 		_communicationFds.erase(it);
// 	}
// 	FD_CLR(clientFd, &_readSetMaster);
// 	FD_CLR(clientFd, &_writeSetMaster);
// 	FD_CLR(clientFd, &_errorSetMaster);
// 	close(clientFd);
// }

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
	signal(SIGINT, handle_sigint);
	_addServerSocketsToEpoll();
	int nfds;
	Logger::warning("I am aabotu to start loop", true);
	std::cout<<"my pid is: " << getpid() << std::endl;
	while(true)
	{
		//Logger::warning("I am stuck", true);
		if(flag)
		{
			Logger::info("Turn off procedure triggered"); std::cout<<std::endl;
			//std::cout << "Ctrl + c pressed" << std::endl;
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
		_notStuckMessage();
	}

}

void ConnectionDispatcher::mainLoop(void)
{
	//only those go in select 

	signal(SIGINT, handle_sigint); 

	//_setAllServerListenSocketsForRead();
	//FD_SET all socketListen fds to those
	while(true)
	{
		if(flag)
		{
			Logger::info("Turn off procedure triggered"); std::cout<<std::endl;
			//std::cout << "Ctrl + c pressed" << std::endl;
			break;
		}


		//HERE SHOULD GO POLL VERSION OF LOOP
		//poll run 

		int retVal = 2;
		//select(selectMaxFD + 1, &_readSetTemp, &_writeSetTemp, &_errorSetTemp, &_selectTimeout);
		if(retVal == -1)
		{
			std::cout << "Select Failed" << std::endl;
		}
		else if(retVal == 0)
		{
			//std::cout << "Nothing is ready yet" << std::endl;
			//nothing is ready
		}
		else 
		{
			//_handleReadyFd();
			// std::vector<int> toREad = _sockets.getAllListenFd();
			// void *buffer[1024];
			// read(toREad[0], buffer, 1024);
			//something is ready
			// read
			// move it from current read to next write
		}
		//_notStuckMessage();
	}
	_closeAllClients();
	Logger::warning("SERVER IS TURNED OFF"); std::cout<<std::endl;
	//std::cout << "I am here" << std::endl;
	

}
