#include "ConnectionManager.hpp"
#include "../Utils/Logger.hpp"
#include "Socket.hpp"
#include "../Client/Client.hpp"
#include <csignal>
#include <sys/epoll.h>
#include <unistd.h> // FIXME. Used by Logger for getpid()
#include <vector>

#define MAX_WAIT		-1 // 0: epoll runs in nonblocking way but CPU runs at 6,7 % 
#define MAX_EVENTS		40 // what happens if we exceed this?

//==========================================================================//
// REGULAR METHODS==========================================================//
//==========================================================================//

// Helper function for _epollLoop() to add server sockets to epoll
static int		epollAddFd(int& epollFd, int& fd)
{
	int ret;
	// STRUCT NEEDED FOR EPOLL TO SAVE FLAGS INTO (SETTINGS)
	struct epoll_event	ev;

	// SETTING UP EV EVENTS 'SETTINGS' STRUCT FOR LISTEN_SOCKET
	ev.events = EPOLLIN | EPOLLOUT;
	ev.data.fd = fd;

	// ADDING LISTEN_SOCKET TO EPOLL WITH THE EV 'SETTINGS' STRUCT
	ret = epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev);
	return (ret);
}

void	ConnectionManager::_acceptNewClient(int listen_socket)
{
	struct sockaddr client_addr;
	socklen_t addrlen = sizeof(client_addr);
	int	clientFd;

	// ACCEPT RETURNS CLIENT FD
	clientFd = accept(listen_socket, &client_addr, &addrlen);
	if (clientFd == -1)
		throw std::runtime_error("accept error");

	// CREATE NEW CLIENT INSTANCE WITH CLIENT FD CONSTRUCTOR
	Client * newClient = new Client(clientFd, client_addr, addrlen);

	// IF CLIENT FD ALREADY EXISTS IN MAP, THEN SET NOWRITE IN THE CLIENT INSTANCE.
	// WE DON'T IMMEADIATELY DELETE CLIENT BECAUSE IT MIGHT BE PROCESSING.
	// IN THAT CASE THE  CLIENT INSTANCE NEEDS TO FINISH ITS JOB
	// AND HAS TO BE DELETED -> THIS CASE IS NOT YET TAKEN CARE OF
	// RIGHT NOW MEMORY LEAK!!! ADD TO GRAVEYARD VECTOR??
	if (_clients.find(clientFd) != _clients.end())
	{
		// _clients[clientFd]->setWriteClient(false);
		// _clients[clientFd]->setClientState(Client::DELETE);
		Logger::error("F@ck Found duplicate fd in clients map", _clients[clientFd]->getId());
		delete _clients[clientFd];
	}
	_clients[clientFd] = newClient;

	// ADD CLIENT FD TO THE LIST OF FDS THAT EPOLL IS WATCHING FOR ACTIVITY
	// Data::epollAddFd(clientfd);
	epollAddFd(_epollFd, clientFd);
}

// bool ConnectionManager::_handleServerSocket(struct epoll_event* events, int idx)
// {
// 	if (!events)
// 		return (false);
// 	std::vector<Socket> serverSockets = Socket::getSockets();
// 	std::vector<Socket>::const_iterator it = serverSockets.begin();
// 	for(; it != serverSockets.end(); ++it)
// 	{
// 		if (events[idx].data.fd == it->getSocketFD())
// 		{
// 			_acceptNewClient(it->getSocketFD());
// 			return (true);
// 		}
// 	}
// 	return (false);
// }

// Helper function for _epollLoop() to add server sockets to epoll
void		ConnectionManager::_addServerSocketsToEpoll()
{
	// std::vector<int> socketFds = Data::getServerSocketFds();
	// struct epoll_event	ev;
	// ev.events = EPOLLIN | EPOLLOUT;
	int ret = 0;
	std::vector<Socket> sockets = Socket::getSockets();
	std::vector<Socket>::const_iterator it = sockets.begin();
	for(; it != sockets.end(); ++it)
	{
		int fd = it->getSocketFD();
		Logger::info("Adding to epoll this server socket: ", fd);
		// ev.data.fd = fd;
		// if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev) == -1)
		ret = epollAddFd(_epollFd, fd);
		if (ret == -1)
			throw std::runtime_error("epoll_ctl error: adding file descriptor to epoll failed");
	}
}

static bool		isServerSocket(int fd)
{
	std::vector<Socket> serverSockets = Socket::getSockets();
	std::vector<Socket>::const_iterator it = serverSockets.begin();
	for(; it != serverSockets.end(); ++it)
	{
		if (fd == it->getSocketFD())
			return (true);
	}
	return (false);
}

static Client*		isClient(int fd, std::map<int, Client*>& clients)
{
	// std::map<int, Client*>::iterator it = _clients.find(fd);
	std::map<int, Client*>::const_iterator it = clients.find(fd);
	if (it == clients.end())
		return (NULL);
	return (it->second);
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
			if (isServerSocket(events[idx].data.fd))
			{
				_acceptNewClient(events[idx].data.fd);
				continue;
			}
			if ((client = isClient(events[idx].data.fd, _clients)) != NULL)
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
// ConnectionManager::ConnectionManager(int epollFd, std::list<Client*>& clients) :
ConnectionManager::ConnectionManager(int epollFd, std::map<int, Client*>& clients) :
_epollFd(epollFd), _clients(clients) {}

// Destructor
ConnectionManager::~ConnectionManager()
{
	Socket::closeSockets();
}