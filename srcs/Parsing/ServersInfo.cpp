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

void ServersInfo::printAllServersInfo(void) const 
{
	for(size_t i = 0; i < _servers.size(); i++)
	{
		std::cout << "-------------------INFO FOR SERVER " << i << "----------------------------" << std::endl;
		_servers[i].printServerSettings();
		std::cout << std::endl;
	}
}

const std::vector<ServerSettings>& ServersInfo::getAllServers(void) const
{
	return(_servers);
}

const ServerSettings& ServersInfo::getOneServer(int serverIndex) const 
{
	if(serverIndex > _numberOfServers)
	{
		std::runtime_error("Will segfault. There is not enough servers");
	}
	return(_servers[serverIndex]);
}
