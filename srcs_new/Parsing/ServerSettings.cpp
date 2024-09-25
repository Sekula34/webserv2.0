#include "ServerSettings.hpp"
#include "DefaultSettings.hpp"
#include "Directive.hpp"
#include "LocationSettings.hpp"
#include "ParsingUtils.hpp"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <vector>
#include "../Utils/Logger.hpp"

ServerSettings::ServerSettings()
{
	_serverId = -1;
}

ServerSettings::ServerSettings(int serverId, DefaultSettings& setings, std::vector<Token>& allTokens)
:DefaultSettings(setings), _serverId(serverId)
{
	_serverTokens = Token::getAllServerTokens(serverId, allTokens);
	_serverDirectives = Directive::getAllServerDirectives(_serverTokens);
	checkDuplicateDirectives(_getServerLevelDirectives());
	_applyAllServerLevelDirectives();
	_serverLocations = _setServerLocations();
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

bool ServerSettings::amIServerLocation(const std::string& path) const
{
	bool found = true;
	fetchLocationWithUri(path, found);
	return found;
}

std::string ServerSettings::getLocationURIfromPath(const std::string& fullPath) const
{
	std::string toTry(fullPath);
	while (true)
	{
		if(amIServerLocation(toTry) == true)
			return toTry;
		toTry = ParsingUtils::getDirName(toTry);
	}
}

void ServerSettings::addDirectiveToServer(Directive directive)
{
	_serverDirectives.push_back(directive);
}

const std::vector<Directive>& ServerSettings::getServerDirectives(void) const
{
	return(_serverDirectives);
}

const std::vector<Token>& ServerSettings::getServerTokens(void) const 
{
	return (_serverTokens);
}

const std::vector<LocationSettings>& ServerSettings::getServerLocations() const 
{
	return (_serverLocations);
}

const int& ServerSettings::getServerId() const 
{
	return(_serverId);
}

std::vector<LocationSettings>::const_iterator ServerSettings::fetchLocationWithUri(const std::string uri, bool& found) const
{
	found = true;
	LocationSettings::FindByUri functor(uri);
	std::vector<LocationSettings>::const_iterator it = std::find_if(_serverLocations.begin(), _serverLocations.end(), functor);
	if(it == _serverLocations.end())
	{
		found = false;
	}
	return (it);
}

const std::vector<Directive> ServerSettings::_getServerLevelDirectives() const
{
	std::vector<Directive> serverLevelDirectives;
	for(size_t i = 0; i < _serverDirectives.size(); i++)
	{
		//if path is 2 that means directive have http server
		if(_serverDirectives[i].getDirectivePathSize() == 2)
			serverLevelDirectives.push_back(_serverDirectives[i]);
	}
	return serverLevelDirectives;
}

//set and apply all location directives and the same time
std::vector<LocationSettings> ServerSettings::_setServerLocations()
{
	std::vector<LocationSettings> serverLocations;
	for(size_t i = 0; i < _serverTokens.size(); i++)
	{
		if(_serverTokens[i].getCurrentTokenContextType() == Token::LOCATION)
		{
			LocationSettings location(*this, _serverTokens[i], _serverTokens, *this);
			serverLocations.push_back(location);
			std::cout << location << std::endl;
		}
	}
	if(_hasDefaultLocation(serverLocations) == false)
		_generateDefaultLocation(serverLocations);
	return serverLocations;
}

bool ServerSettings::_hasDefaultLocation(const std::vector<LocationSettings>& serverLocation) const
{
	std::vector<LocationSettings>::const_iterator it = serverLocation.begin();
	for(;it != serverLocation.end(); it++)
	{
		if(it->getLocationUri() == "/")
			return true;
	}
	return false;
}

void ServerSettings::_generateDefaultLocation(std::vector<LocationSettings>& serverLocation)
{
	LocationSettings location(*this, _serverTokens, *this);
	serverLocation.push_back(location);
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
std::ostream& operator<<(std::ostream& os, const ServerSettings& server)
{
	std::vector<std::string> locationsUri;
	for(size_t i = 0 ; i < server.getServerLocations().size(); ++i)
		locationsUri.push_back(server.getServerLocations()[i].getLocationUri());
	std::string title = Logger::createFancyTitle("Server Settings print");
	os << title << std::endl;
	os << "Server id: " << server.getServerId() << std::endl;
	os << "Server name: " << server.getServerName() << std::endl;
	os << "Server port: " << server.getPort() << std::endl;
	os << Logger::logVector(locationsUri, "Locations Uri").str();
	os << static_cast<DefaultSettings>(server) << std::endl;
	os << Logger::getCharSequence(title.size(), '-')<<std::endl;
	return os;
}
