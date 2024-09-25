#include "LocationSettings.hpp"
#include "DefaultSettings.hpp"
#include "Directive.hpp"
#include "ServerSettings.hpp"
#include "Token.hpp"
#include <cstddef>
#include <ostream>
#include <string>
#include <vector>
#include "../Utils/Logger.hpp"
#include "../Utils/FileUtils.hpp"



const std::vector<Directive>& LocationSettings::getLocationDirectives(void) const
{
	return(_locationDirectives);
}

const std::string& LocationSettings::getLocationUri() const 
{
	return(_uri);
}


bool LocationSettings::setIndexPagePath(std::string& pathToIndex) const
{
	pathToIndex.erase();
	for(size_t i = 0; i < p_index.size(); i++)
	{
		std::string path = this->p_root + "/" + p_index[i];
		Logger::info("path is ", path);
		bool found = FileUtils::isPathValid(path);
		if(found == true)
		{
			Logger::info("File found", "");
			pathToIndex = path;
			return true;
		}
		else
		{
			Logger::warning("File is not found", "");
		}
	}
	return false;
}

bool LocationSettings::isCgiLocation(void) const
{
	if(getCgiExtensions().size() == 0)
		return false;
	return true;
}

bool LocationSettings::isCgiExtensionSet(const std::string& scriptExtension) const
{
	for(size_t i = 0; i < p_cgiExtensions.size(); i++)
	{
		if(p_cgiExtensions[i] == scriptExtension)
			return true;
	}
	return false;
}
LocationSettings::LocationSettings(const ServerSettings& locationServer)
:_locationServer(locationServer)
{

}

LocationSettings::LocationSettings(const DefaultSettings& settings, const Token& locationToken,
std::vector<Token>& serverTokens, const ServerSettings& locationServer)
:
DefaultSettings(settings),_locationToken(locationToken),
_locationServer(locationServer)
{
	_parentServerTokens = serverTokens;
	_uri = _getUriFromToken(locationToken);
	_locationDirectives = _setLocationDirectives();
	checkDuplicateDirectives(_locationDirectives);
	Directive::applyAllDirectives(_locationDirectives, (*this));
}
LocationSettings::LocationSettings(const DefaultSettings& settings,
std::vector<Token>& serverTokens, const ServerSettings& locationServer)
:
DefaultSettings(settings),
_locationServer(locationServer)
{
	_parentServerTokens = serverTokens;
	_uri = "/";
	_locationDirectives = _setLocationDirectives();
	Directive::applyAllDirectives(_locationDirectives, (*this));
}

LocationSettings::LocationSettings(const LocationSettings& source)
:
DefaultSettings(source),
_uri(source._uri),
_locationToken(source._locationToken),
_locationDirectives(source._locationDirectives), 
_parentServerTokens(source._parentServerTokens),
_locationServer(source._locationServer)
{
	//(*this) = source;
}

LocationSettings& LocationSettings::operator=(const LocationSettings& source)
{
	_locationToken = source._locationToken;
	_uri = source._uri;
	_locationDirectives = source._locationDirectives;
	_parentServerTokens = source._parentServerTokens;
	return (*this);
}

LocationSettings::~LocationSettings()
{

}

std::string LocationSettings::_getUriFromToken(const Token& token)
{
	std::string uri = token.getTokenInfo();
	std::string _directiveName("location");
	size_t namePos = uri.find(_directiveName) + _directiveName.size();
	uri = uri.substr(namePos);
	for(std::string::iterator i = uri.begin(); i < uri.end() ; i++)
	{
		if(*i == ' ' || *i == '\t')
		{
			uri.erase(i);
			i--;
		}
		else
			break;
	}
	return uri;
}

std::vector<Directive> LocationSettings::_setLocationDirectives()
{
	std::vector<Directive> locationDirectives;
	//std::cout << "HERE all server directives are " << std::endl;
	//Token::printAllTokensInfo(_serverTokens);
	for(size_t i = 0; i < _parentServerTokens.size(); i++)
	{
		std::vector<Token> tokenPath = _parentServerTokens[i].getTokenPath();
		if(tokenPath.size() != 3)
			continue;
		if(tokenPath[2].getTokenInfo() == _locationToken.getTokenInfo())
		{
			Directive oneLocDir(_parentServerTokens[i]);
			locationDirectives.push_back(oneLocDir);
		}
	}
	return (locationDirectives);
}

std::ostream& operator<<(std::ostream& os, const LocationSettings& location)
{
	std::string title = Logger::createFancyTitle("Location setting print");
	os << title << std::endl;
	os << "Location uri: " << location.getLocationUri() << std::endl;
	os << "I belong to this server: ";
	os << location._locationServer.getServerName() << ":" <<  location._locationServer.getPort() << std::endl;
	os << Logger::logMap(location.p_acceptedMethods, "Limit except map").str();
	os << location.getNginxReturn() << std::endl;
	return os;
}
// void LocationSettings::printLocationSettings(void) const 
// {
// 	//_locationServer->printServerSettings();
// 	std::cout << "---------------LOCATION SETTINGS ---------------" <<  std::endl;
// 	std::cout << "Location :" << _uri << std::endl;
// 	std::cout << "Root: " << p_root << std::endl;
// 	ParsingUtils::printMap(p_errorPages, "Location Error Pages");
// 	ParsingUtils::printMap(p_acceptedMethods, "Location accepted methods");
// 	std::cout << "Default client Max Body is " << p_clientMaxBody << std::endl;
// 	std::cout << "Default Auto index is " << p_autoindex << std::endl;
// 	p_return.printNginxReturnInfo();
// 	ParsingUtils::printVector(p_index, "indexes");
// 	std::cout << "Location directives: "<< std::endl;
// 	// for(size_t i = 0; i < _locationDirectives.size(); i++)
// 	// {
// 	// 	_locationDirectives[i].printDirectiveInfor();
// 	// }
// 	std::cout << "___________________________________________________"<<std::endl;
// }
