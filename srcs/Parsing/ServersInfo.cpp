#include "ServersInfo.hpp"
#include "Configuration.hpp"
#include "DefaultSettings.hpp"
#include "Directive.hpp"
#include "ServerSettings.hpp"
#include "Token.hpp"
#include <cstddef>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <algorithm>
#include "../Utils/Logger.hpp"
#include "../Utils/HttpStatusCode.hpp"
#include "../Client/Message.hpp"


ServersInfo::ServersInfo(std::string configPath)
{
	Configuration serversConf(configPath);
	_allTokens = serversConf.getAllTokens();
//	Token::printAllTokensInfo(_allTokens);
	_numberOfServers = serversConf.getNumberOfServers();
	if(_numberOfServers < 1)
	{
		std::cerr << yellow << "There is not even one server in config file" << resetText << std::endl;
		throw Configuration::InvalidConfigFileException();
	}

	DefaultSettings defSettings;
	_setHttpDirectives();
	Directive::applyAllDirectives(_httpDirectives, defSettings);
	//defSettings.printAllSettings();
	//Directive::printAllDirectives(_httpDirectives);
	for(int i = 1; i <= _numberOfServers; i++)
	{
		ServerSettings oneServer(i,defSettings,_allTokens);
		//oneServer.printServerSettings();
		_servers.push_back(oneServer);
	}
}

ServersInfo::ServersInfo(const ServersInfo& source)
{
	(void) source;
}

ServersInfo& ServersInfo::operator=(const ServersInfo& source)
{
	(void) source;
	return(*this);
}

ServersInfo::~ServersInfo()
{

}

//check if Token is directive that belongs to http only
bool ServersInfo::_isTokenHttpDirective(const Token& toCheck) const
{
	if(toCheck.getTokenType() != DIRECTIVE)
		return false;
	std::vector<Token> path = toCheck.getTokenPath();
	if(path.size() != 1)
		return false;
	ContextType parentType = path[0].getTokenContextType();
	if(parentType != HTTP )
		return false;
	return true;
}

std::vector<ServerSettings> ServersInfo::_getAllServersIdWithPort(int port) const
{
	std::vector<ServerSettings> serversPort;
	for(size_t i = 0; i < _servers.size(); i++)
	{
		const ServerSettings& oneServer(_servers[i]);
		if(oneServer.getPort() == port)
			serversPort.push_back(oneServer);
	}
	return serversPort;
}

void ServersInfo::_setHttpDirectives(void)
{
	for(size_t i = 0; i < _allTokens.size(); i++)
	{
		if(_isTokenHttpDirective(_allTokens[i]) == true)
		{
			Directive httpDirective(_allTokens[i]);
			_httpDirectives.push_back(httpDirective);
		}
	}
}

//MR_NOTE: This function is not being used.
void ServersInfo::printAllServersInfo(void) const 
{
	for(size_t i = 0; i < _servers.size(); i++)
	{
		std::cout << "-------------------INFO FOR SERVER " << i << "----------------------------" << std::endl;
		_servers[i].printServerSettings();
		std::cout << std::endl;
	}
}

// MR_NOTE: This function is not being used.
const std::vector<ServerSettings>& ServersInfo::getAllServers(void) const
{
	return(_servers);
}

const ServerSettings& ServersInfo::getServerById(int serverId) const 
{
	int serverIndex = serverId - 1;
	if(serverIndex > _numberOfServers || serverIndex < 0)
	{
		throw std::runtime_error("Will segfault. There is not enough servers or index is less than 0");
	}
	return(_servers[serverIndex]);
}



const ServerSettings* ServersInfo::getServerByPort(int portNumber, std::string serverName) const
{
	int serverId;
	const ServerSettings* toReturn = NULL;
	std::vector<ServerSettings> ServersId = _getAllServersIdWithPort(portNumber);
	if(ServersId.size() == 0)
		return NULL;
	if(ServersId.size() == 1 || serverName == "") // MR_DOUBT: Why if its empty?
	{
		serverId = ServersId[0].getServerId();
		toReturn = &getServerById(serverId);
		return toReturn;
	}
	for(size_t i = 0; i< ServersId.size(); i++)
	{
		ServerSettings& oneServer(ServersId[i]);
		if(oneServer.getServerName() == serverName)
		{
			serverId = oneServer.getServerId();
			return &getServerById(serverId);
		}
	}
	serverId = ServersId[0].getServerId(); // MR_DOUBT: Why if it doesnt found name, we return the 1st?
	return &getServerById(serverId);
}

// MR_DOUBT: What is the meaning of "header" here? And why this function is on ServersInfo?
bool ServersInfo::_validateRequestHeader(const RequestHeader* header) const
{
	if(header == NULL)
	{
		Logger::warning("Trying to get Client Server with client that have no header");
		return false;
	}
	if(header->getHttpStatusCode() == 400) // or check if code is 400 
	{
		const std::string reason = HttpStatusCode::getReasonPhrase(header->getHttpStatusCode());
		Logger::warning("Client send " + reason, true);
		Logger::warning("There is no filled info in header for finding server", true);
		return false;
	}
	return true;
}

// MR_DOUBT: What is the meaning of "header" here? And why this function is on ServersInfo?
const ServerSettings* ServersInfo::getClientServer(const Client& client) const
{
	if(_validateRequestHeader(static_cast<RequestHeader*>(client.getClientMsg()->getHeader())) == false)
		return NULL;
	int portNumber = static_cast<RequestHeader*>(client.getClientMsg()->getHeader())->getHostPort();
	const ServerSettings* toReturn = getServerByPort(portNumber, static_cast<RequestHeader*>(client.getClientMsg()->getHeader())->getHostName());
	return toReturn;
}

//goes through vector of servers
//get server Port, check if port is in unique vector if not add it
//return unique ports;
const std::vector<int> ServersInfo::getUniquePorts() const 
{
	std::vector<int> uniquePorts;
	for(size_t i = 0; i < _servers.size(); i++)
	{
		const int serverPort = _servers[i].getPort();
		std::vector<int>::iterator it;
		it = std::find(uniquePorts.begin(), uniquePorts.end(), serverPort);
		if(it == uniquePorts.end())
		{
			uniquePorts.push_back(serverPort);
		}
	}
	return uniquePorts;
}
