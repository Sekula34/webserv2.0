#include "ServerSettings.hpp"
#include "DefaultSettings.hpp"
#include "Directive.hpp"
#include "LocationSettings.hpp"
#include <cstddef>
#include <iostream>
#include <vector>

ServerSettings::ServerSettings()
{
	_serverId = -1;
}

ServerSettings::ServerSettings(int serverId, DefaultSettings& setings, std::vector<Token>& allTokens)
:DefaultSettings(setings), _serverId(serverId)
{
	_serverTokens = Token::getAllServerTokens(serverId, allTokens);
	_serverDirectives = Directive::getAllServerDirectives(_serverTokens);
	//applyallServerDirectives
	_applyAllServerLevelDirectives();
	//printAllSettings();
	//getAlllocations
	_serverLocations = _setServerLocations();
	// if(serverId == 1)
	// {
	// 	LocationSettings::printAllLocationSettings(_serverLocations);
	// }
	//_serverLocations[0].printLocationSettings();
	//std::cout <<"Size of server locations is " << _serverLocations.size() << std::endl;
	//apllyAlllocation Settings
	//Directive::printAllDirectives(_serverDirectives);
}
ServerSettings::ServerSettings(const ServerSettings& source)
: DefaultSettings(source), _serverId(source._serverId)
{
	(*this) = source;
}

ServerSettings& ServerSettings::operator=(const ServerSettings& source)
{
	_serverId = source._serverId;
	_serverDirectives = source._serverDirectives;
	_serverTokens = source._serverTokens;
	_serverLocations = source._serverLocations;
	return(*this);
}

ServerSettings::~ServerSettings()
{

}

//set and apply all location directives and the same time
std::vector<LocationSettings> ServerSettings::_setServerLocations()
{
	std::vector<LocationSettings> serverLocations;
	for(size_t i = 0; i < _serverTokens.size(); i++)
	{
		if(_serverTokens[i].getCurrentTokenContextType() == LOCATION)
		{
			//LocationSettings location(server, serverTokens[i]);
			LocationSettings location(*this, _serverTokens[i], _serverTokens);
			serverLocations.push_back(location);
		}
	}
	return serverLocations;
}
//apply every directive server have //but only on server level check path. Not location Level
void ServerSettings::_applyAllServerLevelDirectives()
{
	for(size_t i = 0; i < _serverDirectives.size(); i++)
	{
		//if path is 2 that means directive have http server
		if(_serverDirectives[i].getDirectivePathSize() == 2)
			_serverDirectives[i].apply(*this);
	}
}
void ServerSettings::addDirectiveToServer(Directive directive)
{
	_serverDirectives.push_back(directive);
}

std::vector<Directive> ServerSettings::getServerDirectives(void) const
{
	return(_serverDirectives);
}

std::vector<Token> ServerSettings::getServerTokens(void) const 
{
	return (_serverTokens);
}

std::vector<LocationSettings> ServerSettings::getServerLocations() const 
{
	return (_serverLocations);
}

const int& ServerSettings::getServerId() const 
{
	return(_serverId);
}

void ServerSettings::printServerTokens(void) const 
{
	std::cout << "Server id: " << _serverId << " tokens are: "<< std::endl;
	Token::printAllTokensInfo(_serverTokens);
}
void ServerSettings::printServerSettings(void) const
{
	std::cout << "---------------DEFAULT SERVER SETTINGS PRINT ---------------" <<  std::endl;
	std::cout << "Server id: " << _serverId << std::endl;
	std::cout << "Default server name :" << _serverName << std::endl;
	std::cout << "Default listen port:" << _listenPort << std::endl;
	std::cout << "Defautl host: " << _host << std::endl;
	//std::cout << "Server directives: "<< std::endl;
	// for(size_t i = 0; i < _serverDirectives.size(); i++)
	// {
	// 	_serverDirectives[i].printDirectiveInfor();
	// }
	std::cout <<"Server locations are :" << std::endl;
	std::cout << "\t" << std::endl;
	for(size_t i = 0; i < _serverLocations.size(); i++)
	{
		_serverLocations[i].printLocationSettings();
	}
	std::cout << "___________________________________________________"<<std::endl;
}
