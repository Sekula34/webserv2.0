#include "ConnectionManager.hpp"
#include "../Utils/Logger.hpp"
#include "Socket.hpp"
#include "../Client/Client.hpp"
#include <exception>
#include <map>
#include <vector>
#include <csignal>

#define MAX_WAIT		-1 // 0: epoll runs in nonblocking way but CPU runs at 6,7 % 
extern volatile sig_atomic_t flag;

int ConnectionManager::_epollFd = epoll_create(1);

//==========================================================================//
// REGULAR METHODS==========================================================//
//==========================================================================//

// Helper function to add fd to epoll
static int		epollAddFd(Client* client, const int& epollFd, const int& fd)
{
	int ret;
	// STRUCT NEEDED FOR EPOLL TO SAVE FLAGS INTO (SETTINGS)
	struct epoll_event	ev;

	// SETTING UP EV EVENTS 'SETTINGS' STRUCT FOR LISTEN_SOCKET
	ev.events = EPOLLIN | EPOLLOUT;
	ev.data.fd = fd;

	Logger::warning("adding fd to epoll: ", fd);
	if (client)
		Logger::warning("in Client with ID: ", client->getId());
	// ADDING LISTEN_SOCKET TO EPOLL WITH THE EV 'SETTINGS' STRUCT
	ret = epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev);
	return (ret);
}

const int& ConnectionManager::getEpollFd()
{
	return (_epollFd);
}

// Helper function to remove fd to epoll
static void	epollRemoveFd(Client& client, const int& epollFd, const int& fd, struct epoll_event* events)
{
	Logger::warning("removing fd from epoll: ", fd);
	Logger::warning("In Client with ID: ", client.getId());
	// REMOVE THE FD OF THIS CLIENT INSTANCE FROM EPOLLS WATCH LIST
	if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, events) == -1)
	{
		Logger::error("The following FD could not be removed from epoll: ", fd);
		Logger::error("In Client with ID: ", client.getId());
	}
		// throw std::runtime_error("epoll_ctl error: removing file descriptor from epoll failed");
}

// Method for _epollLoop() to accept new client and add it to epoll
void	ConnectionManager::_acceptNewClient(int listen_socket)
{
	struct sockaddr client_addr;
	socklen_t addrlen = sizeof(client_addr);
	int	clientFd;

	if (flag != 0)
		return ;

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
		close(clientFd);
		Logger::error("Failed newClient: ", e.what());
		return ;
	}

	// ADD CLIENT FD TO THE LIST OF FDS THAT EPOLL IS WATCHING FOR ACTIVITY
	int ret = epollAddFd(newClient, _epollFd, clientFd);
	if (ret == -1)
		delete newClient;
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
		Logger::info("Adding to epoll the following server socket: ", fd);
		ret = epollAddFd(NULL, _epollFd, fd);
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

static void	updateClientFds(Client& client, const int& epollIdx, const struct epoll_event* events)
{
	const int fd = events[epollIdx].data.fd;
	FdData& fdData = client.getFdDataByFd(fd); 
	if (fdData.state == FdData::CLOSED || fdData.state == FdData::CLOSE)
		return ;
	bool allowedToSend = events[epollIdx].events & EPOLLOUT;
	bool allowedToReceive = events[epollIdx].events & EPOLLIN;
	if (allowedToSend && !allowedToReceive)
		fdData.state = FdData::R_SEND;
	if (!allowedToSend && allowedToReceive)
		fdData.state = FdData::R_RECEIVE;
	if (allowedToSend && allowedToReceive)
		fdData.state = FdData::R_SENDREC;
	if (!allowedToSend && !allowedToReceive)
		fdData.state = FdData::NONE;
}

static bool	safeToDelete(Client& client)
{
	// CLIENT STATE SAIS DELETE ME
	if((client.getClientState() == Client::DELETEME && client.getCgiFlag() == false)
	|| (client.getClientState() == Client::DELETEME && client.getWaitReturn() != 0))	
		return (true);

	// NO CGI PROCESS RUNNING AND CLIENT HAS TIMED OUT
	if (client.getCgiFlag() == false
		&& client.checkTimeout() == false
		&& client.getWaitReturn() == 0)
		return (true);

	if (client.checkTimeout(MAX_TIMEOUT * 2) == false)
		return (true);
	
	return (false);
}

void	ConnectionManager::_removeFromEpollUnclosedFds(Client& client)
{
	std::vector<FdData>& fds = client.getClientFds();
	std::vector<FdData>::iterator it = fds.begin();

	for (; it != fds.end(); it++)
	{
		if (it->state != FdData::CLOSED)
		{
			it->state = FdData::CLOSED;
			epollRemoveFd(client, _epollFd, it->fd, _events);
			close(it->fd);
		}
	}
}

// Method for _epollLoop() to update Client Fd state (e_fdState)
void	ConnectionManager::_handleClient(Client& client, const int& idx)
{

	// if ((client.checkTimeout() == false || flag > 0)
	if ((client.checkTimeout() == false || flag > 0 || (client.getErrorCode() != 0 ))
		&& client.getWaitReturn() == 0
		&& client.getSignalSent() == 0)
	{
		client.setSignalSent(1);
		if (client.getCgiFlag() == true && client.getChildPid() != 0)
		{
			Logger::warning("sending sig TERM to child", "");
			kill(client.getChildPid(), SIGTERM);
		}
		if ((client.getWaitReturn() != 0 || client.getCgiFlag() == false)
			&& client.getClientState() != Client::DELETEME)
			client.setClientState(Client::DO_RESPONSE);
	}

	if (client.checkTimeout(MAX_TIMEOUT * 1.5) == false
		&& client.getWaitReturn() == 0
		&& client.getSignalSent() == 1)
	{
		client.setSignalSent(2);
		Logger::warning("sending sig KILL to child", "");
		if (client.getChildPid() != 0)
			kill(client.getChildPid(), SIGKILL);
		client.setClientState(Client::DO_RESPONSE);
	}

	if (safeToDelete(client))
	{
		// epollRemoveFd(_epollFd, client.getFdDataByType(FdData::CLIENT_FD).fd, _events);
		_removeFromEpollUnclosedFds(client);

		delete &client; // delete needs an address
		return ;
	}
	updateClientFds(client, idx, _events);
}

void		ConnectionManager::_addChildSocketsToEpoll()
{
	std::map<int, Client*>::iterator it = _clients.begin();
	for (; it != _clients.end(); ++it)
	{
		Client& currentClient = *(it->second);
		std::vector<FdData>& fds = currentClient.getClientFds();
		if (fds.size() == 1)
			continue ;
		std::vector<FdData>::iterator itFd = fds.begin();
		for (; itFd != fds.end(); ++itFd)
		{
			if ((itFd->type == FdData::TOCHILD_FD || itFd->type == FdData::FROMCHILD_FD)
	   			&& itFd->state == FdData::NEW)
			{
				epollAddFd(&currentClient, _epollFd, itFd->fd);
				itFd->state = FdData::NONE;
			}
		}
	}
}


void		ConnectionManager::_handleCgiFds(const int& idx)
{
	int targetFd = _events[idx].data.fd;
	std::map<int, Client*>::iterator it = _clients.begin();
	for (; it != _clients.end(); ++it)
	{
		Client& currentClient = *(it->second);
		FdData& fdData = currentClient.getFdDataByFd(targetFd);
		if(fdData.type == FdData::CLIENT_FD)
			continue ;
		if (fdData.state == FdData::CLOSE)
		{
			epollRemoveFd(currentClient, _epollFd, targetFd, _events);
			close(targetFd);
			fdData.state = FdData::CLOSED;
			return ;
		}
		// if (fdData.state == FdData::NEW)
		// {
		// 	epollAddFd(_epollFd, targetFd);
		// 	fdData.state = FdData::NONE;
		// 	return ;
		// }
		updateClientFds(currentClient, idx, _events);
		return ;
	}
	Logger::error("Could not find targetFd in any client (while trying to set cgi socket state): ", targetFd);
}

void ConnectionManager::closeClientFds()
{
	std::map<int, Client*>::iterator it = Client::clients.begin();
	for(; it != Client::clients.end(); it++)
	{
		close(it->second->getFdDataByType(FdData::CLIENT_FD).fd);
		// if (it->second->socketToChild != DELETED)
		if (it->second->getClientFds().size() == 1)
			continue ;
		if (it->second->getFdDataByType(FdData::TOCHILD_FD).state != FdData::CLOSED)
			close(it->second->getFdDataByType(FdData::TOCHILD_FD).fd);
		// if (it->second->socketFromChild != DELETED)
		if (it->second->getFdDataByType(FdData::FROMCHILD_FD).state != FdData::CLOSED)
			close(it->second->getFdDataByType(FdData::FROMCHILD_FD).fd);
	}
}


void	ConnectionManager::epollLoop()
{
	Client* client = NULL;


	_addChildSocketsToEpoll();
	int nfds = epoll_wait(_epollFd, _events, MAX_EVENTS, MAX_WAIT);
	if (nfds == -1 && !flag)
	{
		Logger::error("epoll_wait failed", "");
		return ;
	}
	for (int idx = 0; idx < nfds && nfds != -1; ++idx)
	{
		if (isServerSocket(_events[idx].data.fd))
			_acceptNewClient(_events[idx].data.fd);
		else if ((client = isClient(_events[idx].data.fd, _clients)) != NULL)
			_handleClient(*client, idx); // BF:_updateClientFd
		else
			_handleCgiFds(idx);
	}
}


//==========================================================================//
// Constructor, Destructor and OCF Parts ===================================//
//==========================================================================//

// Custom Constructor
ConnectionManager::ConnectionManager() :
 _clients(Client::clients)
{
	if (_epollFd < 0)
		throw std::runtime_error("epoll create failed");
	_addServerSocketsToEpoll();
}

// Destructor
ConnectionManager::~ConnectionManager()
{
	// close (_epollFd);
	// Socket::closeSockets();
}
