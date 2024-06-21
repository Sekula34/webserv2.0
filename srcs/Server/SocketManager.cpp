#include "SocketManager.hpp"
#include "Socket.hpp"
#include <cstddef>
#include <unistd.h>
#include <vector>


SocketManager::SocketManager()
{

}

SocketManager::SocketManager(std::vector<int>& uniquePorts)
{
	for(size_t i = 0; i < uniquePorts.size(); i++)
	{
		Socket oneSocket(uniquePorts[i]);
		_allSockets.push_back(oneSocket);
	}
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
		close(_allSockets[i].getSocketFd());
	}
}

std::vector<Socket>& SocketManager::getAllSockets(void)
{
	return (_allSockets);
}
