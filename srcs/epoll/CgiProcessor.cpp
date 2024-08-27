
#include "../Server/SocketManager.hpp"
#include "../Server/Socket.hpp"
#include "../Utils/Data.hpp"
#include "../Parsing/ParsingUtils.hpp"
#include "CgiProcessor.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

/******************************************************************************/
/*                               Constructors                                 */
/******************************************************************************/

CgiProcessor::CgiProcessor (void):
_allSockets(Data::getServerSockets()),
_nfds(Data::getNfds())
{}

CgiProcessor::CgiProcessor (Client& c):
_allSockets(Data::getServerSockets()),
_nfds(Data::getNfds())
{
	_client = &c;
	_args = NULL;
	_env = NULL;
	_tmp = NULL;
	_forked = false;
	_initScriptVars();
	_createEnvVector();
	_createArgsVector();
	_env = _vecToChararr(_envVec);
	_args = _vecToChararr(_argsVec);
	_killedChild = false;
	Logger::info("CgiProcessor constructor called"); std::cout << std::endl;
}

/******************************************************************************/
/*                                Destructor                                  */
/******************************************************************************/


CgiProcessor::~CgiProcessor (void)
{
	Logger::info("CgiProcessor destructor called"); std::cout << std::endl;
	_deleteChararr(_tmp);
	_deleteChararr(_args);
	_deleteChararr(_env);
}

/******************************************************************************/
/*                             Copy Constructor                               */
/******************************************************************************/

CgiProcessor::CgiProcessor(CgiProcessor const & src):
_allSockets(Data::getServerSockets()),
_nfds(Data::getNfds())
{
	//std::cout << "CgiProcessor copy constructor called" << std::endl;
	*this = src;
}

/******************************************************************************/
/*                      Copy Assignment Operator Overload                     */
/******************************************************************************/

CgiProcessor &	CgiProcessor::operator=(CgiProcessor const & rhs)
{
	//std::cout << "CgiProcessor Copy assignment operator called" << std::endl;
	if (this != &rhs)
	{
	}
	return (*this);
}

int	CgiProcessor::getPid()
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
// Just remember that, for chunked request, your server needs to unchunk it
// parse header
// if no header or incomplete header, create header
// If no content_length is returned from CGI, EOF will mark the end of the returned data.
// check return of write to child,
// use send instead of write to child?
// call write to child multiple times untill whole message has been written
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

std::string	CgiProcessor::getInterpreterPath(std::string suffix)
{
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
	Logger::warning("CGI interpreter not installed:");
	std::cout << Data::getCgiLang().at(suffix) << std::endl;
	return (tmp = "");
}

std::string	CgiProcessor::getScriptName(std::string suffix)
{
	std::vector<std::string> sections = ParsingUtils::splitString(_client->header->urlSuffix->getPath(), '/');
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

bool	CgiProcessor::isRegularFile(std::string file)
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

void	CgiProcessor::_initScriptVars()
{
	// the suffix should come from url parser
	std::string suffix = ".py";

	// this should happen when parsing the config file
	Data::setCgiLang(suffix, "python3");


	std::string location = "/cgi-bin/";

	// go through PATH variable and check whether interpreter is installed
	// if not installed stops Cgi sets 500 error
	_interpreterAbsPath = getInterpreterPath(suffix);
	if (_interpreterAbsPath.empty())
		return ;

	_scriptName = getScriptName(suffix);
	if (_scriptName.empty())
		return ;

	_scriptAbsPath = Data::findStringInEnvp("PWD=") + location
		+ Data::getCgiLang().at(suffix) + "/" + _scriptName;

	_scriptLocation = Data::findStringInEnvp("PWD=") + location
		+ Data::getCgiLang().at(suffix);

	std::cout << _scriptAbsPath << std::endl;
	if (access(_scriptAbsPath.c_str(), X_OK) != 0)	
	{
		Logger::warning("CGI script not executable: ");
		std::cout << _scriptAbsPath << std::endl;
		_stopCgiSetErrorCode();
		return ;
	}
	if (!isRegularFile(_scriptAbsPath))
		_stopCgiSetErrorCode();
}

void	CgiProcessor::_createEnvVector()
{
	std::string	line;

	// AUTH_TYPE -> should be empty because we don't support authentification
	line = "AUTH_TYPE=";											
	_envVec.push_back(line);

	// CONTENT_LENGTH
	line = "CONTENT_LENGTH=";											
	if (_client->getClientBody().size() > 0)
	{
		std::stringstream ss;
		ss << _client->getClientBody().size();
		line += ss.str();
	}
	_envVec.push_back(line);

	// CONTENT_TYPE 
	line = "CONTENT_TYPE="; 		
	if (_client->header->getHeaderFields().find("Content-Type") != _client->header->getHeaderFields().end())
		line +=_client->header->getHeaderFields().find("Content-Type")->second;
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
	line += _scriptAbsPath;
	_envVec.push_back(line);

	// PATH_TRANSLATED 
	// this should include PATH_INFO (the way RFC defines PATH_INFO)
	// actual PATH_INFO part is missing!!
	line = "PATH_TRANSLATED="; 
	line += _scriptAbsPath;
	_envVec.push_back(line);

	// QUERY_STRING
	line = "QUERY_STRING="; 
	line += _client->header->urlSuffix->getQueryParameters();
	_envVec.push_back(line);

	// REMOTE_ADDR
	line = "REMOTE_ADDR="; 
	line += _client->getClientIp();
	_envVec.push_back(line);

	// REMOTE_HOST -> not mandatory according to RFC

	// REMOTE_IDENT -> not mandatory according to RFC

	// REMOTE_USER -> we don't support it (only makes sense if authentification was required)
	
	//REQUEST_METHOD
	line = "REQUEST_METHOD="; 
	line += _client->header->getRequestLine().requestMethod;
	_envVec.push_back(line);

	//SCRIPT_NAME
	// should be "cgi-bin/hello.py"
	line = "SCRIPT_NAME="; 
	line += _client->header->urlSuffix->getPath();
	_envVec.push_back(line);
	
	//SERVER_NAME
	line = "SERVER_NAME="; 
	line += _client->header->getHostName();
	_envVec.push_back(line);
	
	//SERVER_PORT
	std::stringstream ss2;
	line = "SERVER_PORT="; 
	ss2 << _client->header->getHostPort();
	line += ss2.str();
	_envVec.push_back(line);
	
	//SERVER_PROTOCOL 
	line = "SERVER_PROTOCOL="; 
	line += _client->header->getRequestLine().protocolVersion;
	_envVec.push_back(line);
	
	//SERVER_SOFTWARE
	line = "SERVER_SOFTWARE=webserv2.0 ("; 
	line += operatingSystem();
	line += ")"; 
	_envVec.push_back(line);
}

void	CgiProcessor::_createArgsVector()
{
	_argsVec.push_back(_interpreterAbsPath);
	_argsVec.push_back(_scriptAbsPath);
}

char**	CgiProcessor::_vecToChararr(std::vector<std::string> list)
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
void	CgiProcessor::_deleteChararr(char ** lines)
{
	if (!lines)
		return ;
	size_t i = 0;
	for(; lines[i]; i++)
		delete [] lines[i];
	delete [] lines[i];
	delete [] lines;
}

int	CgiProcessor::_execute()
{
	close(_socketsToChild[0]);
	close(_socketsFromChild[0]);
	Data::closeAllFds();
	if (chdir(_scriptLocation.c_str()) == -1)
	{
		close(_socketsFromChild[1]);
		close(_socketsToChild[1]);
		throw std::runtime_error("chdir failed in CGI child process");
	// exit (1); -> which one is correct?
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
	// what to do here? throw exception?
	throw std::runtime_error("execve failed in CGI child process");
 	// exit (1);
}

bool	CgiProcessor::_isSocketReady(int socket, int macro)
{
	for (size_t i = 0; i < static_cast<size_t>(_nfds); ++i)
	{
		if (Data::setEvents()[i].data.fd == socket && Data::setEvents()[i].events & macro)
			return (true);
	}
	return (false);
}

void	CgiProcessor::_writeToChild()
{
	if (_client->hasWrittenToCgi || !_isSocketReady(_client->socketToChild, EPOLLOUT))
		return ;
	// std::cout << "writing to child" << std::endl;
	write(_client->socketToChild, "check this out\n", 15);
	_client->hasWrittenToCgi = true;
	Logger::warning("removing FD from epoll: ");
	std::cout << "FD: " << _socketsToChild[0] << " Id: " << _client->getId() << std::endl;
	Data::epollRemoveFd(_client->socketToChild);
	close(_client->socketToChild);
	_client->socketToChild = DELETED;
}

void	CgiProcessor::_readFromChild()
{
	int n = 0;

	if (!_client->hasReadFromCgi && _isSocketReady(_client->socketFromChild, EPOLLIN))
	{
		_client->clearRecvLine();
		n = recv(_client->socketFromChild, _client->getRecvLine(), MAXLINE - 1, MSG_DONTWAIT);
		// std::cout << "bytes read from child socket: " << n << std::endl;

		// successful read, concat message
		if (n > 0)
			_client->addRecvLineToCgiMessage();

		// invalid read, stop CGI and set errorcode = 500
		if (n < 0)
		{
			std::cout << "setting cgiRunning to false in readFromChild because -1 from read" << std::endl;
			_stopCgiSetErrorCode();
		}

		// EOF reached, child has gracefully shutdown connection
		if (n == 0)
			_client->hasReadFromCgi = true;
	}
}

void	CgiProcessor::_ioChild()
{
	_writeToChild();
	if (!_client->hasWrittenToCgi)
		return ;

	// running waitpid to know when childprocess has finished and quit if it crashes
	_waitForChild();
	if (!_client->cgiRunning)
		return ;

	_readFromChild();
	if (_client->hasReadFromCgi && _client->waitReturn > 0)
	{
		Logger::warning("removing FD from epoll: ");
		std::cout << "FD: " << _socketsFromChild[0] << " Id: " << _client->getId() << std::endl;
		Data::epollRemoveFd(_client->socketFromChild);
		close(_client->socketFromChild);
		_client->socketFromChild = DELETED;
		_client->_cgiOutput = _client->getCgiMessage();
		std::cout << "setting cgiRunning to false after reading from child finished" << std::endl;
		_client->cgiRunning = false;
	}
	return ;
}

void	CgiProcessor::_waitForChild()
{
	int		status;
	
	if (_client->waitReturn > 0)
		return ;
	// std::cout << "### client with id: " << _client->getId() << "is in ioChild" << std::endl;
	if (!_client->checkTimeout() && !_killedChild)
	{
		_killedChild = true;
		Logger::warning("calling SIGKILL on child process: ");
		std::cout << _pid << std::endl;
		// kill(_pid, SIGKILL);
		kill(_pid, SIGINT);
	}
	_client->waitReturn = waitpid(_pid, &status, WNOHANG);
	if (_client->waitReturn == -1)
	{
		std::cout << "waitpid error, stopping CGI" << std::endl;
		_stopCgiSetErrorCode();
		return ;
	}
	if (_client->waitReturn > 0)	
	{
		if (WIFSIGNALED(status))
		{
			Logger::warning("child exited due to SIGNAL: ");
			std::cout << WTERMSIG(status);
			std::cout << ", ID: " << _client->getId() <<  std::endl;
			// _childExited = true;
		}
		if (WIFEXITED(status))
		{
			Logger::warning("child exited with code: ");
			std::cout << WEXITSTATUS(status);
			std::cout << ", ID: " << _client->getId() <<  std::endl;
			_exitstatus = WEXITSTATUS(status);
			// _childExited = true;
		}
	}
	return ;
}

bool	CgiProcessor::_createSockets()
{
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, _socketsToChild) < 0)
		return (false);
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, _socketsFromChild) < 0)
		return (false);
	return (true);
}

void	CgiProcessor::_stopCgiSetErrorCode()
{
	Logger::error("stopping CGI, errorcode 500 ");
	std::cout << ",id: " << _client->getId() << std::endl;
	_client->setErrorCode(500);
	_client->cgiRunning = false;
}

void	CgiProcessor::_prepareSockets()
{
	close(_socketsToChild[1]);
	close(_socketsFromChild[1]);
	Logger::warning("adding FD to epoll: ");
	std::cout << "FD: " << _socketsToChild[0] << " Id: " << _client->getId() << std::endl;
	Data::epollAddFd(_socketsToChild[0]);
	Logger::warning("adding FD to epoll: ");
	std::cout << "FD: " << _socketsFromChild[0] << " Id: " << _client->getId() << std::endl;
	Data::epollAddFd(_socketsFromChild[0]);
	_client->setChildSocket(_socketsToChild[0], _socketsFromChild[0]);
}

int CgiProcessor::process()
{
	if (!_forked)
	{
		_forked = true;

		// creates two socketpairs in order to communicate with child process
		if(!_createSockets())
			return (_stopCgiSetErrorCode(), 1);
		if ((_pid = fork()) == -1)
			return (_stopCgiSetErrorCode(), 1);
		if (_pid == CHILD)
		{
			// runs execve on cgi-script
			_execute();
			return (_stopCgiSetErrorCode(), 1);
		}

		//closing unused sockets and adding relevant sockets to epoll
		_prepareSockets();
	}
	_ioChild();
	return (0);
}
