#include "DummyVirtualServer.hpp"
#include "DummyClient.hpp"
#include "DummyServerSettings.hpp"
#include "DummyMessage.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>

//==========================================================================//
// REGULAR METHODS==========================================================//
//==========================================================================//

void	VirtualServer::_execDelete(DummyMessage* request)
{
	// Execute DELETE method.
	// (void)request;
	std::cout << "DELETE method executed" << std::endl;
	std::map<std::string, std::string>& msgHeader = request->getHeader();
	std::string filename = msgHeader["uri"];
	if (remove(filename.c_str()) != 0)
		std::cerr << "Unable to DELETE file!" << std::endl;
}

void	VirtualServer::_execPost(DummyMessage* request)
{
	// Execute POST method.
	std::cout << "POST method executed" << std::endl;
	std::map<std::string, std::string>& msgHeader = request->getHeader();
	std::string filename = msgHeader["uri"];
	std::ofstream outputFile(filename.c_str());
	if (!outputFile.is_open())
	{
		std::cerr << "Unable to create POST file!" << std::endl;
		return ;
	}
	outputFile << *request;
	outputFile.close();
}

void	VirtualServer::_execGet(DummyMessage* request)
{
	// Execute DELETE method.
	(void)request;
	std::cout << "GET method executed" << std::endl;
}

bool	VirtualServer::_isvalidRequest(DummyMessage* request)
{
	/* PSEUDOCODE:
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
