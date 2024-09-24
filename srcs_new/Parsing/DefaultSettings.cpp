#include "DefaultSettings.hpp"
#include <cstddef>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>
#include "NginxReturn.hpp"
#include "../Utils/FileUtils.hpp"
#include "../Utils/Logger.hpp"
#include "Configuration.hpp"


bool DefaultSettings::isMethodAllowed(std::string method) const
{
	std::map<std::string, bool>::const_iterator it = p_acceptedMethods.find(method);
	if(it != p_acceptedMethods.end() && it->second == true)
		return true;
	return false;
}

void DefaultSettings::setAllAcceptedMethodsToFalse(void)
{
	std::map<std::string, bool>::iterator it;
	for(it = p_acceptedMethods.begin(); it != p_acceptedMethods.end(); it++)
	{
		it->second = false;
	}
}

void DefaultSettings::setAcceptedMethodToTrue(std::string methodName)
{
	std::map<std::string, bool>::iterator it = p_acceptedMethods.find(methodName);
	if(it != p_acceptedMethods.end())
		it->second = true;
	return;
}

void DefaultSettings::checkDuplicateDirectives(const std::vector<Directive>& dirVec)
{
	const Directive* duplicate = NULL;
	if(Directive::isDuplicateDirectivePresent(dirVec, duplicate) == true)
	{
		if(duplicate != NULL)
		{
			std::ostringstream oss;
			oss << "[" << duplicate->getDirectiveName() << "]" << " is duplicate in " << FileUtils::getConfigFilePath() << ":";
			oss << duplicate->getDirectiveLineNum();
			Logger::error(oss.str(), true);
		}
		throw Configuration::InvalidConfigFileException();
	}
}
DefaultSettings::DefaultSettings()
{
	p_serverName = "[Default Server Name]";
	p_listenPort = 8080;
	p_host = "[Default HOST]";
	_setDefaultHttpMethods();
	_setDefaultIndexes();
	p_clientMaxBody = 1000000;
	p_autoindex = false;
	p_root = "/";
}

DefaultSettings::DefaultSettings(const DefaultSettings& source)
{
	(*this) = source;
}

DefaultSettings& DefaultSettings::operator=(const DefaultSettings& source)
{
	p_serverName = source.p_serverName;
	p_listenPort = source.p_listenPort;
	p_host = source.p_host;
	p_errorPages = source.p_errorPages;
	p_acceptedMethods = source.p_acceptedMethods;
	p_clientMaxBody = source.p_clientMaxBody;
	p_autoindex = source.p_autoindex;
	p_return = source.p_return;
	p_index = source.p_index;
	p_root = source.p_root;
	p_cgiExtensions = source.p_cgiExtensions;
	return (*this);
}

DefaultSettings::~DefaultSettings()
{

}


void DefaultSettings::setListenPort(int listenPort)
{
	p_listenPort = listenPort;
}

void DefaultSettings::setErrorPage(int errorCode ,std::string path)
{
	std::map<int, std::string>::iterator it = p_errorPages.find(errorCode);
	if(it != p_errorPages.end())
	{
		p_errorPages.erase(it);
	}
	p_errorPages[errorCode] = path;
}

void DefaultSettings::setClientMaxBodySize(size_t maxSize)
{
	p_clientMaxBody = maxSize;
}

void DefaultSettings::setAutoIndex(bool state)
{
	p_autoindex = state;
}

void DefaultSettings::setNginxReturn(int statusCode, std::string redirectPath, bool flag)
{
	p_return.setFlag(flag);
	p_return.setStatus(statusCode);
	p_return.setRedirectPath(redirectPath);
}

void DefaultSettings::setIndexes(std::vector<std::string> indexes)
{
	p_index.clear();
	p_index = indexes;
}

void DefaultSettings::setRoot(std::string root)
{
	p_root = root;
}

void DefaultSettings::setServerName(std::string serverName)
{
	p_serverName = serverName;
}

void DefaultSettings::setCgiExtensions(std::vector<std::string> extensionVector)
{
	p_cgiExtensions = extensionVector;
}

const int& DefaultSettings::getPort(void) const
{
	return(p_listenPort);
}

const std::string& DefaultSettings::getRoot(void) const 
{
	return(p_root);
}

const std::string& DefaultSettings::getServerName(void) const
{
	return(p_serverName);
}

const NginnxReturn& DefaultSettings::getNginxReturn(void) const 
{
	return(p_return);
}

const bool& DefaultSettings::getAutoindexFlag(void) const
{
	return (p_autoindex);
}

const std::vector<std::string>& DefaultSettings::getIndexes() const 
{
	return (p_index);
} 

const std::string DefaultSettings::getErrorPagePath(const int errorCode) const
{
	std::string path = "";
	std::map<int, std::string>::const_iterator it;
	it = p_errorPages.find(errorCode);
	if(it != p_errorPages.end())
	{
		path = it->second;
	}
	return path;
}

const std::vector<std::string>& DefaultSettings::getCgiExtensions(void) const
{
	return p_cgiExtensions;
}

void DefaultSettings::_setDefaultHttpMethods(void)
{
	p_acceptedMethods["GET"] = true;
	p_acceptedMethods["POST"] = true;
	p_acceptedMethods["DELETE"] = true;
}

void DefaultSettings::_setDefaultIndexes(void)
{
	p_index.push_back("index");
	p_index.push_back("index.html");
}
std::ostream& operator<<(std::ostream& os, const DefaultSettings& settings)
{
	std::string title = Logger::createFancyTitle("Http Settings print", '^');
	os << title << std::endl;
	os << "Client Max Body size: " << settings.p_clientMaxBody << std::endl;
	os << "Autoindex: " << settings.p_autoindex << std::endl;
	os << "Root: " << settings.p_root << std::endl;
	os << Logger::logMap(settings.p_errorPages, "Error Pages Map").str();
	os << Logger::logVector(settings.p_index, "Indexes vector").str() << std::endl;
	return os;
}
