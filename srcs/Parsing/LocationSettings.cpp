#include "LocationSettings.hpp"
#include "DefaultSettings.hpp"
#include "Directive.hpp"
#include "ServerSettings.hpp"
#include "Token.hpp"
#include <cstddef>
#include <iostream>
#include <vector>
#include "ParsingUtils.hpp"
#include "../Utils/Logger.hpp"
#include "../Utils/FileUtils.hpp"
LocationSettings::LocationSettings()
{

}

LocationSettings::LocationSettings(const DefaultSettings& settings, const Token& locationToken,
std::vector<Token>& serverTokens)
:DefaultSettings(settings),_locationToken(locationToken)
{
	_parentServerTokens = serverTokens;
	_uri = _getUriFromToken(locationToken);
	_locationDirectives = _setLocationDirectives();
	p_checkDuplicateDirectives(_locationDirectives);
	Directive::applyAllDirectives(_locationDirectives, (*this));
}
LocationSettings::LocationSettings(const DefaultSettings& settings,
std::vector<Token>& serverTokens)
:DefaultSettings(settings)
{
	_parentServerTokens = serverTokens;
	_uri = "/";
	_locationDirectives = _setLocationDirectives();
	Directive::applyAllDirectives(_locationDirectives, (*this));
}

LocationSettings::LocationSettings(const LocationSettings& source)
:DefaultSettings(source), _uri(source._uri),  _locationToken(source._locationToken),
  _locationDirectives(source._locationDirectives), _parentServerTokens(source._parentServerTokens)
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

void LocationSettings::printLocationSettings(void) const 
{
	//_locationServer->printServerSettings();
	std::cout << "---------------LOCATION SETTINGS ---------------" <<  std::endl;
	std::cout << "Location :" << _uri << std::endl;
	std::cout << "Root: " << _root << std::endl;
	ParsingUtils::printMap(_errorPages, "Location Error Pages");
	ParsingUtils::printMap(_acceptedMethods, "Location accepted methods");
	std::cout << "Default client Max Body is " << _clientMaxBody << std::endl;
	std::cout << "Default Auto index is " << _autoindex << std::endl;
	_return.printNginxReturnInfo();
	ParsingUtils::printVector(_index, "indexes");
	std::cout << "Location directives: "<< std::endl;
	// for(size_t i = 0; i < _locationDirectives.size(); i++)
	// {
	// 	_locationDirectives[i].printDirectiveInfor();
	// }
	std::cout << "___________________________________________________"<<std::endl;
}

std::vector<Directive> LocationSettings::getLocationDirectives(void) const
{
	return(_locationDirectives);
}

const std::string& LocationSettings::getLocationUri() const 
{
	return(_uri);
}

void LocationSettings::printAllLocationSettings(std::vector<LocationSettings> &allLocations)
{
	for(size_t i = 0; i < allLocations.size(); i++)
	{
		allLocations[i].printLocationSettings();
	}
}

bool LocationSettings::setIndexPagePath(std::string& pathToIndex) const
{
	pathToIndex.erase();
	for(size_t i = 0; i < _index.size(); i++)
	{
		std::string path = this->_root + "/" + _index[i];
		Logger::info("path is "); std::cout << path<< std::endl;
		bool found = FileUtils::isPathValid(path);
		if(found == true)
		{
			Logger::info("File found");
			pathToIndex = path;
			return true;
		}
		else
		{
			Logger::warning("File is not found");
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
