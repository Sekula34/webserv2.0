#include "ConnectionDispatcher.hpp"
#include "Socket.hpp"
#include "SocketManager.hpp"
#include <cstddef>
#include <stdexcept>
#include <sys/select.h>
#include <unistd.h>
#include <vector>
#include <iostream>

ConnectionDispatcher::ConnectionDispatcher(SocketManager& sockets, ServersInfo& serversInfo)
:_sockets(sockets), _serversInfo(serversInfo)
{
	_selectTimeout.tv_sec = 0;
	_selectTimeout.tv_usec = 0;
}
ConnectionDispatcher::ConnectionDispatcher(ConnectionDispatcher& source)
:_sockets(source._sockets), _serversInfo(source._serversInfo), _selectTimeout(source._selectTimeout)
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

}

Socket& ConnectionDispatcher::_findwhichSocketIsReady()
{
	std::vector<int> listenFD = _sockets.getAllListenFd();
	for(size_t i = 0 ; i < listenFD.size(); i++)
	{
		if(FD_ISSET(listenFD[i], &_readSetTemp))
		{
			Socket &toREturn = _sockets.getSocketByFd(listenFD[i]);
			return (toREturn);
		}
	}
	throw std::runtime_error("0 SOCKETS ARE READY");
}

/**
 * @brief FD_SET all filedescriptor of socket for select later
 * 
 */
void ConnectionDispatcher::_setAllServerListenSocketsForRead(void)
{
	std::vector<int> listenFd = _sockets.getAllListenFd();
	for(size_t i = 0; i < listenFd.size(); i++)
	{
		FD_SET(listenFd[i], &_readSetMaster);
	}
}

void ConnectionDispatcher::mainLoop(void)
{
	//only those go in select 
	FD_ZERO(&_errorSetMaster);
	FD_ZERO(&_readSetMaster);
	FD_ZERO(&_writeSetMaster);

	_setAllServerListenSocketsForRead();
	//FD_SET all socketListen fds to those
	while(true)
	{
		_readSetTemp = _readSetMaster;
		_writeSetTemp = _writeSetMaster;
		_errorSetTemp = _errorSetMaster;

		//find maxFD
		//ADD communication socket here as well
		int selectMaxFD = _sockets.getMaxSocketFd();
		int retVal = select(selectMaxFD + 1, &_readSetTemp, &_writeSetTemp, &_errorSetTemp, &_selectTimeout);
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
			Socket ready = _findwhichSocketIsReady();
			//ready.getCommunicationSocket();
			std::cout << "Ready socket is " << std::endl;
			std::cout << ready << std::endl;
			int communicationSocket = ready.getCommunicationSocket();
			char buffer[1024] = {0};
			int valread = read( communicationSocket , buffer, 1024);
			(void) valread;
			std::cout << buffer << std::endl;
			close(communicationSocket);
			// std::vector<int> toREad = _sockets.getAllListenFd();
			// void *buffer[1024];
			// read(toREad[0], buffer, 1024);
			//something is ready
			// read
			// move it from current read to next write
		}
		//select(int nfds, fd_set *__restrict readfds, fd_set *__restrict writefds, fd_set *__restrict exceptfds, struct timeval *__restrict timeout)
		/*
			select
			{
			}

			//if listen fd is ready accept it
				// add fd to next set

			// after read add fd to next write

			// after write handle fd sets depending on keep alive or not, maybe i will skip this 


			//check FDISINSEt
		*/

		//read = next;

	}
}
