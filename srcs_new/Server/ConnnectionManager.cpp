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

// Helper function for to add fd to epoll
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

// Function for _epollLoop() to accept new client and add it to epoll
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

	if (_clients.find(clientFd) != _clients.end())
	{
		Logger::error("F@ck Found duplicate fd in clients map", _clients[clientFd]->getId());
		delete _clients[clientFd];
	}
	_clients[clientFd] = newClient;

	// ADD CLIENT FD TO THE LIST OF FDS THAT EPOLL IS WATCHING FOR ACTIVITY
	epollAddFd(_epollFd, clientFd);
}

// Function for _epollLoop() to add server sockets to epoll
void		ConnectionManager::_addServerSocketsToEpoll()
{
	int ret = 0;
	std::vector<Socket> sockets = Socket::getSockets();
	std::vector<Socket>::const_iterator it = sockets.begin();
	for(; it != sockets.end(); ++it)
	{
		int fd = it->getSocketFD();
		Logger::info("Adding to epoll this server socket: ", fd);
		ret = epollAddFd(_epollFd, fd);
		if (ret == -1)
			throw std::runtime_error("epoll_ctl error: adding file descriptor to epoll failed");
	}
}

// Helper function to check if fd belongs to a server socket
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

// Helper function to check if fd belongs to a client;if yes then returns Client
static Client*		isClient(int fd, std::map<int, Client*>& clients)
{
	std::map<int, Client*>::const_iterator it = clients.find(fd);
	if (it == clients.end())
		return (NULL);
	return (it->second);
}

void	ConnectionManager::_updateClientFd(Client& client, const int& idx, const struct epoll_event* events)
{
	if (client.getClientState() == Client::DELETEME)
		return ;
	bool allowedToSend = events[idx].events & EPOLLOUT;
	bool allowedToReceive = events[idx].events & EPOLLIN;
	if (allowedToSend && !allowedToReceive)
		client.setClientFdState(0, Client::R_SEND);
	if (!allowedToSend && allowedToReceive)
		client.setClientFdState(0, Client::R_RECEIVE);
	if (allowedToSend && allowedToReceive)
		client.setClientFdState(0, Client::R_SENDREC);
}

void	ConnectionManager::_epollLoop()
{
	struct epoll_event	events[MAX_EVENTS];
	Client* client;

	Logger::info("my pid is: ", getpid());
	// signal(SIGINT, handle_sigint);
	// _addServerSocketsToEpoll();
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
				_updateClientFd(*client, idx, events);

			// handle CGI Sockets
				// add/remove CGI Sockets from/to epoll
				// update CGI Sockets


			// delete Clients 
				// remove Client Fds
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
_epollFd(epollFd), _clients(clients)
{
	_addServerSocketsToEpoll();
}

// Destructor
ConnectionManager::~ConnectionManager()
{
	Socket::closeSockets();
}