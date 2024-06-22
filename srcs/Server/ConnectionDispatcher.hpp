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
		
		/**
		 * @brief temp goes in select and will be destroyed every time
		 * 
		 */
		fd_set _readSetTemp, _writeSetTemp, _errorSetTemp;
		/**
		 * @brief master is permanent 
		 * 
		 */
		fd_set _readSetMaster, _writeSetMaster, _errorSetMaster;

		/**
		 * @brief fill vector with all socket that have something to read and are ready
		 * 
		 * @return std::vector<Socket> which can be empty if nothing is ready 
		 */
		std::vector<Socket> _getAllReadyToReadSockets();
		void _setAllServerListenSocketsForRead(void);

		/**
		 * @brief function that handles Ready File Descriptor when select return > 0
		 * 
		 */
		void _handleReadyFd(void);
		void _handleAllReadySockets(std::vector<Socket>& readySockets);

		//ConnectionDispatcher();
	public :
		ConnectionDispatcher(SocketManager& sockets, ServersInfo& serverInfo);
		ConnectionDispatcher(ConnectionDispatcher& source);
		ConnectionDispatcher& operator=(ConnectionDispatcher& source);
		~ConnectionDispatcher();

		void mainLoop(void);

};

#endif
