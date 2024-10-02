#include "DefaultSettings.hpp"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>
#include "../Parsing/NginxReturn.hpp"
#include "../Utils/FileUtils.hpp"
#include "../Utils/Logger.hpp"
#include "../Parsing/Configuration.hpp"


const bool& DefaultSettings::getFirstListenApplyFlag() const
{
	return (p_firstListenApply);
}

const bool& DefaultSettings::getFirstNameApply() const 
{
	return (p_firstNameApply);
}

void DefaultSettings::setListenFlagFalse() 
{
	p_firstListenApply = false;
}

void DefaultSettings::setNameFlagFalse()
{
	p_firstNameApply = false;
}

bool DefaultSettings::isListeningToPort(const int& portToCheck) const
{
	const std::vector<int>& serverPorts(getPorts());
	std::vector<int>::const_iterator it = std::find(serverPorts.begin(), serverPorts.end(), portToCheck);
	if(it == serverPorts.end())
		return false;
	return true;
}

bool DefaultSettings::isContainingName(const std::string& nameToCheck) const 
{
	const std::vector<std::string>& serverNames(getServerName());
	std::vector<std::string>::const_iterator it = std::find(serverNames.begin(), serverNames.end(), nameToCheck);
	if(it == serverNames.end())
		return false;
	return true;
}

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

void DefaultSettings::removeDefaultListen()
{
	const int listenPort = DEFAULT_LISTEN_PORT;
	std::vector<int>::iterator it = std::find(p_listenPort.begin(), p_listenPort.end(), listenPort);
	if(it != p_listenPort.end())
		p_listenPort.erase(it);
}

void DefaultSettings::removeDefaultName()
{
	const std::string& serverName = DEFAULT_SERVER_NAME;
	std::vector<std::string>::iterator it = std::find(p_serverName.begin(), p_serverName.end(), serverName);
	if(it != p_serverName.end())
		p_serverName.erase(it);
}
DefaultSettings::DefaultSettings()
{
	p_serverName.push_back(DEFAULT_SERVER_NAME);
	p_listenPort.push_back(DEFAULT_LISTEN_PORT);
	p_firstListenApply = true;
	_setDefaultHttpMethods();
	_setDefaultIndexes();
	p_clientMaxBody = DEFAULT_MAX_BODY_SIZE;
	p_autoindex = false;
	p_root = "/"; //TODO fix later
	p_uploadFolder = "Uploads";//TODO: check if this folder exist
}

DefaultSettings::DefaultSettings(const DefaultSettings& source)
{
	(*this) = source;
}

DefaultSettings& DefaultSettings::operator=(const DefaultSettings& source)
{
	p_serverName = source.p_serverName;
	p_listenPort = source.p_listenPort;
	p_firstListenApply = source.p_firstListenApply;
	p_errorPages = source.p_errorPages;
	p_acceptedMethods = source.p_acceptedMethods;
	p_clientMaxBody = source.p_clientMaxBody;
	p_autoindex = source.p_autoindex;
	p_return = source.p_return;
	p_index = source.p_index;
	p_root = source.p_root;
	p_cgiExtensions = source.p_cgiExtensions;
	p_uploadFolder = source.p_uploadFolder;
	return (*this);
}

DefaultSettings::~DefaultSettings()
{

}


void DefaultSettings::addListenPort(const int& listenPort)
{
	p_listenPort.push_back(listenPort);
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

void DefaultSettings::addServerName(const std::string& serverName)
{
	p_serverName.push_back(serverName);
}

void DefaultSettings::setCgiExtensions(std::vector<std::string> extensionVector)
{
	p_cgiExtensions = extensionVector;
}


void DefaultSettings::setUploadFolder(const std::string& folderName)
{
	p_uploadFolder = folderName;
}

const std::vector<int>& DefaultSettings::getPorts(void) const
{
	return(p_listenPort);
}

const std::string& DefaultSettings::getRoot(void) const 
{
	return(p_root);
}

const std::vector<std::string>& DefaultSettings::getServerName(void) const
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

const std::string& DefaultSettings::getUploadFolder(void) const 
{
	return p_uploadFolder;
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
