
#include "../Server/SocketManager.hpp"
#include "../Server/Socket.hpp"
#include "../Utils/Data.hpp"
#include "../Parsing/ParsingUtils.hpp"
#include "CgiProcessor.hpp"
#include "Message.hpp"
#include "Node.hpp"
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
	sentSigterm = false;
	_terminate = false;
	_shutdownStart = 0;
	_sentSigkill = false;
	_bytesSent = 0;
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
	Logger::warning("CGI interpreter not installed:");
	std::cout << Data::getCgiLang().at(suffix) << std::endl;
	return (tmp = "");
}

std::string	CgiProcessor::getScriptName(std::string suffix)
{
	std::vector<std::string> sections = ParsingUtils::splitString((static_cast<RequestHeader*>(_client->getClientMsg()->getHeader()))->urlSuffix->getPath(), '/');
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

bool	CgiProcessor::_isRegularFile(std::string file)
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
	std::string suffix = static_cast<RequestHeader*>(_client->getClientMsg()->getHeader())->urlSuffix->getCgiScriptExtension();

	// this should happen when parsing the config file
	// Data::setCgiLang(suffix, "python3");

	std::string location = "/cgi-bin/";

	// go through PATH variable and check whether interpreter is installed
	// if not installed stops Cgi sets 500 error
	_interpreterAbsPath = getInterpreterPath(suffix);
	if (_interpreterAbsPath.empty())
	{
		_stopCgiSetErrorCode();
		return ;
	}

	_scriptName = static_cast<RequestHeader*>(_client->getClientMsg()->getHeader())->urlSuffix->getCgiScriptName();
	// _scriptName = getScriptName(suffix);
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
	if (!_isRegularFile(_scriptAbsPath))
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
	if (_client->getClientMsg()->getChain().begin()->getBodySize() > 0)
	{
		std::stringstream ss;
		ss << _client->getClientMsg()->getChain().begin()->getBodySize();
		line += ss.str();
	}
	_envVec.push_back(line);

	// CONTENT_TYPE 
	line = "CONTENT_TYPE="; 		
	if (_client->getClientMsg()->getHeader()->getHeaderFieldMap().find("Content-Type") != _client->getClientMsg()->getHeader()->getHeaderFieldMap().end())
		line +=_client->getClientMsg()->getHeader()->getHeaderFieldMap().find("Content-Type")->second;
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
	line += (static_cast<RequestHeader*>(_client->getClientMsg()->getHeader()))->urlSuffix->getQueryParameters();
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
	line += (static_cast<RequestHeader*>(_client->getClientMsg()->getHeader()))->getRequestLine().requestMethod;
	_envVec.push_back(line);

	//SCRIPT_NAME
	// should be "cgi-bin/hello.py"
	line = "SCRIPT_NAME="; 
	line += (static_cast<RequestHeader*>(_client->getClientMsg()->getHeader()))->urlSuffix->getPath();
	_envVec.push_back(line);
	
	//SERVER_NAME
	line = "SERVER_NAME="; 
	line += (static_cast<RequestHeader*>(_client->getClientMsg()->getHeader()))->getHostName();
	_envVec.push_back(line);
	
	//SERVER_PORT
	std::stringstream ss2;
	line = "SERVER_PORT="; 
	ss2 << (static_cast<RequestHeader*>(_client->getClientMsg()->getHeader()))->getHostPort();
	line += ss2.str();
	_envVec.push_back(line);
	
	//SERVER_PROTOCOL 
	line = "SERVER_PROTOCOL="; 
	line += (static_cast<RequestHeader*>(_client->getClientMsg()->getHeader()))->getRequestLine().protocolVersion;
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
	signal(SIGINT, SIG_IGN);
	close(_socketsToChild[0]);
	close(_socketsFromChild[0]);
	Data::closeAllFds();
	std::cout << "starting child script" << std::endl;
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
	throw std::runtime_error("execve failed in CGI child process");
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
	int		writeValue = 0;

	// return if epoll does not allow us to write to socket
	if (_client->hasWrittenToCgi || !_isSocketReady(_client->socketToChild, EPOLLOUT))
		return ;

	// SEND
	const std::string& bodystr = _client->getClientMsg()->getBodyString();

	if (bodystr.size() > 0)
		writeValue = send(_client->socketToChild, bodystr.c_str() + _bytesSent, bodystr.size() - _bytesSent, MSG_DONTWAIT | MSG_NOSIGNAL);
	// std::cout << "bytes written to CGI child process: " << writeValue << std::endl;
	_bytesSent += writeValue;

	// return if full message could not be sent
	if (_bytesSent < bodystr.size() && writeValue > 0)
		return ;

	// if unable to send full message, log error and set error Code
	if (writeValue < 0 || (writeValue == 0 && _bytesSent < bodystr.size()))
	{
		Logger::error("failed to send Request Body to CGI", true);
		_client->setErrorCode(500);
	}

	// finished writing
	_client->hasWrittenToCgi = true;

	// closing Socket and and removing it from epoll
	Logger::warning("removing FD from epoll: ");
	std::cout << "FD: " << _socketsToChild[0] << " Id: " << _client->getId() << std::endl;
	Data::epollRemoveFd(_client->socketToChild);
	close(_client->socketToChild);
	_client->socketToChild = DELETED;
}

void	CgiProcessor::_readFromChild()
{
	int readValue = 0;

	if (!_client->hasReadFromCgi && _isSocketReady(_client->socketFromChild, EPOLLIN))
	{

		if (!_client->getServerMsg())
			_client->setServerMsg(new Message(false));

		_client->clearRecvLine();
		readValue = recv(_client->socketFromChild, _client->getRecvLine(), MAXLINE, MSG_DONTWAIT | MSG_NOSIGNAL);
		std::cout << "bytes read from child socket: " << readValue << std::endl;
		std::cout << "buffer: ";
		Logger::chars(_client->getRecvLine(), true);

		// successful read -> concat message
		if (readValue > 0)
			_client->getServerMsg()->bufferToNodes(_client->getRecvLine(), readValue);

		// failed read -> stop CGI and set errorcode = 500
		if (readValue < 0)
		{
			Logger::warning("failed tor read from Child Process", true);
			_stopCgiSetErrorCode();
		}

		// EOF reached, child has gracefully shutdown connection
		if (readValue == 0 && _client->waitReturn != 0)
		{
			_client->getServerMsg()->printChain();
			if (!_client->getServerMsg()->getHeader())
				_client->getServerMsg()->_createHeader();
			_client->getServerMsg()->setState(COMPLETE);
			_client->hasReadFromCgi = true;
			// copy the error code in the CgiResponseHeader into client
			// so that errors from CGI can be processed
			if (_client->getServerMsg() && _client->getServerMsg()->getHeader() && !_client->getErrorCode())
				_client->setErrorCode(_client->getServerMsg()->getHeader()->getHttpStatusCode());
		}
	}
}

void	CgiProcessor::_closeCgi()
{
	if (_client->hasReadFromCgi && _client->waitReturn > 0)
	{
		Logger::warning("removing FD from epoll: ");
		std::cout << "FD: " << _socketsFromChild[0] << " Id: " << _client->getId() << std::endl;
		Data::epollRemoveFd(_client->socketFromChild);
		close(_client->socketFromChild);
		_client->socketFromChild = DELETED;
		// _client->_cgiOutput = _client->getCgiMessage();
		_client->cgiRunning = false;
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

	_closeCgi();
	return ;
}

void	CgiProcessor::terminateChild()
{
	_terminate = true;	
}

void	CgiProcessor::_timeoutKillChild()
{
	if (_sentSigkill)
		return ;
	double diff = (static_cast<double>(std::clock() - _shutdownStart) * 1000) / CLOCKS_PER_SEC;
	if (_shutdownStart && diff > MAX_TIMEOUT / 2) 
	{	
		Logger::error("killing child process with PID: ");
		std::cout << _pid << std::endl;
		kill(_pid, SIGKILL);
		_sentSigkill = true;
	}
}

void	CgiProcessor::_handleChildTimeout()
{
	// send SIGTERM to child on Timeout OR on Shutdown of Server due to pressing CTRL+C
	if ((!_client->checkTimeout() && !sentSigterm) || _terminate)
	{
		_shutdownStart = std::clock();
		_terminate = false;
		sentSigterm = true;
		Logger::warning("calling SIGTERM on child process: ");
		std::cout << _pid << std::endl;
		kill(_pid, SIGTERM);
	}
	// if SIGTERM not successful send SIGKILL after MAX_TIMEOUT / 2
	_timeoutKillChild();
}

void	CgiProcessor::_handleReturnStatus(int status)
{
		if (WIFSIGNALED(status))
		{
			Logger::warning("child exited due to SIGNAL: ");
			std::cout << WTERMSIG(status);
			std::cout << ", ID: " << _client->getId() <<  std::endl;
			_stopCgiSetErrorCode();
		}
		if (WIFEXITED(status))
		{
			_exitstatus = WEXITSTATUS(status);
			Logger::warning("child exited with code: ");
			std::cout << _exitstatus;
			std::cout << ", ID: " << _client->getId() <<  std::endl;
			if (_exitstatus != 0)
				_stopCgiSetErrorCode();
		}
}

void	CgiProcessor::_waitForChild()
{
	int		status;
	
	// return if waitpid was already successful once
	if (_client->waitReturn > 0)
		return ;

	// send SIGINT or even SIGKILL to child on timeout or CTRL+C
	_handleChildTimeout();

	// WAITPID
	_client->waitReturn = waitpid(_pid, &status, WNOHANG);

	// waitpid fail
	if (_client->waitReturn == -1)
	{
		Logger::error("waitpid, stopping CGI", true);
		_stopCgiSetErrorCode();
		return ;
	}

	// waitpid success
	if (_client->waitReturn > 0)	
		_handleReturnStatus(status);
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
	delete _client->getServerMsg();
	_client->setServerMsg(NULL);
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
		// FORK!!
		if ((_pid = fork()) == -1)
			return (_stopCgiSetErrorCode(), 1);
		if (_pid == CHILD)
		{
			signal(SIGINT, SIG_IGN);
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
