#include "VirtualServer.hpp"
#include "DefaultSettings.hpp"
#include "../Parsing/Directive.hpp"
#include "LocationSettings.hpp"
#include "../Parsing/ParsingUtils.hpp"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <vector>
#include "../Utils/Logger.hpp"

VirtualServer::VirtualServer()
{
	_serverId = -1;
}

VirtualServer::VirtualServer(int serverId, DefaultSettings& setings, std::vector<Token>& allTokens)
:DefaultSettings(setings), _serverId(serverId)
{
	_serverTokens = Token::getAllServerTokens(serverId, allTokens);
	_serverDirectives = Directive::getAllServerDirectives(_serverTokens);
	checkDuplicateDirectives(_getServerLevelDirectives());
	_applyAllServerLevelDirectives();
	_serverLocations = _setServerLocations();
}
VirtualServer::VirtualServer(const VirtualServer& source)
: DefaultSettings(source), _serverId(source._serverId)
{
	(*this) = source;
}

VirtualServer& VirtualServer::operator=(const VirtualServer& source)
{
	_serverId = source._serverId;
	_serverDirectives = source._serverDirectives;
	_serverTokens = source._serverTokens;
	_serverLocations = source._serverLocations;
	return(*this);
}

VirtualServer::~VirtualServer()
{

}

bool VirtualServer::amIServerLocation(const std::string& path) const
{
	bool found = true;
	fetchLocationWithUri(path, found);
	return found;
}

std::string VirtualServer::getLocationURIfromPath(const std::string& fullPath) const
{
	std::string toTry(fullPath);
	while (true)
	{
		if(amIServerLocation(toTry) == true)
			return toTry;
		toTry = ParsingUtils::getDirName(toTry);
	}
}

void VirtualServer::addDirectiveToServer(Directive directive)
{
	_serverDirectives.push_back(directive);
}

const std::vector<Directive>& VirtualServer::getServerDirectives(void) const
{
	return(_serverDirectives);
}

const std::vector<Token>& VirtualServer::getServerTokens(void) const 
{
	return (_serverTokens);
}

const std::vector<LocationSettings>& VirtualServer::getServerLocations() const 
{
	return (_serverLocations);
}

const int& VirtualServer::getServerId() const 
{
	return(_serverId);
}

std::vector<LocationSettings>::const_iterator VirtualServer::fetchLocationWithUri(const std::string uri, bool& found) const
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

const std::vector<Directive> VirtualServer::_getServerLevelDirectives() const
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
std::vector<LocationSettings> VirtualServer::_setServerLocations()
{
	std::vector<LocationSettings> serverLocations;
	for(size_t i = 0; i < _serverTokens.size(); i++)
	{
		if(_serverTokens[i].getCurrentTokenContextType() == Token::LOCATION)
		{
			LocationSettings location(*this, _serverTokens[i], _serverTokens);
			serverLocations.push_back(location);
		}
	}
	if(_hasDefaultLocation(serverLocations) == false)
		_generateDefaultLocation(serverLocations);
	return serverLocations;
}

bool VirtualServer::_hasDefaultLocation(const std::vector<LocationSettings>& serverLocation) const
{
	std::vector<LocationSettings>::const_iterator it = serverLocation.begin();
	for(;it != serverLocation.end(); it++)
	{
		if(it->getLocationUri() == "/")
			return true;
	}
	return false;
}

void VirtualServer::_generateDefaultLocation(std::vector<LocationSettings>& serverLocation)
{
	LocationSettings location(*this, _serverTokens);
	serverLocation.push_back(location);
}

//apply every directive server have //but only on server level check path. Not location Level
void VirtualServer::_applyAllServerLevelDirectives()
{
	for(size_t i = 0; i < _serverDirectives.size(); i++)
	{
		//if path is 2 that means directive have http server
		if(_serverDirectives[i].getDirectivePathSize() == 2)
			_serverDirectives[i].apply(*this);
	}
}
std::ostream& operator<<(std::ostream& os, const VirtualServer& server)
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
