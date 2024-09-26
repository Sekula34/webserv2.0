#include "../Server/Socket.hpp"
#include "../Utils/Data.hpp"
#include "../Utils/Logger.hpp"
#include "../Parsing/ParsingUtils.hpp"
#include "../Message/Message.hpp"
#include "../Message/Node.hpp"
#include "../Message/RequestHeader.hpp"
#include "Cgi.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

/******************************************************************************/
/*                               Constructors                                 */
/******************************************************************************/

Cgi::Cgi (void)
{}

Cgi::Cgi (Client& client)
{
	_args = NULL;
	_env = NULL;
	_tmp = NULL;
	_initScriptVars(client);
	_createEnvVector(client);
	_createArgsVector();
	_env = _vecToChararr(_envVec);
	_args = _vecToChararr(_argsVec);
	sentSigterm = false;
	_terminate = false;
	_shutdownStart = 0;
	_sentSigkill = false;
	Logger::info("Cgi constructor called", "");
}

/******************************************************************************/
/*                                Destructor                                  */
/******************************************************************************/


Cgi::~Cgi (void)
{
	Logger::info("Cgi destructor called", "");
	_deleteChararr(_tmp);
	_deleteChararr(_args);
	_deleteChararr(_env);
}

/******************************************************************************/
/*                             Copy Constructor                               */
/******************************************************************************/

Cgi::Cgi(Cgi const & src)
{
	//std::cout << "Cgi copy constructor called" << std::endl;
	*this = src;
}

/******************************************************************************/
/*                      Copy Assignment Operator Overload                     */
/******************************************************************************/

Cgi &	Cgi::operator=(Cgi const & rhs)
{
	//std::cout << "Cgi Copy assignment operator called" << std::endl;
	if (this != &rhs)
	{
	}
	return (*this);
}

int	Cgi::getPid()
{
	return (_pid);
}


/*
 * RFC 3875 - Common Gateway Protocol
 
	* The server receives the request from the client
	
	* selects a CGI script to handle the request
	
	* converts the client request to a CGI request
	
	* executes the script
	
	* converts the CGI response into a response for the client
	
	* When processing the client request, it is responsible for implementing
	any protocol or transport level authentication and security. 

	The server MUST perform translations and protocol conversions on the
	client request data required by this specification.

	* Furthermore, the server retains its responsibility to the client
	to conform to the relevant network protocol even if the CGI script
	fails to conform to this specification.
*/

// Because you won’t call the CGI directly, use the full path as PATH_INFO.
// if no header or incomplete header, create header
// finish alls meta-variables

std::string operatingSystem()
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

std::string	Cgi::getInterpreterPath(std::string suffix)
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
	_stopCgiSetErrorCode();
	Logger::warning("CGI interpreter not installed:", "");
	std::cout << Data::getCgiLang().at(suffix) << std::endl;
	return (tmp = "");
}

std::string	Cgi::getScriptName(std::string suffix, Client& client)
{
	std::vector<std::string> sections = ParsingUtils::splitString((static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader()))->urlSuffix->getPath(), '/');
	std::vector<std::string>::iterator it = sections.begin();
	for (; it != sections.end(); it++)
	{
		if ((*it).find(suffix) != std::string::npos)
			return (*it);
	}
	Logger::warning("no valid CGI script name in URI", true);
	_stopCgiSetErrorCode();
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

void	Cgi::_initScriptVars(Client& client)
{
	// the suffix is .py or .php
	std::string suffix = static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader())->urlSuffix->getCgiScriptExtension();

	// this is the location on the server and is hardcoded
	std::string location = "/cgi-bin/";

	// go through PATH variable and check whether interpreter is installed
	// if not installed stops Cgi sets 500 error
	_interpreterAbsPath = getInterpreterPath(suffix);
	if (_interpreterAbsPath.empty())
	{
		_stopCgiSetErrorCode();
		return ;
	}

	_scriptName = static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader())->urlSuffix->getCgiScriptName();
	if (_scriptName.empty())
		return ;

	_scriptAbsPath = Data::findStringInEnvp("PWD=") + location
		+ Data::getCgiLang().at(suffix) + "/" + _scriptName;

	_scriptLocation = Data::findStringInEnvp("PWD=") + location
		+ Data::getCgiLang().at(suffix);

	// Logger::info(_scriptAbsPath, true);
	if (access(_scriptAbsPath.c_str(), X_OK) != 0)	
	{
		Logger::warning("CGI script not executable: ", _scriptAbsPath);
		_stopCgiSetErrorCode();
		return ;
	}
	if (!_isRegularFile(_scriptAbsPath))
		_stopCgiSetErrorCode();
}

void	Cgi::_createEnvVector(Client& client)
{
	std::string	line;
	std::string pathInfo = (static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader()))->urlSuffix->getCgiPathInfo();

	// AUTH_TYPE -> should be empty because we don't support authentification
	line = "AUTH_TYPE=";											
	_envVec.push_back(line);

	// CONTENT_LENGTH
	line = "CONTENT_LENGTH=";											
	if (client.getMsg(Client::REQ_MSG)->getChain().begin()->getBodySize() > 0)
	{
		std::stringstream ss;
		ss << client.getMsg(Client::REQ_MSG)->getChain().begin()->getBodySize();
		line += ss.str();
	}
	_envVec.push_back(line);

	// CONTENT_TYPE 
	line = "CONTENT_TYPE="; 		
	if (client.getMsg(Client::REQ_MSG)->getHeader()->getHeaderFieldMap().find("Content-Type") != client.getMsg(Client::REQ_MSG)->getHeader()->getHeaderFieldMap().end())
		line +=client.getMsg(Client::REQ_MSG)->getHeader()->getHeaderFieldMap().find("Content-Type")->second;
	_envVec.push_back(line);

	// GATEWAY_INTERFACE
	line = "GATEWAY_INTERFACE=CGI/1.1"; 
	_envVec.push_back(line);

	// PATH_INFO 
	// Because you won’t call the CGI directly, use the full path as PATH_INFO.
	// I DON'T UNDERSTAND the subject here
	// PATH_INFO usually is the part in the url that comes after
	// the executable name and before the query string e.g.:
	// localhost:9090/cgi-bin/hello.py/[PATH_INFO_stuff]?name=user
	// actual PATH_INFO part is missing!!
	line = "PATH_INFO="; 
	line += _scriptLocation;
	line += pathInfo;
	_envVec.push_back(line);

	line = "RFC_PATH_INFO="; 
	line += pathInfo;
	_envVec.push_back(line);

	// PATH_TRANSLATED 
	// this should include PATH_INFO (the way RFC defines PATH_INFO)
	// actual PATH_INFO part is missing!!
	if (!pathInfo.empty())
	{
		line = "PATH_TRANSLATED="; 
		line += _scriptLocation;
		line += pathInfo;
		_envVec.push_back(line);
	}

	// QUERY_STRING
	line = "QUERY_STRING="; 
	line += (static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader()))->urlSuffix->getQueryParameters();
	_envVec.push_back(line);

	// REMOTE_ADDR
	line = "REMOTE_ADDR="; 
	line += client.getClientIp();
	_envVec.push_back(line);

	// REMOTE_HOST -> not mandatory according to RFC

	// REMOTE_IDENT -> not mandatory according to RFC

	// REMOTE_USER -> we don't support it (only makes sense if authentification was required)
	
	//REQUEST_METHOD
	line = "REQUEST_METHOD="; 
	line += (static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader()))->getRequestLine().requestMethod;
	_envVec.push_back(line);

	//SCRIPT_NAME
	// should be "cgi-bin/hello.py"
	line = "SCRIPT_NAME="; 
	line += (static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader()))->urlSuffix->getPath();
	_envVec.push_back(line);
	
	//SERVER_NAME
	line = "SERVER_NAME="; 
	line += (static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader()))->getHostName();
	_envVec.push_back(line);
	
	//SERVER_PORT
	std::stringstream ss2;
	line = "SERVER_PORT="; 
	ss2 << (static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader()))->getHostPort();
	line += ss2.str();
	_envVec.push_back(line);
	
	//SERVER_PROTOCOL 
	line = "SERVER_PROTOCOL="; 
	line += (static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader()))->getRequestLine().protocolVersion;
	_envVec.push_back(line);
	
	//SERVER_SOFTWARE
	line = "SERVER_SOFTWARE=webserv2.0 ("; 
	line += operatingSystem();
	line += ")"; 
	_envVec.push_back(line);

	// root of document
	line = "DOCUMENT_ROOT="; 
	line += _scriptLocation;
	_envVec.push_back(line);
}

void	Cgi::_createArgsVector()
{
	_argsVec.push_back(_interpreterAbsPath);
	_argsVec.push_back(_scriptAbsPath);
}

char**	Cgi::_vecToChararr(std::vector<std::string> list)
{
	int i = 0;
	std::vector<std::string>::iterator it = list.begin();
	char**	_tmp = new char*[list.size() + 1];

	for (; it != list.end(); it++)
	{
		char* line = new char[it->size() + 1];
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
void	Cgi::_deleteChararr(char ** lines)
{
	if (!lines)
		return ;
	size_t i = 0;
	for(; lines[i]; i++)
		delete [] lines[i];
	delete [] lines[i];
	delete [] lines;
}

int	Cgi::_execute()
{
	signal(SIGINT, SIG_IGN);
	close(_socketsToChild[0]);
	close(_socketsFromChild[0]);
	Data::closeAllFds();
	if (chdir(_scriptLocation.c_str()) == -1)
	{
		close(_socketsFromChild[1]);
		close(_socketsToChild[1]);
		throw std::runtime_error("chdir failed in CGI child process");
	}
 	if (dup2(_socketsToChild[1], STDIN_FILENO) == -1)
	{
		close(_socketsFromChild[1]);
		close(_socketsToChild[1]);
		throw std::runtime_error("dup2 failed in CGI child process");
	}
	close(_socketsToChild[1]);
 	if (dup2(_socketsFromChild[1], STDOUT_FILENO) == -1)
	{
		close(_socketsFromChild[1]);
		throw std::runtime_error("dup2 failed in CGI child process");
	}
	close(_socketsFromChild[1]);

 	execve(_args[0], _args, _env);
	// TODO: should this be a runtime_error?
	throw std::runtime_error("execve failed in CGI child process");
}

void	Cgi::terminateChild()
{
	_terminate = true;	
}

void	Cgi::_timeoutKillChild()
{
	if (_sentSigkill)
		return ;
	double diff = (static_cast<double>(std::clock() - _shutdownStart) * 1000) / CLOCKS_PER_SEC;
	if (_shutdownStart && diff > MAX_TIMEOUT / 2) 
	{	
		Logger::error("killing child process with PID: ", _pid);
		kill(_pid, SIGKILL);
		_sentSigkill = true;
	}
}

void	Cgi::_handleChildTimeout(Client& client)
{
	// send SIGTERM to child on Timeout OR on Shutdown of Server due to pressing CTRL+C
	if ((!client.checkTimeout() && !sentSigterm) || _terminate)
	{
		_shutdownStart = std::clock();
		_terminate = false;
		sentSigterm = true;
		Logger::warning("calling SIGTERM on child process: ", _pid);
		kill(_pid, SIGTERM);
	}
	// if SIGTERM not successful send SIGKILL after MAX_TIMEOUT / 2
	_timeoutKillChild();
}

void	Cgi::_handleReturnStatus(int status, Client& client)
{
		if (WIFSIGNALED(status))
		{

			Logger::warning("child exited due to SIGNAL: ", WTERMSIG(status));
			Logger::warning("Child ID:", client.getId());
			_stopCgiSetErrorCode();
		}
		if (WIFEXITED(status))
		{
			_exitstatus = WEXITSTATUS(status);
			Logger::warning("child exited with code: ", _exitstatus);
			Logger::warning("Child ID:", client.getId());
			if (_exitstatus != 0)
				_stopCgiSetErrorCode();
		}
}

void	Cgi::_waitForChild(Client& client)
{
	int		status;
	
	// return if waitpid was already successful once
	if (client.waitReturn > 0)
		return ;

	// send SIGINT or even SIGKILL to child on timeout or CTRL+C
	_handleChildTimeout(client);

	// WAITPID
	client.waitReturn = waitpid(_pid, &status, WNOHANG);

	// waitpid fail
	if (client.waitReturn == -1)
	{
		Logger::error("waitpid, stopping CGI in client with ID", client.getId());
		_stopCgiSetErrorCode(client);
		return ;
	}

	// waitpid success
	if (client.waitReturn > 0)	
		_handleReturnStatus(status, client);
	return ;
}

void	Cgi::_stopCgiSetErrorCode(Client& client)
{
	Logger::error("stopping CGI, errorcode 500 in Client with ID: ", client.getId());
	client.setErrorCode(500);
	client.setClientState(Client::DO_RESPONSE);
}

bool	Cgi::_createSockets()
{
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, _socketsToChild) < 0)
		return (false);
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, _socketsFromChild) < 0)
		return (false);
	return (true);
}

void	Cgi::_prepareSockets(Client& client)
{
	close(_socketsToChild[1]);
	close(_socketsFromChild[1]);
	client.setChildSocket(_socketsToChild[0], _socketsFromChild[0]);
}
int Cgi::loop()
{
	std::map<int, Client*>::iterator it = Client::clients.begin();
	for (; it != Client::clients.end(); ++it)
		_cgiClient(*(it->second));
}

void Cgi::_cgiClient(Client& client)
{
	// RETURN OUT OF THIS FUNCTION IF RUNNING CGI IS NOT NECESSARY
	if (client.getClientState() != Client::DO_CGIREC
		|| client.getClientState() != Client::DO_CGISEND)
	{
		return ;
	}

	// CREATE CHILD SOCKETS AND FORK ONCE
	if (client.getClientFds().size() == 1)
	{
		// creates two socketpairs in order to communicate with child process
		if(!_createSockets())
		{
			_stopCgiSetErrorCode();
			return ;
		}
		// FORK!!
		if ((_pid = fork()) == -1)
		{
			_stopCgiSetErrorCode();
			return ;
		}
		if (_pid == CHILD)
		{
			signal(SIGINT, SIG_IGN);
			_execute();
		}
		//closing unused sockets and adding relevant sockets to epoll
		_prepareSockets(client);
	}

	// RUN WAIT FOR CHILD
	_waitForChild(client);
	return ;
}
