#include "LocationSettings.hpp"
#include "DefaultSettings.hpp"
#include "../Parsing/Directive.hpp"
#include "VirtualServer.hpp"
#include "../Parsing/Token.hpp"
#include <cstddef>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include "../Utils/Logger.hpp"
#include "../Parsing/Configuration.hpp"



const std::vector<Directive>& LocationSettings::getLocationDirectives(void) const
{
	return(_locationDirectives);
}

const std::string& LocationSettings::getLocationUri() const 
{
	return(_uri);
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
LocationSettings::LocationSettings()
{

}

LocationSettings::LocationSettings(const DefaultSettings& settings, const Token& locationToken,
std::vector<Token>& serverTokens)
:
DefaultSettings(settings),_locationToken(locationToken)
{
	_parentServerTokens = serverTokens;
	_uri = _getUriFromToken(locationToken);
	if(_uri == "")
	{
		std::ostringstream oss; 
		oss <<"Found location that have no _uri in line " << _locationToken.getTokenLineNumber();
		Logger::error("Location error: ", oss.str());
		throw Configuration::InvalidConfigFileException();
	}
	_locationDirectives = _setLocationDirectives();
	checkDuplicateDirectives(_locationDirectives);
	Directive::applyAllDirectives(_locationDirectives, (*this));
}
LocationSettings::LocationSettings(const DefaultSettings& settings,
std::vector<Token>& serverTokens)
:
DefaultSettings(settings)
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
_parentServerTokens(source._parentServerTokens)
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
	os << "I belong to this server: " << std::endl;
	os << Logger::logMap(location.p_acceptedMethods, "Limit except map").str();
	os << location.getNginxReturn() << std::endl;
	os << static_cast<DefaultSettings>(location);
	return os;
}
