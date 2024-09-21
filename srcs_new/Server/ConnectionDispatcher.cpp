#include "ConnectionDispatcher.hpp"
#include "Socket.hpp"
#include "../Client/Client.hpp"
#include "Data.hpp"
#include <stdexcept>
#include <csignal>
#include <unistd.h> // DELETE. This is for Logger. getpid()

#define MAX_WAIT	-1 // 0: epoll runs in nonblocking way but CPU runs at 6,7 % 
#define MAX_EVENTS	40 // what happens if we exceed this?

volatile sig_atomic_t flag = 0 ;

void handle_sigint(int)
{
	flag = 1;
	Logger::warning("CTRL + C caught, Server is turning off", "");
}

static bool	_stopByEpollError(int& _nfds)
{
	if (_nfds == -1)
	{
		Logger::error("Epoll wait failed", true);
		return (true);
	}
	return (false);
}

bool	ConnectionDispatcher::_stopBySigInt()
{
	if (flag)
	{
		if (flag == 1)
		{
			signal(SIGINT, SIG_IGN);
			flag++;
			Logger::info("Turn off procedure triggered", true);
			_shutdownCgiChildren();
		}
		if (flag && _clients.size() != 0)
			return (false);
		return (true);
	}
	return (false);
}

//REGULAR METHODS===========================================================//

/* CREATE CLIENT FD BY CALLING ACCEPT ON LISTEN SOCKET, CREATE CLIENT INSTANCE
ADD INSTANCE TO CLIENTS MAP. MAP KEY: CLIENT FD, MAP VALUE: CLIENT INSTANCE POINTER */
// void	ConnectionDispatcher::_epoll_accept_client(int listen_socket)
static void	_epollAcceptClient(const int listen_socket, std::map<int, Client* >& clients)
{
	struct sockaddr client_addr;
	socklen_t addrlen = sizeof(client_addr);
	int	clientfd;

	// ACCEPT RETURNS CLIENT FD
	clientfd = accept(listen_socket, &client_addr, &addrlen);
	if (clientfd == -1)
		throw std::runtime_error("accept error");

	// CREATE NEW CLIENT INSTANCE WITH CLIENT FD CONSTRUCTOR
	Client* newClient = new Client(clientfd, client_addr, addrlen);

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

// Method to accept incomming conection of new clients
bool ConnectionDispatcher::_handleServerSocket(size_t idx)
{
	for(size_t i = 0; i < Socket::getSocketFDs().size(); i++)
	{
		if (Data::setEvents()[idx].data.fd == Socket::getSocketFDs()[i])
		{
			_epollAcceptClient(Socket::getSocketFDs()[i], _clients);
			return true;
		}
	}
	return false;
}

void ConnectionDispatcher::_epollLoop()
{
	Client* client;

	Logger::info("my pid is: ", getpid());
	signal(SIGINT, handle_sigint);
	_addServerSocketsToEpoll();
	while(true)
	{
		if (_stopByEpollError(_nfds) || _stopBySigInt())
			break;
		_nfds = epoll_wait(_epollfd, Data::setEvents(), MAX_EVENTS, MAX_WAIT);
		for (int idx = 0; idx < _nfds && _nfds != -1; ++idx)
		{
			if (_handleServerSocket(idx) == true)
				continue;
			if ((client = _isClient(Data::setEvents()[idx].data.fd)) != NULL)
				_handleClient(*client, idx);
		}
	}
}
