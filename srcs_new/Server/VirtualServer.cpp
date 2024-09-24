#include "VirtualServer.hpp"
#include "Delete/DummyClient.hpp"
#include "Delete/DummyServerSettings.hpp"
#include "Delete/DummyMessage.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>
//==========================================================================//
// REGULAR METHODS==========================================================//
//==========================================================================//

void	VirtualServer::_execDelete(DummyMessage* request)
{
	// Execute GET method as filip was doing.
	(void)request;
	std::cout << "DELETE method executed" << std::endl;
}

void	VirtualServer::_execPost(DummyMessage* request)
{
	// Execute POST method.
	(void)request;
	std::cout << "POST method executed" << std::endl;
}

void	VirtualServer::_execGet(DummyMessage* request)
{
	// Execute DELETE method.
	(void)request;
	std::cout << "GET method executed" << std::endl;
}

bool	VirtualServer::_isvalidRequest(DummyMessage* request)
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
	std::map<std::string, std::string>& msgHeader = request->getHeader();
	std::vector<std::string> fieldsToCheck = {"method", "host", "port"};

	std::vector<std::string>::const_iterator fieldIt = fieldsToCheck.begin();
	for (; fieldIt != fieldsToCheck.end(); ++fieldIt)
	{
		std::map<std::string, std::string>::const_iterator headerIt = msgHeader.find(*fieldIt);
		if (headerIt != msgHeader.end())
		{
			std::cout << "HeaderField:" << headerIt->first << " -- HeaderValue:" << headerIt->second << std::endl;
			std::cout << "ConfigFile:" << headerIt->first << "-- ConfigValue:" << _serverSettings.getValue(headerIt->first) << std::endl;
			// Should I handle when getValue returns an empty string?
			if (_serverSettings.getValue(headerIt->first).find(headerIt->second) == std::string::npos)
				return (false);
		}
	}
	return (true);
	// return (true);
	//  std::map<std::string, std::string>::const_iterator it = msgHeader.find("method");
	// if (it != msgHeader.end() && _serverSettings.getValue(it->first).find(it->second) != std::string::npos
	// && )
	
	
	
	// if (it != msgHeader.end())
	// 	std::string headerField = it->first;


	//  	_serverSettings.getValue(it->first).find_first_of(it->second);

	//  return (false);
}

void	VirtualServer::generateResponse(DummyClient& client)
{
	if (client.getErrorCode() != 0)
	{
		std::cout << "Internal server error" << std::endl;
		return ;
	}
	if (_isvalidRequest(client.getMsg(DummyClient::REQ_MSG)) == false)
	{
		client.setErrorCode(400);
		std::cout << "Invalid request" << std::endl;
		return ;
	}
	// std::map<std::string, std::string>::const_iterator it = client.getMsg(DummyClient::REQ_MSG)->getHeader().begin();
	// std::map<std::string, std::string>::const_iterator it = client.getMsg(DummyClient::REQ_MSG)->getHeader().find("method");
	std::map<std::string, std::string>& clientMsgHeader = client.getMsg(DummyClient::REQ_MSG)->getHeader();
	std::map<std::string, std::string>::const_iterator it = clientMsgHeader.find("method");
	if (it != clientMsgHeader.end() && it->second == "GET")
		_execGet(client.getMsg(DummyClient::REQ_MSG));
	else if (it != clientMsgHeader.end() && it->second == "POST")
		_execPost(client.getMsg(DummyClient::REQ_MSG));
	else if (it != clientMsgHeader.end() && it->second == "DELETE")
		_execDelete(client.getMsg(DummyClient::REQ_MSG));
	else
	{
		client.setErrorCode(405);
		std::cout << "Not implemented" << std::endl;
		return ;
	}
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

// Overloaded Insertion Operator (For testing).
std::ostream&	operator<<(std::ostream& out, const VirtualServer& virtualServer)
{
	out << "VIRTUAL SERVER" << std::endl;
	out << virtualServer._serverSettings;
	return (out);
}

// std::cout << "-----------------DEBUGGING-----------------" << std::endl;