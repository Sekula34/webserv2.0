#include "ConnectionDispatcher.hpp"
#include "SocketManager.hpp"

ConnectionDispatcher::ConnectionDispatcher(SocketManager& sockets, ServersInfo& serversInfo)
:_sockets(sockets), _serversInfo(serversInfo)
{

}
ConnectionDispatcher::ConnectionDispatcher(ConnectionDispatcher& source)
:_sockets(source._sockets), _serversInfo(source._serversInfo)
{

}

ConnectionDispatcher& ConnectionDispatcher::operator=(ConnectionDispatcher& source)
{
	_sockets = source._sockets;
	_serversInfo = source._serversInfo;
	return(*this);
}

ConnectionDispatcher::~ConnectionDispatcher()
{
	
}
