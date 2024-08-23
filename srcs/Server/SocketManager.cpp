#include "SocketManager.hpp"
#include "../Utils/Data.hpp"
#include "Socket.hpp"
#include <cstddef>
#include <stdexcept>
#include <unistd.h>
#include <vector>
#include <iostream>


SocketManager::SocketManager()
{

}

SocketManager::SocketManager(const std::vector<int>& uniquePorts)
{
	for(size_t i = 0; i < uniquePorts.size(); i++)
	{
		Socket oneSocket(uniquePorts[i]);
		_allSockets.push_back(oneSocket);
	}
	// maybe Data will replace socket manager...
	Data::setServerSockets(&_allSockets);
}

SocketManager::SocketManager(const SocketManager& source)
{
	_allSockets = source._allSockets;
}

SocketManager& SocketManager::operator=(const SocketManager& source)
{
	_allSockets = source._allSockets;
	return(*this);
}

SocketManager::~SocketManager()
{
	for(size_t i = 0; i < _allSockets.size(); i++)
	{
		std::cout << "trying to close sockets" << std::endl;
		close(_allSockets[i].getSocketFd());
	}
}

std::vector<Socket>& SocketManager::getAllSockets(void)
{
	return (_allSockets);
}

std::vector<int> SocketManager::getAllListenFd(void) const
{
	std::vector<int> listenFds;
	for(size_t i = 0; i < _allSockets.size(); i++)
	{
		int oneFd = _allSockets[i].getSocketFd();
		listenFds.push_back(oneFd);
	}
	return listenFds;
}

Socket& SocketManager::getSocketByFd(int socketFD)
{
	for(size_t i = 0; i < _allSockets.size(); i++)
	{
		if(_allSockets[i].getSocketFd() == socketFD)
		{
			return _allSockets[i];
		}
	}
	std::cerr << "Cannot find socket with " << socketFD << std::endl;
	throw std::runtime_error("NO SUCH SOCKET");
}

int SocketManager::getMaxSocketFd() const
{
	int maxFD(-1);

	for(size_t i = 0; i <_allSockets.size(); i++)
	{
		int currentFd = _allSockets[i].getSocketFd();
		if(currentFd > maxFD)
			maxFD = currentFd;
	}
	return maxFD;
}
