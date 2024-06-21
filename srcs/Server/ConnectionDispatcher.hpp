#ifndef CONNECTIONDISPATCHER_HPP
# define CONNECTIONDISPATCHER_HPP
#include "SocketManager.hpp"
#include "../Parsing/ServersInfo.hpp"

/**
 * @brief class for handling select creating servers and give them tasks
 * 
 */
class ConnectionDispatcher 
{
	private :
		SocketManager &_sockets;
		ServersInfo &_serversInfo;

		//ConnectionDispatcher();
	public :
		ConnectionDispatcher(SocketManager& sockets, ServersInfo& serverInfo);
		ConnectionDispatcher(ConnectionDispatcher& source);
		ConnectionDispatcher& operator=(ConnectionDispatcher& source);
		~ConnectionDispatcher();

};

#endif
