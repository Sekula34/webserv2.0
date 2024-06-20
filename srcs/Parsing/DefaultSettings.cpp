#include "DefaultSettings.hpp"
#include <cstddef>
#include <iostream>
#include <vector>
#include "ParsingUtils.hpp"
DefaultSettings::DefaultSettings()
{
	_serverName = "[Default Server Name]";
	_listenPort = 8080;
	_host = "[Default HOST]";
	_errorPages[404] = "/html/404.html";
	_setDefaultHttpMethods();
	_setDefaultIndexes();
	_clientMaxBody = 1000000;
	_autoindex = false;
	_root = "/";
}

DefaultSettings::DefaultSettings(const DefaultSettings& source)
{
	(*this) = source;
}

DefaultSettings& DefaultSettings::operator=(const DefaultSettings& source)
{
	_serverName = source._serverName;
	_listenPort = source._listenPort;
	_host = source._host;
	_errorPages = source._errorPages;
	_acceptedMethods = source._acceptedMethods;
	_clientMaxBody = source._clientMaxBody;
	_autoindex = source._autoindex;
	_return = source._return;
	_index = source._index;
	_root = source._root;
	return (*this);
}

DefaultSettings::~DefaultSettings()
{

}

void DefaultSettings::printAllSettings(void) const 
{
	std::cout << "---------------DEFAULT ALL SETTINGS PRINT ---------------" <<  std::endl;
	std::cout << "Default server name :" << _serverName << std::endl;
	std::cout << "Default listen port:" << _listenPort << std::endl;
	std::cout << "Defautl host: " << _host << std::endl;
	std::cout << "Default root: " <<_root << std::endl;
	ParsingUtils::printMap(_errorPages, "ErrorPages");
	ParsingUtils::printMap(_acceptedMethods);
	//std::cout << "Default limit except: " << _limitExcept << std::endl;
	std::cout << "Default client Max Body is " << _clientMaxBody << std::endl;
	std::cout << "Default Auto index is " << _autoindex << std::endl;
	_return.printNginxReturnInfo();
	ParsingUtils::printVector(_index, "Index vector");
	std::cout << "___________________________________________________"<<std::endl;
}


void DefaultSettings::setAllAcceptedMethodsToFalse(void)
{
	std::map<std::string, bool>::iterator it;
	for(it = _acceptedMethods.begin(); it != _acceptedMethods.end(); it++)
	{
		it->second = false;
	}
}

void DefaultSettings::setAcceptedMethodToTrue(std::string methodName)
{
	std::map<std::string, bool>::iterator it = _acceptedMethods.find(methodName);
	if(it != _acceptedMethods.end())
		it->second = true;
	return;
}

void DefaultSettings::_setDefaultHttpMethods(void)
{
	_acceptedMethods["GET"] = true;
	_acceptedMethods["POST"] = true;
	_acceptedMethods["DELETE"] = true;
}

void DefaultSettings::_setDefaultIndexes(void)
{
	_index.push_back("index");
	_index.push_back("index.html");
}

void DefaultSettings::setListenPort(int listenPort)
{
	_listenPort = listenPort;
}

void DefaultSettings::setErrorPage(int errorCode ,std::string path)
{
	std::map<int, std::string>::iterator it = _errorPages.find(errorCode);
	if(it != _errorPages.end())
	{
		_errorPages.erase(it);
	}
	_errorPages[errorCode] = path;
}

void DefaultSettings::setClientMaxBodySize(size_t maxSize)
{
	_clientMaxBody = maxSize;
}

void DefaultSettings::setAutoIndex(bool state)
{
	_autoindex = state;
}

void DefaultSettings::setNginxReturn(int statusCode, std::string redirectPath, bool flag)
{
	_return.setFlag(flag);
	_return.setStatus(statusCode);
	_return.setRedirectPath(redirectPath);
}

void DefaultSettings::setIndexes(std::vector<std::string> indexes)
{
	_index.clear();
	_index = indexes;
}

void DefaultSettings::setRoot(std::string root)
{
	_root = root;
}

void DefaultSettings::setServerName(std::string serverName)
{
	_serverName = serverName;
}

const int& DefaultSettings::getPort(void) const
{
	return(_listenPort);
}

const std::string& DefaultSettings::getRoot(void) const 
{
	return(_root);
}

const std::string& DefaultSettings::getServerName(void) const
{
	return(_serverName);
}
