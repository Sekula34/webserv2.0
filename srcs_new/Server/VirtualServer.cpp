#include "VirtualServer.hpp"

#include <iostream>
//==========================================================================//
// REGULAR METHODS==========================================================//
//==========================================================================//

void	VirtualServer::_execDelete(DummyMessage& request)
{
	// Execute GET method as filip was doing.
	std::cout << "GET method executed" << std::endl;
}

void	VirtualServer::_execPost(DummyMessage& request)
{
	// Execute POST method.
	std::cout << "GET method executed" << std::endl;
}

void	VirtualServer::_execGet(DummyMessage& request)
{
	// Execute DELETE method.
	std::cout << "GET method executed" << std::endl;
}

bool	VirtualServer::_isvalidRequest(DummyMessage& request)
{
	/* 
	if (client.message.method == _serverSettings.method
	&& client.message.protocol == _serverSettings.protocol --> This should be checked while generating the header
	&& client.message.host == _serverSettings.host
	&& client.message.port == _serverSettings.port
	&& client.message.uri == _serverSettings.uri
	&& client.message.cgi == _serverSettings.cgi)
		return (true);
	else
		return (false);
	 */
}

void	VirtualServer::generateResponse(DummyClient& client)
{
	/* 
	if (client.errorCode != 0)
	{
		std::cout << "Internal server error" << std::endl;
		return ;
	}
	if (_isvalidRequest == false)
	{
		client.errorCode = 400;
		std::cout << "Invalid request" << std::endl;
		return ;
	}
	if (client.client.message.method == GET)
		_execGet();
	else if (client.client.message.method == POST)
		_execPost();
	else if (client.client.message.method == DELETE)
		_execDelete();
	else
	{
		client.errorCode == 405;
		std::cout << "Not implemented" << std::endl;
		return ;
	}
	 */
}

//==========================================================================//
// Constructor, Destructor and OCF Parts ===================================//
//==========================================================================//

// Custom Constructor
VirtualServer::VirtualServer(const DummyServerSettings& settings)
: _serverSettings(settings)
{}

// Destructor
VirtualServer::~VirtualServer() {}

// Copy Constructor
VirtualServer::VirtualServer(const VirtualServer& src)
: _serverSettings(src._serverSettings)
{}
