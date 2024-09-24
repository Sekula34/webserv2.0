#include "ConnectionManager.hpp"
#include "../Utils/Logger.hpp"
#include "Socket.hpp"
#include "../Client/Client.hpp"
// #include <csignal>
#include <map>
#include <sys/epoll.h>
#include <unistd.h> // FIXME. Used by Logger for getpid()
#include <vector>

#define MAX_WAIT		-1 // 0: epoll runs in nonblocking way but CPU runs at 6,7 % 
#define MAX_EVENTS		40 // what happens if we exceed this?

//==========================================================================//
// REGULAR METHODS==========================================================//
//==========================================================================//

// Helper function to add fd to epoll
static int		epollAddFd(const int& epollFd, const int& fd)
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

// Helper function to remove fd to epoll
static void	epollRemoveFd(const int& epollFd, const int& fd, struct epoll_event* events)
{
	// REMOVE THE FD OF THIS CLIENT INSTANCE FROM EPOLLS WATCH LIST
	if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, events) == -1)
		Logger::error("The following FD could not be removed from epoll: ", fd);
		// throw std::runtime_error("epoll_ctl error: removing file descriptor from epoll failed");
}

// Method for _epollLoop() to accept new client and add it to epoll
void	ConnectionManager::_acceptNewClient(int listen_socket)
{
	struct sockaddr client_addr;
	socklen_t addrlen = sizeof(client_addr);
	int	clientFd;

	// ACCEPT RETURNS CLIENT FD
	clientFd = accept(listen_socket, &client_addr, &addrlen);
	if (clientFd == -1)
	{
		Logger::error("Failed to accept new Client on socket fd: ", listen_socket);
		return ;
		// throw std::runtime_error("accept error");
	}

	// CREATE NEW CLIENT INSTANCE WITH CLIENT FD CONSTRUCTOR
	Client* newClient = NULL;
	try
	{
		newClient = new Client(clientFd, client_addr, addrlen);
	}
	catch (std::exception& e)
	{
		Logger::error("Failed newClient: ", e.what());
		return ;
	}

	// if (_clients.find(clientFd) != _clients.end()) // This should never happen
	// {
	// 	Logger::error("F@ck Found duplicate fd in clients map", _clients[clientFd]->getId());
	// 	delete _clients[clientFd];
	// }

	// ADD CLIENT FD TO THE LIST OF FDS THAT EPOLL IS WATCHING FOR ACTIVITY
	int ret = epollAddFd(_epollFd, clientFd);
	if (ret == -1)
		delete newClient;

	// ADDING NEWLY CREATED CLIENT TO _CLIENTS MAP
	// _clients[clientFd] = newClient;
}

// Method for _epollLoop() to add server sockets to epoll
void		ConnectionManager::_addServerSocketsToEpoll()
{
	int ret = 0;
	std::vector<Socket> sockets = Socket::getSockets();
	std::vector<Socket>::const_iterator it = sockets.begin();
	for(; it != sockets.end(); ++it)
	{
		int fd = it->getSocketFd();
		Logger::info("Adding to epoll this server socket: ", fd);
		ret = epollAddFd(_epollFd, fd);
		if (ret == -1)
			throw std::runtime_error("epoll_ctl error: adding file descriptor to epoll failed");
	}
}

// Helper function for _epollLoop() to check if fd belongs to a server socket
static bool		isServerSocket(int fd)
{
	std::vector<Socket> serverSockets = Socket::getSockets();
	std::vector<Socket>::const_iterator it = serverSockets.begin();
	for(; it != serverSockets.end(); ++it)
	{
		if (fd == it->getSocketFd())
			return (true);
	}
	return (false);
}

// Helper function for _epollLoop() to check if fd belongs to a client; if yes then returns Client
static Client*		isClient(int fd, std::map<int, Client*>& clients)
{
	std::map<int, Client*>::const_iterator it = clients.find(fd);
	if (it == clients.end())
		return (NULL);
	return (it->second);
}

// static void	updateClientFd(Client& client, const int& idx, const struct epoll_event* events)
// {
// 	bool allowedToSend = events[idx].events & EPOLLOUT;
// 	bool allowedToReceive = events[idx].events & EPOLLIN;
// 	if (allowedToSend && !allowedToReceive)
// 		client.setClientFdState(0, Client::R_SEND);
// 	if (!allowedToSend && allowedToReceive)
// 		client.setClientFdState(0, Client::R_RECEIVE);
// 	if (allowedToSend && allowedToReceive)
// 		client.setClientFdState(0, Client::R_SENDREC);
// }

// TODO: Decalre an enum to be more verbose for idx of clientFds
static void	updateClientFds(Client& client, const int& epollIdx, const struct epoll_event* events)
{
	const int fd = events[epollIdx].data.fd;
	FdData& fdData = client.getFdDataByFd(fd); 
	bool allowedToSend = events[epollIdx].events & EPOLLOUT;
	bool allowedToReceive = events[epollIdx].events & EPOLLIN;
	if (allowedToSend && !allowedToReceive)
		fdData.state = FdData::R_SEND;
		// client.setClientFdState(fd, Client::R_SEND);
	if (!allowedToSend && allowedToReceive)
		fdData.state = FdData::R_RECEIVE;
		// client.setClientFdState(fd, Client::R_RECEIVE);
	if (allowedToSend && allowedToReceive)
		fdData.state = FdData::R_SENDREC;
		// client.setClientFdState(fd, Client::R_SENDREC);
}

// Method for _epollLoop() to update Client Fd state (e_fdState)
void	ConnectionManager::_handleClient(Client& client, const int& idx, struct epoll_event* events)
{
	if (client.getClientState() == Client::DELETEME)
	{
		// std::map<int, Client*>::iterator it = _clients.find(client.getClientFd());
		// _clients.erase(it);

		// epollRemoveFd(_epollFd, client.getClientFd(), events);
		epollRemoveFd(_epollFd, client.getFdDataByType(FdData::CLIENT_FD).fd, events);
		delete &client; // delete needs an address
		return ;
	}

	updateClientFds(client, idx, events);
	// bool allowedToSend = events[idx].events & EPOLLOUT;
	// bool allowedToReceive = events[idx].events & EPOLLIN;
	// if (allowedToSend && !allowedToReceive)
	// 	client.setClientFdState(0, Client::R_SEND);
	// if (!allowedToSend && allowedToReceive)
	// 	client.setClientFdState(0, Client::R_RECEIVE);
	// if (allowedToSend && allowedToReceive)
	// 	client.setClientFdState(0, Client::R_SENDREC);
}

// void		ConnectionManager::_handleCgiFds(Client& client, const int& idx, struct epoll_event* events)
// {
// 	int targetFd = events[idx].data.fd;
// 	// std::map<int, Client*>::iterator it = _clients.find(client.getClientFd());
// 	std::map<int, Client*>::iterator it = _clients.begin();
// 	for (; it != _clients.end(); ++it)
// 	{
// 		Client& currentClient = *(it->second);
// 		Client::fdPairsVec::iterator itFds = currentClient.getClientFds().begin();
// 		for (int i = 0; itFds != currentClient.getClientFds().end(); ++itFds, ++i)
// 		{
// 			if (targetFd == itFds->first)
// 			{
// 				if (i == 0)
// 					Logger::error("We are trying to change client fd. this should not happen in client with id: ", currentClient.getId());
// 				if (itFds->second == Client::CLOSE)
// 				{
// 					epollRemoveFd(_epollFd, targetFd, events);
// 					close(targetFd);
// 					itFds->second = Client::CLOSED;
// 					return ;
// 				}
// 				if (itFds->second == Client::NONE)
// 				{
// 					epollAddFd(_epollFd, targetFd);
// 					return;
// 				}
// 				updateClientFds(currentClient, i, idx, events);
// 				return ;
// 			}
// 		}
// 	}
// 	Logger::error("Could not find targetFd in any client (while trying to set cgi socket state): ", targetFd);
// }

// void		ConnectionManager::_handleCgiFds(const int& idx, struct epoll_event* events)
// {
// 	int targetFd = events[idx].data.fd;
// 	// std::map<int, Client*>::iterator it = _clients.find(client.getClientFd());
// 	std::map<int, Client*>::iterator it = _clients.begin();
// 	for (; it != _clients.end(); ++it)
// 	{
// 		Client& currentClient = *(it->second);
// 		//Client::fdPairsMap::iterator itFds = currentClient.getClientFds().begin();
// 		Client::fdPairsMap::iterator fdIt = currentClient.getClientFds().find(targetFd);
// 		if(fdIt == currentClient.getClientFds().end())
// 			continue ;
// 		const int& fd = fdIt->first;
// 		Client::fdTypeStatePair& tyteState = fdIt->second;
// 		Client::e_clientFdType& fdType = tyteState.first;
// 		Client::e_fdState& fdState = tyteState.second;

// 		if (fdType == Client::CLIENT_FD)
// 			Logger::error("We are trying to change client fd. this should not happen in client with id: ", currentClient.getId());
// 		if (fdState == Client::CLOSE)
// 		{
// 			epollRemoveFd(_epollFd, targetFd, events);
// 			close(targetFd);
// 			fdState = Client::CLOSED;
// 			return ;
// 		}
// 		if (fdState == Client::NONE)
// 		{
// 			epollAddFd(_epollFd, targetFd);
// 			return;
// 		}
// 		updateClientFds(currentClient, idx, events);
// 		return ;

		
// 	}
// 	Logger::error("Could not find targetFd in any client (while trying to set cgi socket state): ", targetFd);
// }

void		ConnectionManager::_handleCgiFds(const int& idx, struct epoll_event* events)
{
	int targetFd = events[idx].data.fd;
	// std::map<int, Client*>::iterator it = _clients.find(client.getClientFd());
	std::map<int, Client*>::iterator it = _clients.begin();
	for (; it != _clients.end(); ++it)
	{
		Client& currentClient = *(it->second);
		FdData& fdData = currentClient.getFdDataByFd(targetFd);
		if(fdData.type == FdData::CLIENT_FD)
			continue ;
		if (fdData.state == FdData::CLOSE)
		{
			epollRemoveFd(_epollFd, targetFd, events);
			close(targetFd);
			fdData.state = FdData::CLOSED;
			return ;
		}
		if (fdData.state == FdData::NONE)
		{
			epollAddFd(_epollFd, targetFd);
			return ;
		}
		updateClientFds(currentClient, idx, events);
		return ;

		
	}
	Logger::error("Could not find targetFd in any client (while trying to set cgi socket state): ", targetFd);
}

void	ConnectionManager::epollLoop()
{
	struct epoll_event	events[MAX_EVENTS];
	Client* client = NULL;


	// signal(SIGINT, handle_sigint);
	// _addServerSocketsToEpoll();

		// if (!_catchEpollErrorAndSignal())
			// break;
	int nfds = epoll_wait(_epollFd, events, MAX_EVENTS, MAX_WAIT);
	for (int idx = 0; idx < nfds && nfds != -1; ++idx)
	{
		if (isServerSocket(events[idx].data.fd))
			_acceptNewClient(events[idx].data.fd);
		else if ((client = isClient(events[idx].data.fd, _clients)) != NULL)
			_handleClient(*client, idx, events); // BF:_updateClientFd
		else
			_handleCgiFds(idx, events);
		// handle CGI Sockets
			// add/remove CGI Sockets from/to epoll
			// update CGI Sockets

		// handle Client
			// delete Clients
				// remove Client Fd from Epoll
			// update Client Fds
	}

}

// void	ConnectionManager::ConnectionManagerLoop()
// {

// }

//==========================================================================//
// Constructor, Destructor and OCF Parts ===================================//
//==========================================================================//

// Custom Constructor
ConnectionManager::ConnectionManager(int epollFd) :
_epollFd(epollFd), _clients(Client::clients)
{
	_addServerSocketsToEpoll();
}

// Destructor
ConnectionManager::~ConnectionManager()
{
	Socket::closeSockets();
}