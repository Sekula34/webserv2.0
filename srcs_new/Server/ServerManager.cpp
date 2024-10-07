#include "ServerManager.hpp"
#include "../Parsing/Configuration.hpp"
#include "DefaultSettings.hpp"
#include "../Parsing/Directive.hpp"
#include "LocationSettings.hpp"
#include "VirtualServer.hpp"
#include "../Parsing/Token.hpp"
#include <cstddef>
#include <fcntl.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include "../Utils/Logger.hpp"
#include "../Client/Client.hpp"
#include "../Message/Message.hpp"
#include "../Message/Node.hpp"
#include "../Message/RequestHeader.hpp"
#include "../Client/Client.hpp"
#include "../Parsing/ParsingUtils.hpp"
#include "ResponseGenerator.hpp"
#include <cstdlib> // For strtol
#include <cerrno> // For errno.

// #include "../Utils/Logger.hpp"
// #include "../Utils/HttpStatusCode.hpp"
// #include "../Message/Message.hpp"

void ServerManager::_assignVirtualServer(Client& client)
{
	if(client.getVirtualServer() != NULL)
		return;
	if(client.getErrorCode() == 400)
	{
		client.setVirtualServer(getServerById(1)); 
		return;
	}
	const RequestHeader* reqHeader = static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader());
	int port = reqHeader->getHostPort(); //TODO: change getHostPort to return error value when there is no Host Port
	std::string serverName = reqHeader->getHostName();

	const VirtualServer* server = getServerByPort(port, serverName);
	if (server == NULL)
	{
		client.setErrorCode(400);
		client.setVirtualServer(getServerById(1)); 
		return;
	}
	if(server->isListeningToPort(client.getClientPort()) == false) 
	{
		client.setErrorCode(400);
		client.setVirtualServer(getServerById(1)); 
		return;
	}
	client.setVirtualServer(*server);
}

bool	ServerManager::_isCgi(Client& client)
{
	bool locFound = false;
	std::vector<LocationSettings>::const_iterator location = _setCgiLocation(client, locFound);
	if(locFound)
	{
		// Logger::info("Cgi checked and it exist on this location :", location->getLocationUri()); 
		if(_parseCgiURLInfo(*location, client) == true)
			return (ServerManager::_isSupportedScriptExtenstion(*location, client));
	}
	return false;
}

bool	ServerManager::_isPostRequest(Client& client)
{
	RequestHeader& header = *static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader());
	std::string clientMethod = header.getRequestLine().requestMethod;
	if (clientMethod == "POST")
	{
		// Logger::warning("The method is POST: ", clientMethod);
		int fd = open("test.bin", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
		if (fd == -1)
		{
			Logger::error("----- Testing: Unable to create file", "");
			client.setErrorCode(500);
			return (false);
		}
		client.setFileFd(fd);
		return (true);
	}
	// Logger::warning("The Method is this one: ", clientMethod);
	return (false);
}

// Method to check header against the Virtual Server Config (allowed methods and bodySioze limit)
//false is error. true is good.
bool	ServerManager::_checkIfRequestAllowed(Client& client)
{
	if(client.getErrorCode() != 0)
		return false;
	RequestHeader& header = *static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader());
	const VirtualServer& server = *client.getVirtualServer();
	const std::string& clientUriPath = header.urlSuffix->getPath();
	const std::string& serverLocationUri = server.getLocationURIfromPath(clientUriPath);
	bool found = false;
	std::vector<LocationSettings>::const_iterator it = server.fetchLocationWithUri(serverLocationUri, found);
	if(found == true)
	{
		const LocationSettings& reponseLocation = *it;
		// Check if the method is allowed
		std::string clientMethod = header.getRequestLine().requestMethod;
		if(reponseLocation.isMethodAllowed(clientMethod) == false)
		{
			client.setErrorCode(405); //TODO: Nginx considers this as 403 forbbiden
			Logger::warning("Seted 405 method not allowed", 405);
			return false;
		}
		// Check if bodySize is within boundary
		const std::map<std::string, std::string>& headerFieldMap = header.getHeaderFieldMap();
		std::map<std::string, std::string>::const_iterator itHeader = headerFieldMap.find("Content-Length");
		if (itHeader != headerFieldMap.end())
		{
			errno = 0;
			char*	end;
			long bodySize = strtol(itHeader->second.c_str(), &end, 10);
			if (end == itHeader->second.c_str() || *end != '\0' || errno == ERANGE || bodySize < 0)
			{
				client.setErrorCode(400);
				Logger::warning("Seted 400 Bad request", 400);
				return false;
			}
			if (reponseLocation.getClientMaxBody() < static_cast<size_t>(bodySize))
			{
				client.setErrorCode(413);
				Logger::warning("Seted 413 Content Too Large", 413);
				return false;
			}
		}
		return (true);
	}
	Logger::warning("Client requested location that is not in this server ", clientUriPath);
	client.setErrorCode(404);
	return false;
}

// MR: Method for checking if bodysize is withing the boundaries of Directive client_max_body_size
// FIXME: Mabe its good to code function for get client's responseLocation. (repeated here and _checkIfRequestAllowed).
bool	ServerManager::_checkBodySizeLimit(Client& client)
{
	if(client.getErrorCode() != 0)
		return false;
	RequestHeader& header = *static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader());
	const VirtualServer& server = *client.getVirtualServer();
	const std::string& clientUriPath = header.urlSuffix->getPath();
	const std::string& serverLocationUri = server.getLocationURIfromPath(clientUriPath);
	bool found = false;
	std::vector<LocationSettings>::const_iterator it = server.fetchLocationWithUri(serverLocationUri, found);
	if(found == true)
	{
		const LocationSettings& reponseLocation = *it;
		// Check if clien_max_body_size is exceeded
		if (reponseLocation.getClientMaxBody() < client.getMsg(Client::REQ_MSG)->getBodySize())
		{
			client.setErrorCode(413);
			Logger::warning("Seted 413 Content Too Large", 413);
			return false;
		}
		return (true);
	}
	Logger::warning("Client requested location that is not in this server ", clientUriPath);
	client.setErrorCode(404);
	return false;
}

void ServerManager::loop()
{
	std::map<int, Client*>::iterator it = Client::clients.begin();
	for (; it != Client::clients.end(); ++it)
	{
		Client& client = *(it->second);
		if(client.getClientState() == Client::DELETEME)
			continue;
		if(client.getErrorCode() != 0)
			client.setClientState(Client::DO_RESPONSE);
		if (client.getMsg(Client::REQ_MSG)->getChain().begin()->getState() != COMPLETE) //TODO: check if header is complete not full req
			continue;
		_assignVirtualServer(client); //TODO:  check if VS assignment with incomplete header could segfault
		// MR: Check message header against Directives (method and content-length)
		if(client.getIsRequestChecked() == false)
		{
			if(_checkIfRequestAllowed(client) == false)
				client.setClientState(Client::DO_RESPONSE);
			client.setIsRequestChecked();
		}
		// MR: Check message bodySize against Directives (client_max_body_size)
		if (client.getClientState() == Client::DO_REQUEST)
		{
			if (_checkBodySizeLimit(client) == false)
				client.setClientState(Client::DO_RESPONSE);
		}
		// if (client.getMsg(Client::REQ_MSG)->getState() != COMPLETE) //TODO: check if header is complete not full req
		// 	continue;
		if(client.getClientState() == Client::DO_REQUEST && client.getMsg(Client::REQ_MSG)->getState() == COMPLETE)
		{
			// if(client.getErrorCode() != 0)
			// 	client.setClientState(Client::DO_RESPONSE);
			if(_isCgi(client) == true)
			{
				client.setCgiFlag(true);
				Logger::warning("Client Requested valid cgi, ID: ", client.getId());
				client.setClientState(Client::DO_CGISEND);

				// TODO: this is fake set state, just to make webserve work without cgi
				// client.setClientState(Client::DO_RESPONSE);
			}
			else if (_isPostRequest(client))
			{
				Logger::warning("Client made valid POST request, ID: ", client.getId());
				client.setClientState(Client::DO_FILEWRITE);
			}
			else
			{
				Logger::warning("No cgi will be executed, client error: ", client.getErrorCode());
				client.setClientState(Client::DO_RESPONSE);
			}
			//change state
		}
		if(client.getClientState() == Client::DO_RESPONSE)
		{
			ResponseGenerator::generateClientResponse(client);
			//client.setClientState(Client::DELETEME);
			//_createResponse(client);
			// CREATE RESPONSE
		}
	}
}



ServerManager::ServerManager(std::string configPath)
{
	Configuration serversConf(configPath);
	_allTokens = serversConf.getAllTokens();
	_numberOfServers = serversConf.getNumberOfServers();
	if(_numberOfServers < 1)
	{
		Logger::error("There is not eve one server in config file ", configPath);
		throw Configuration::InvalidConfigFileException();
	}

	DefaultSettings defSettings;
	_setHttpDirectives();
	defSettings.checkDefaultFolderAcces();
	DefaultSettings::checkDuplicateDirectives(_httpDirectives);
	Directive::applyAllDirectives(_httpDirectives, defSettings);
	for(int i = 1; i <= _numberOfServers; i++)
	{
		VirtualServer oneServer(i,defSettings,_allTokens);
		_servers.push_back(oneServer);
	}
}

ServerManager::ServerManager(const ServerManager& source)
{
	(void) source;
}

ServerManager& ServerManager::operator=(const ServerManager& source)
{
	(void) source;
	return(*this);
}

ServerManager::~ServerManager()
{

}


const std::vector<VirtualServer>& ServerManager::getAllServers(void) const
{
	return(_servers);
}

const VirtualServer& ServerManager::getServerById(int serverId) const 
{
	int serverIndex = serverId - 1;
	if(serverIndex > _numberOfServers || serverIndex < 0)
	{
		throw std::runtime_error("Will segfault. There is not enough servers or index is less than 0");
	}
	return(_servers[serverIndex]);
}

const VirtualServer* ServerManager::getServerByPort(int portNumber, std::string serverName) const
{
	int serverId;
	const VirtualServer* toReturn = NULL;
	std::vector<VirtualServer> ServersId = _getAllServersIdWithPort(portNumber);
	if(ServersId.size() == 0)
		return NULL;
	if(ServersId.size() == 1 || serverName == "")
	{
		serverId = ServersId[0].getServerId();
		toReturn = &getServerById(serverId);
		return toReturn;
	}
	for(size_t i = 0; i< ServersId.size(); i++)
	{
		VirtualServer& oneServer(ServersId[i]);
		if(oneServer.isContainingName(serverName) == true)
		{
			serverId = oneServer.getServerId();
			return &getServerById(serverId);
		}
	}
	serverId = ServersId[0].getServerId();
	return &getServerById(serverId);
}

// TODO:
// bool ServerManager::_validateRequestHeader(const RequestHeader* header) const
// {
// 	if(header == NULL)
// 	{
// 		Logger::warning("Trying to get Client Server with client that have no header", "");
// 		return false;
// 	}
// 	if(header->getHttpStatusCode() == 400) // or check if code is 400 
// 	{
// 		const std::string reason = HttpStatusCode::getReasonPhrase(header->getHttpStatusCode());
// 		Logger::warning("Client send " + reason, true);
// 		Logger::warning("There is no filled info in header for finding server", true);
// 		return false;
// 	}
// 	return true;
// }

// TODO: Important
// const VirtualServer* ServerManager::getClientServer(const Client& client) const
// {
// 	if(_validateRequestHeader(static_cast<RequestHeader*>(client.getClientMsg()->getHeader())) == false)
// 		return NULL;
// 	int portNumber = static_cast<RequestHeader*>(client.getClientMsg()->getHeader())->getHostPort();
// 	const VirtualServer* toReturn = getServerByPort(portNumber, static_cast<RequestHeader*>(client.getClientMsg()->getHeader())->getHostName());
// 	return toReturn;
// }

//goes through vector of servers
//get server Port, check if port is in unique vector if not add it
//return unique ports;
const std::vector<int> ServerManager::getUniquePorts() const 
{
	std::vector<int> uniquePorts;
	for(size_t i = 0; i < _servers.size(); i++)
	{
		const std::vector<int>& serverPorts = _servers[i].getPorts();
		std::vector<int>::const_iterator it = serverPorts.begin();
		for(; it != serverPorts.end(); it++)
			_addOnePort(uniquePorts, *it);
	}
	return uniquePorts;
}

void ServerManager::_addOnePort(std::vector<int>& uniquePorts, const int serverPort) const
{
	std::vector<int>::iterator it;
	it = std::find(uniquePorts.begin(), uniquePorts.end(), serverPort);
	if(it == uniquePorts.end())
		uniquePorts.push_back(serverPort);
}

std::vector<LocationSettings>::const_iterator ServerManager::_setCgiLocation(Client& client, bool& foundLoc)
{
	foundLoc = false;

	const VirtualServer& cgiServer = *client.getVirtualServer();
	RequestHeader* clientHeader =  static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader());
	std::string ServerLocation = cgiServer.getLocationURIfromPath(clientHeader->urlSuffix->getPath());
	bool found = true;
	std::vector<LocationSettings>::const_iterator it = cgiServer.fetchLocationWithUri(ServerLocation, found);
	if(found == false || it->isCgiLocation() == false)
	{
		foundLoc = false;
		return cgiServer.getServerLocations().end();
	}
	foundLoc = true;
	return it;
}

bool ServerManager::_parseCgiURLInfo(const LocationSettings& cgiLocation,Client& client)
{
	// Logger::info("Called cgi parse url", true);
	RequestHeader& clientHeader = *static_cast<RequestHeader *>(client.getMsg(Client::REQ_MSG)->getHeader());

	std::string fileName = ParsingUtils::getFileNameFromUrl(clientHeader.urlSuffix->getPath(), cgiLocation.getLocationUri());
	std::string scriptName = ParsingUtils::extractUntilDelim(fileName, "/", false);
	if(scriptName == "")
		scriptName = fileName;
	if(clientHeader.urlSuffix->setCgiScriptName(scriptName) == false)
	{
		Logger::warning("Unsupported cgi file type", scriptName);
		client.setErrorCode(502); 
		return false;
	}
	std::string scriptPath = cgiLocation.getLocationUri() + fileName;
	_setCgiPathInfo(clientHeader.urlSuffix->getPath(), scriptName, client);
	// std::ostringstream oss;
	// oss << "Script Name " << clientHeader.urlSuffix->getCgiScriptName() << std::endl;
	// oss << "Script extension: " << clientHeader.urlSuffix->getCgiScriptExtension()  << std::endl;
	// oss << "Path info" << clientHeader.urlSuffix->getCgiPathInfo() << std::endl;
	// Logger::info(oss.str(), "");
	return true;
}

void ServerManager::_setCgiPathInfo(const std::string& urlpath, const std::string scriptPath, Client& client)
{
	size_t pathInfoPos = urlpath.find(scriptPath);
	if(pathInfoPos == std::string::npos)
	{
		Logger::error("Set cgi path this should never happend", "");
		return;
	}
	std::string pathInfo = urlpath.substr(pathInfoPos + scriptPath.size());
	if(_isCgiPathInfoValid(pathInfo) == false)
	{
		Logger::error("not valid cgi path", pathInfo);
		client.setErrorCode(502);
		return;
	}
	static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader())->urlSuffix->setCgiPathInfo(pathInfo);
}

bool ServerManager::_isCgiPathInfoValid(std::string pathInfo)
{
	if(pathInfo.find("/..") != std::string::npos || pathInfo.find("~") != std::string::npos)
	{
		return false;
	}
	return true;
}

bool ServerManager::_isSupportedScriptExtenstion(const LocationSettings& location, Client& client)
{
	RequestHeader* header = static_cast<RequestHeader *>(client.getMsg(Client::REQ_MSG)->getHeader());
	std::string requestedScriptExtension = header->urlSuffix->getCgiScriptExtension();
	if(location.isCgiExtensionSet(requestedScriptExtension) == true)
		return true;
	std::ostringstream oss; 
	oss << "403 is set because requested script extension " << requestedScriptExtension << " is not supported on location " << location.getLocationUri();
	Logger::error(oss.str(), true);
	client.setErrorCode(403);
	return false;
}

//check if Token is directive that belongs to http only
bool ServerManager::_isTokenHttpDirective(const Token& toCheck) const
{
	if(toCheck.getTokenType() != Token::DIRECTIVE)
		return false;
	std::vector<Token> path = toCheck.getTokenPath();
	if(path.size() != 1)
		return false;
	Token::ContextType parentType = path[0].getTokenContextType();
	if(parentType != Token::HTTP )
		return false;
	return true;
}

std::vector<VirtualServer> ServerManager::_getAllServersIdWithPort(int port) const
{
	std::vector<VirtualServer> serversPort;
	for(size_t i = 0; i < _servers.size(); i++)
	{
		const VirtualServer& oneServer(_servers[i]);
		if(oneServer.isListeningToPort(port) == true)
			serversPort.push_back(oneServer);
	}
	return serversPort;
}

void ServerManager::_setHttpDirectives(void)
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
