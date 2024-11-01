#include "../Server/Socket.hpp"
#include "../Server/ConnectionManager.hpp"
#include "../Utils/Data.hpp"
#include "../Utils/Logger.hpp"
#include "../Parsing/ParsingUtils.hpp"
#include "../Message/Message.hpp"
#include "../Message/Node.hpp"
#include "../Message/RequestHeader.hpp"
#include "Cgi.hpp"
#include <exception>
#include <sys/stat.h>
#include "../Server/VirtualServer.hpp" // For constructing DOCUMENT_ROOT

extern volatile sig_atomic_t flag;

//==========================================================================//
// STATIC ATTRIBUTES/METHODS================================================//
//==========================================================================//

static std::string operatingSystem()
{
    #ifdef _WIN32
    return "Windows 32-bit";
    #elif _WIN64
    return "Windows 64-bit";
    #elif __linux__
    return "Linux";
    #elif __APPLE__ || __MACH__
    return "MacOS";
    #elif __unix || __unix__
    return "Unix";
    #elif __FreeBSD__
    return "FreeBSD";
    #else
    return "unknown OS";
    #endif
}  

static std::string	getScriptLocation(char* str)
{
	std::string tmp = str;
	std::string scriptLocation = "";
	size_t pos = tmp.rfind("/");
	if (pos != std::string::npos)
		scriptLocation = tmp.substr(0, pos);
	return (scriptLocation);
}

static void	closeSockets(int* s1, int* s2)
{
	if (s1)
	{
		close(s1[0]);
		close(s1[1]);
	}
	if (s2)
	{
		close(s2[0]);
		close(s2[1]);
	}
}

//==========================================================================//
// REGULAR METHODS==========================================================//
//==========================================================================//

std::string	Cgi::_getInterpreterPath(Client& client, std::string suffix)
{
	if (suffix == "")
		return suffix;
	std::string tmp;
	std::vector<std::string> dirs = ParsingUtils::splitString(Data::findStringInEnvp("PATH="), ':');
	for (std::vector<std::string>::iterator it = dirs.begin(); it != dirs.end(); it++)
	{
		tmp = *it + "/" + Data::getCgiLang().at(suffix);
		// std::cout << "trying to access: " << tmp << std::endl;
		if (access(tmp.c_str(), X_OK) == 0)	
			return (tmp);
	}
	_stopCgiSetErrorCode(client);
	Logger::warning("CGI interpreter not installed:", Data::getCgiLang().at(suffix));
	// std::cout << Data::getCgiLang().at(suffix) << std::endl;
	return (tmp = "");
}

std::string	Cgi::_getScriptName(std::string suffix, Client& client)
{
	std::vector<std::string> sections = ParsingUtils::splitString((static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader()))->urlSuffix->getPath(), '/');
	std::vector<std::string>::iterator it = sections.begin();
	for (; it != sections.end(); it++)
	{
		if ((*it).find(suffix) != std::string::npos)
			return (*it);
	}
	Logger::warning("no valid CGI script name in URI", true);
	_stopCgiSetErrorCode(client);
	return ("");
}

bool	Cgi::_isRegularFile(std::string file)
{
	struct stat sb;

	if (stat(file.c_str(), &sb) == -1)
	{
		Logger::error("stat failed in CGI", true);
		return (false);
	}

	if ((sb.st_mode & S_IFMT) != S_IFREG)
	{
		Logger::warning("CGI: invalid filetype detected!", true);
		return (false);
	}
	if (sb.st_nlink > 1)
	{
		Logger::warning("CGI: script can not be a link or linked to!", true);
		return (false);
	}
	return (true);
}

bool	Cgi::_initArgsList(Client& client, std::vector<std::string>& argsVec)
{
	// the suffix is .py or .php
	std::string suffix = static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader())->urlSuffix->getCgiScriptExtension();

	// this is the location on the server and is hardcoded
	std::string location = "/cgi-bin/";

	// go through PATH variable and check whether interpreter is installed
	// if not installed stops Cgi sets 500 error
	std::string interpreterAbsPath = _getInterpreterPath(client, suffix);
	if (interpreterAbsPath.empty())
		return (false);

	std::string scriptName = static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader())->urlSuffix->getCgiScriptName();
	if (scriptName.empty())
		return (false);

	std::string scriptAbsPath = Data::findStringInEnvp("PWD=") + location
		+ Data::getCgiLang().at(suffix) + "/" + scriptName;

	argsVec.push_back(interpreterAbsPath);
	argsVec.push_back(scriptAbsPath);

	return (true);
}

bool	Cgi::_checkScriptAbsPath(std::vector<std::string>& argsVec)
{
	if (access(argsVec[1].c_str(), X_OK) != 0)	
	{
		Logger::warning("CGI script not executable: ", argsVec[1]);
		return (false);
	}
	if (!_isRegularFile(argsVec[1]))
		return (false);
	return (true);
}

void	Cgi::_createEnvVector(Client& client, std::vector<std::string>& envVec, char** args)
{
	RequestHeader* requestHeader = (static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader()));
	Message* requestMessage = client.getMsg(Client::REQ_MSG);
	std::string	line;
	std::string pathInfo = requestHeader->urlSuffix->getCgiPathInfo();
	std::string scriptLocation = getScriptLocation(args[1]);

	// AUTH_TYPE -> should be empty because we don't support authentification
	line = "AUTH_TYPE=";											
	envVec.push_back(line);

	// CONTENT_LENGTH
	line = "CONTENT_LENGTH=";											
	if (requestMessage->getChain().begin()->getBodySize() > 0)
	{
		std::stringstream ss;
		ss << requestMessage->getChain().begin()->getBodySize();
		line += ss.str();
	}
	envVec.push_back(line);

	// CONTENT_TYPE 
	line = "CONTENT_TYPE="; 		
	if (requestHeader->getHeaderFieldMap().find("Content-Type") != requestHeader->getHeaderFieldMap().end())
		line += requestHeader->getHeaderFieldMap().find("Content-Type")->second;
	envVec.push_back(line);

	// GATEWAY_INTERFACE
	line = "GATEWAY_INTERFACE=CGI/1.1"; 
	envVec.push_back(line);



	// QUERY_STRING
	line = "QUERY_STRING="; 
	line += requestHeader->urlSuffix->getQueryParameters();
	envVec.push_back(line);

	// REMOTE_ADDR
	line = "REMOTE_ADDR="; 
	line += client.getClientIp();
	envVec.push_back(line);

	// REMOTE_HOST -> not mandatory according to RFC

	// REMOTE_IDENT -> not mandatory according to RFC

	// REMOTE_USER -> we don't support it (only makes sense if authentification was required)
	
	//REQUEST_METHOD
	line = "REQUEST_METHOD="; 
	line += requestHeader->getRequestLine().requestMethod;
	envVec.push_back(line);

	//SCRIPT_NAME
	// should be "cgi-bin/hello.py"
	std::string scriptName = static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader())->urlSuffix->getCgiScriptName();
	line = "SCRIPT_NAME="; 
	line += "/" + scriptLocation.substr(scriptLocation.find("cgi-bin"));
	line += "/" + scriptName;
	envVec.push_back(line);
	
	//SERVER_NAME
	line = "SERVER_NAME="; 
	line += requestHeader->getHostName();
	envVec.push_back(line);
	
	//SERVER_PORT
	std::stringstream ss2;
	line = "SERVER_PORT="; 
	ss2 << requestHeader->getHostPort();
	line += ss2.str();
	envVec.push_back(line);
	
	//SERVER_PROTOCOL 
	line = "SERVER_PROTOCOL="; 
	line += requestHeader->getRequestLine().protocolVersion;
	envVec.push_back(line);
	
	//SERVER_SOFTWARE
	line = "SERVER_SOFTWARE=webserv/2.0 ("; 
	line += operatingSystem();
	line += ")"; 
	envVec.push_back(line);

	line = "HTTP_USER_AGENT="; 		
	if (requestHeader->getHeaderFieldMap().find("User-Agent") != requestHeader->getHeaderFieldMap().end())
		line += requestHeader->getHeaderFieldMap().find("User-Agent")->second;
	envVec.push_back(line);

	line = "HTTP_ACCEPT="; 		
	if (requestHeader->getHeaderFieldMap().find("Accept") != requestHeader->getHeaderFieldMap().end())
		line += requestHeader->getHeaderFieldMap().find("Accept")->second;
	envVec.push_back(line);


	// root of document
	line = "DOCUMENT_ROOT=";

	std::string root = Data::findStringInEnvp("PWD=") + "/";
	line += root;
	const VirtualServer& server = *client.getVirtualServer();
	const std::string& clientUriPath = requestHeader->urlSuffix->getPath();
	const std::string& serverLocationUri = server.getLocationURIfromPath(clientUriPath);
	bool found = false;
	std::vector<LocationSettings>::const_iterator it = server.fetchLocationWithUri(serverLocationUri, found);
	std::string locationRoot;
	if(found == true)
		locationRoot = it->getRoot();
	line += locationRoot;
	envVec.push_back(line);

	// PATH_INFO 
	// Because you won’t call the CGI directly, use the full path as PATH_INFO.
	// I DON'T UNDERSTAND the subject here
	// PATH_INFO usually is the part in the url that comes after
	// the executable name and before the query string e.g.:
	// localhost:9090/cgi-bin/hello.py/[PATH_INFO_stuff]?name=user
	std::string absolutePathInfo = "";
	if (!pathInfo.empty())
	{
		absolutePathInfo = root;
		absolutePathInfo += locationRoot;
		absolutePathInfo += pathInfo;
	}
	line = "PATH_INFO="; 
	envVec.push_back(line + absolutePathInfo);

	// PATH INFO ACCORDING TO THE RFC
	line = "RFC_PATH_INFO="; 
	line += pathInfo;
	envVec.push_back(line);

	// PATH_TRANSLATED 
	line = "PATH_TRANSLATED="; 
	envVec.push_back(line + absolutePathInfo);

}

char**	Cgi::_vecToChararr(std::vector<std::string> list)
{
	int i = 0;
	std::vector<std::string>::iterator it = list.begin();
	_tmp = new char*[list.size() + 1]();

	for (; it != list.end(); it++)
	{
		char* line = new char[it->size() + 1]();
		for (size_t j = 0; j < it->size(); j++)
			line[j] = it->c_str()[j];
		_tmp[i] = line;
		_tmp[i][it->size()] = '\0';
		i++;
	}
	_tmp[i] = NULL;
	char** result = _tmp;
	_tmp = NULL;
	return (result);
}

void	Cgi::_deleteChararr(char** lines)
{
	if (!lines)
		return ;
	size_t i = 0;
	for(; lines[i]; i++)
		delete [] lines[i];
	delete [] lines[i];
	delete [] lines;
}

static void	fackinPanic()
{
	Logger::info("Panic! ", true);
	// std::map<int, Client*>::iterator it = Client::clients.begin();
	// for (; it != Client::clients.end(); ++it)
	// {
	// 	// it->second->setClientState(Client::DELETEME);
	// 	delete it->second;
	// }
	while (Client::clients.size() > 0)
		delete Client::clients.begin()->second;
	// exit(1);
}

int	Cgi::_execute(char** args, char** env, int* socketsToChild, int* socketsFromChild)
{
	signal(SIGINT, SIG_IGN);
	close (ConnectionManager::getEpollFd());
	close(socketsToChild[0]);
	close(socketsFromChild[0]);
	ConnectionManager::closeClientFds();
	Socket::closeSockets();
	// if (1)
	if (chdir(getScriptLocation(args[1]).c_str()) == -1)
	{
		close(socketsFromChild[1]);
		close(socketsToChild[1]);
		// fackinPanic();
		// client.setCgiFlag(false);
		throw std::runtime_error("chdir failed in CGI child process");

		// return (1);
		// execve("",args + 2,env);
	}
 	if (dup2(socketsToChild[1], STDIN_FILENO) == -1)
	{
		close(socketsFromChild[1]);
		close(socketsToChild[1]);
		throw std::runtime_error("dup2 failed in CGI child process");
	}
	close(socketsToChild[1]);
 	if (dup2(socketsFromChild[1], STDOUT_FILENO) == -1)
	{
		close(socketsFromChild[1]);
		throw std::runtime_error("dup2 failed in CGI child process");
	}
	close(socketsFromChild[1]);

 	// execve(_args[0], _args, _env);
 	execve(args[0], args, env);
	throw std::runtime_error("execve failed in CGI child process");
}

void	Cgi::_handleReturnStatus(int status, Client& client)
{
		if (WIFSIGNALED(status))
		{
			Logger::warning("child exited due to SIGNAL: ", WTERMSIG(status));
			Logger::warning("Child ID: ", client.getId());
			_stopCgiSetErrorCode(client);
		}
		if (WIFEXITED(status))
		{
			int exitstatus = WEXITSTATUS(status);
			Logger::warning("child exited with code: ", exitstatus);
			Logger::warning("Child ID: ", client.getId());

			if (client.getClientState() == Client::CRITICAL_ERROR)
			{
				client.setClientState(Client::DELETEME);
				client.setCgiFlag(false);
				return ;
			}
			if (exitstatus != 0)
				_stopCgiSetErrorCode(client, 502);
		}
}

void	Cgi::_waitForChild(Client& client)
{
	int		status;
	
	// return if waitpid was already successful once
	if (client.getWaitReturn() > 0)
		return ;

	// WAITPID
	client.setWaitReturn(waitpid(client.getChildPid(), &status, WNOHANG));

	// waitpid fail
	if (client.getWaitReturn() == -1)
	{
		Logger::error("waitpid, stopping CGI in client with ID", client.getId());
		_stopCgiSetErrorCode(client);
		return ;
	}

	// waitpid success
	if (client.getWaitReturn() > 0)	
		_handleReturnStatus(status, client);
	return ;
}

void	Cgi::_stopCgiSetErrorCode(Client& client, int code)
{
	Logger::warning("stopping CGI loop for Client with ID: ", client.getId());

	client.setClientState(Client::DO_RESPONSE);
	if (client.getErrorCode() == 0)
	{
		if (client.checkTimeout() == false)
			client.setErrorCode(504);
		else
			client.setErrorCode(code);
	}
	if (client.getClientFds().size() > 1)
	{
		client.getMsg(Client::RESP_MSG)->setState(COMPLETE);
		client.getFdDataByType(FdData::FROMCHILD_FD).state = FdData::CLOSE;
	}
	else
		client.setCgiFlag(false);
}

bool	Cgi::_createSockets(int* socketsToChild, int* socketsFromChild)
{
	// (void)socketsFromChild;
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, socketsToChild) < 0)
		return (false);
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, socketsFromChild) < 0)
	{
		closeSockets(socketsToChild, NULL);
		return (false);
	}
	return (true);
}

void	Cgi::_prepareSockets(Client& client, int* socketsToChild, int* socketsFromChild)
{
	close(socketsToChild[1]);
	close(socketsFromChild[1]);
	client.setChildSocket(socketsToChild[0], socketsFromChild[0]);
}

char**	Cgi::_argumentList(Client& client)
{	
	char** toReturn = NULL;
	std::vector<std::string>	argsVec;
	if (_initArgsList(client, argsVec) == false)
		return (NULL);
	if (!_checkScriptAbsPath(argsVec))
		return (NULL);
	try
	{
		toReturn = _vecToChararr(argsVec);
	}
	catch(std::bad_alloc& a)
	{
		_stopCgiSetErrorCode(client);
		Logger::error("F@ck ", a.what());
		toReturn = NULL;
	}
	return (toReturn);
}

char**	Cgi::_metaVariables(Client& client, char** args)
{
	char** toReturn = NULL;
	std::vector<std::string>	envVec;
	_createEnvVector(client, envVec, args);
	try
	{
		toReturn = _vecToChararr(envVec);
	}
	catch(std::bad_alloc& a)
	{
		_stopCgiSetErrorCode(client);
		Logger::error("F@ck", "");
		toReturn = NULL;
	}
	return (toReturn);
}

static bool	isAllowedCgi(Client& client)
{
	if (client.getCgiFlag() == false)
		return (false);

	// CHILD PROCESS HAS FINISHED RUNNING -> NO NEED TO RUN CGI LOOP
	if (client.getWaitReturn() != 0)
		return (false);

	return (true);
}

void Cgi::_cgiClient(Client& client)
{
	if (isAllowedCgi(client) == false)
		return ;
	// CREATE CHILD SOCKETS AND FORK ONCE
	char** args = NULL;
	char** env = NULL;
	if (client.getClientFds().size() == 1)
	{
		int	socketsToChild[2];
		int	socketsFromChild[2];
		args = _argumentList(client);
		if (!args)
		{
			_stopCgiSetErrorCode(client);
			_deleteChararr(args);
			_deleteChararr(_tmp);
			return ;
		}
		env = _metaVariables(client, args); 
		if (!env)
		{
			_stopCgiSetErrorCode(client);
			_deleteChararr(args);
			_deleteChararr(env);
			_deleteChararr(_tmp);
			return ;
		}
		// CREATES TWO SOCKETPAIRS IN ORDER TO COMMUNICATE WITH CHILD PROCESS
		if(!_createSockets(socketsToChild, socketsFromChild))
		{
			_deleteChararr(args);
			_deleteChararr(env);
			_stopCgiSetErrorCode(client);
			return ;
		}
		// FORK!!
		client.setChildPid(fork());
		if (client.getChildPid() == -1)
		{
			closeSockets(socketsToChild, socketsFromChild);
			_deleteChararr(args);
			_deleteChararr(env);
			_stopCgiSetErrorCode(client);
			return ;
		}
		// RUN CHILD PROCESS
		if (client.getChildPid() == CHILD)
		{
			signal(SIGINT, SIG_IGN);
			try
			{
				_execute(args, env, socketsToChild, socketsFromChild);
			}
			catch(std::exception &e)
			{
				fackinPanic();
				_deleteChararr(args);
				_deleteChararr(env);
				throw (e);
				// client.setCgiFlag(false);
				// _stopCgiSetErrorCode(client);
			}
		}
		// FREE ALLOCATED CHAR ARRAYS
		_deleteChararr(args);
		_deleteChararr(env);
		//CLOSING UNUSED SOCKETS AND ADDING RELEVANT SOCKETS TO EPOLL
		_prepareSockets(client, socketsToChild, socketsFromChild);
	}
	// RUN WAIT FOR CHILD
	_waitForChild(client);
	return ;
}

void Cgi::loop()
{
	std::map<int, Client*>::iterator it = Client::clients.begin();
	for (; it != Client::clients.end(); ++it)
		_cgiClient(*(it->second));
}

//==========================================================================//
// Constructor, Destructor and OCF Parts ===================================//
//==========================================================================//

Cgi::Cgi ()
{
	_tmp = NULL;
	// Logger::info("Cgi constructor called", "");
}

Cgi::~Cgi (void)
{
	_deleteChararr(_tmp);
	// Logger::info("Cgi destructor called", "");
}

Cgi::Cgi(Cgi const & src)
{
	//std::cout << "Cgi copy constructor called" << std::endl;
	*this = src;
}

Cgi &	Cgi::operator=(Cgi const & rhs)
{
	//std::cout << "Cgi Copy assignment operator called" << std::endl;
	if (this != &rhs)
	{
	}
	return (*this);
}

//==========================================================================//
// NOTES====================================================================//
//==========================================================================//

// Because you won’t call the CGI directly, use the full path as PATH_INFO.
// if no header or incomplete header, create header
// finish alls meta-variables
