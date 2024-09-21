#include "ConnectionManager.hpp"
#include "../Utils/Logger.hpp"
#include "Socket.hpp"
#include "../Client/Client.hpp"
#include <csignal>
#include <sys/epoll.h>
#include <unistd.h> // FIXME. Used by Logger for getpid()

#define MAX_WAIT		-1 // 0: epoll runs in nonblocking way but CPU runs at 6,7 % 
#define MAX_EVENTS		40 // what happens if we exceed this?



void	ConnectionManager::_acceptNewClient(int listen_socket)
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

bool ConnectionManager::_handleServerSocket(struct epoll_event* events, int idx)
{
	if (!events)
		return (false);
	std::vector<Socket> serverSockets = Socket::getSockets();
	std::vector<Socket>::const_iterator it = serverSockets.begin();
	for(; it != serverSockets.end(); ++it)
	{
		if (events[idx].data.fd == it->getSocketFD())
		{
			_acceptNewClient(it->getSocketFD());
			return (true);
		}
	}
	return (false);
}

void	ConnectionManager::_epollLoop()
{
	struct epoll_event	events[MAX_EVENTS];
	Client* client;

	Logger::info("my pid is: ", getpid());
	// signal(SIGINT, handle_sigint);
	_addServerSocketsToEpoll();
	while(true)
	{
		// if (!_catchEpollErrorAndSignal())
			// break;
		int nfds = epoll_wait(_epollFd, events, MAX_EVENTS, MAX_WAIT);
		for (int idx = 0; idx < nfds && nfds != -1; ++idx)
		{
			if (_handleServerSocket(events, idx) == true)
				continue;
			if ((client = _isClient(Data::setEvents()[idx].data.fd)) != NULL)
				_handleClient(*client, idx);
		}
	}
}

void	ConnectionManager::ConnectionManagerLoop()
{

}

//==========================================================================//
// Constructor, Destructor and OCF Parts ===================================//
//==========================================================================//

// Custom Constructor
ConnectionManager::ConnectionManager(int epollFd, std::list<Client*>& clients) :
_epollFd(epollFd), _clients(clients) {}

// Destructor
ConnectionManager::~ConnectionManager()
{
	Socket::closeSockets();
}