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
		fd_set _readSetTemp, _writeSetTemp, _errorSetTemp;
		fd_set _readSetMaster, _writeSetMaster, _errorSetMaster;

		/**
		 * @brief use FD_ISSET to find which socket is ready ro read
		 * listen FD socket
		 * 
		 * @return Socket& 
		 * @throw  0 Sockets are ready if cannot find any ready to read 
		 */
		Socket& _findwhichSocketIsReady();
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
