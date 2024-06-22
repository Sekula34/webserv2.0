#ifndef CONNECTIONDISPATCHER_HPP
# define CONNECTIONDISPATCHER_HPP
#include "SocketManager.hpp"
#include "../Parsing/ServersInfo.hpp"
#include <sys/select.h>

/**
 * @brief class for handling select creating servers and give them tasks
 * 
 */
class ConnectionDispatcher 
{
	private :
		SocketManager &_sockets;
		ServersInfo &_serversInfo;
		struct timeval _selectTimeout;
		fd_set _readSet, _writeSet, _errorSet;
		fd_set _nextReadSet, _nextWriteSet, _nextErrorSet;

		void _setAllServerListenSocketsForRead(void);

		//ConnectionDispatcher();
	public :
		ConnectionDispatcher(SocketManager& sockets, ServersInfo& serverInfo);
		ConnectionDispatcher(ConnectionDispatcher& source);
		ConnectionDispatcher& operator=(ConnectionDispatcher& source);
		~ConnectionDispatcher();

		void mainLoop(void);

};

#endif
