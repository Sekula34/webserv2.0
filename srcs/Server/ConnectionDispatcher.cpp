#include "ConnectionDispatcher.hpp"
#include "SocketManager.hpp"
#include <cstddef>
#include <sys/select.h>
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

/**
 * @brief FD_SET all filedescriptor of socket for select later
 * 
 */
void ConnectionDispatcher::_setAllServerListenSocketsForRead(void)
{
	std::vector<int> listenFd = _sockets.getAllListenFd();
	for(size_t i = 0; i < listenFd.size(); i++)
	{
		FD_SET(listenFd[i], &_readSet);
	}
}

void ConnectionDispatcher::mainLoop(void)
{
	FD_ZERO(&_errorSet);
	FD_ZERO(&_readSet);
	FD_ZERO(&_writeSet);
	//only those go in select 

	FD_ZERO(&_nextErrorSet);
	FD_ZERO(&_nextReadSet);
	FD_ZERO(&_nextWriteSet);

	_setAllServerListenSocketsForRead();
	//FD_SET all socketListen fds to those
	while(true)
	{
		_nextReadSet = _readSet;
		_nextWriteSet = _writeSet;
		_nextErrorSet = _errorSet;
		

		//find maxFD
		//ADD communication socket here as well
		int selectMaxFD = _sockets.getMaxSocketFd();
		int retVal = select(selectMaxFD + 1, &_readSet, &_writeSet, &_errorSet, &_selectTimeout);
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
			std::cout << "Something is ready" << std::endl;
			//something is ready
		}
		_readSet = _nextReadSet;
		_errorSet = _nextErrorSet;
		_writeSet = _nextWriteSet;
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
