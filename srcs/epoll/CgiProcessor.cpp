
#include "../Server/SocketManager.hpp"
#include "../Server/Socket.hpp"
#include "../Utils/Data.hpp"
#include "CgiProcessor.hpp"
#include <unistd.h>

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
	_childExited = false;
	create_env_vector();
	create_args_vector();
	_env = vec_to_chararr(_env_vec);
	_args = vec_to_chararr(_args_vec);
	Logger::info("CgiProcessor constructor called"); std::cout << std::endl;
}

/******************************************************************************/
/*                                Destructor                                  */
/******************************************************************************/

CgiProcessor::~CgiProcessor (void)
{
	Logger::info("CgiProcessor destructor called"); std::cout << std::endl;
	delete_chararr(_tmp);
	delete_chararr(_args);
	delete_chararr(_env);
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

// create args checker -> check whether interpreter exists and binary exists
// create timeout checker so that child process is terminated after certain time
// Because you won’t call the CGI directly, use the full path as PATH_INFO.
// Just remember that, for chunked request, your server needs to unchunk
//		it, the CGI will expect EOF as end of the body.
// Same things for the output of the CGI. If no content_length is returned
//		from the CGI, EOF will mark the end of the returned data.
// The CGI should be run in the correct directory for relative path file access.

std::string operating_system()
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

void	CgiProcessor::create_env_vector()
{
	std::string	line;

	// AUTH_TYPE -> should be empty because we don't support authentification
	line = "AUTH_TYPE=";											
	_env_vec.push_back(line);


	// CONTENT_LENGTH
	line = "CONTENT_LENGTH=";											
	if (_client->getClientBody().size() > 0)
	{
		std::stringstream ss;
		ss << _client->getClientBody().size();
		line += ss.str();
	}
	_env_vec.push_back(line);


	// CONTENT_TYPE 
	line = "CONTENT_TYPE="; 		
	if (_client->header->getHeaderFields().find("Content-Type") != _client->header->getHeaderFields().end())
		line +=_client->header->getHeaderFields().find("Content-Type")->second;
	_env_vec.push_back(line);

	

	// GATEWAY_INTERFACE
	line = "GATEWAY_INTERFACE=CGI/1.1"; 
	_env_vec.push_back(line);


	// PATH_INFO 
	// Because you won’t call the CGI directly, use the full path as PATH_INFO.
	// I DON'T UNDERSTAND the subject here
	// PATH_INFO usually is the part in the url that comes after
	// the executable name and before the query string e.g.:
	// localhost:9090/cgi-bin/hello.py/[PATH_INFO_stuff]?name=user
	line = "PATH_INFO="; 
	line += _client->header->urlSuffix->getPath();
	_env_vec.push_back(line);

	

	// PATH_TRANSLATED 
	// The CGI should be run in the correct directory for relative path file access.
	char buf[256];
	getcwd(buf, sizeof(buf));
	line = "PATH_TRANSLATED="; 
	line += buf;
	_env_vec.push_back(line);
	

	// QUERY_STRING
	line = "QUERY_STRING="; 
	line += _client->header->urlSuffix->getQueryParameters();
	_env_vec.push_back(line);

	// REMOTE_ADDR
	line = "REMOTE_ADDR="; 
	line += _client->getClientIp();
	_env_vec.push_back(line);

	// REMOTE_HOST -> not mandatory according to RFC

	// REMOTE_IDENT -> not mandatory according to RFC

	// REMOTE_USER -> we don't support it (only makes sense if authentification was required)
	
	//REQUEST_METHOD
	line = "REQUEST_METHOD="; 
	line += _client->header->getRequestLine().requestMethod;
	_env_vec.push_back(line);


	//SCRIPT_NAME
	// should be "cgi-bin/hello.py"
	line = "SCRIPT_NAME"; 
	line += _client->header->urlSuffix->getPath();
	_env_vec.push_back(line);
	
	//SERVER_NAME
	line = "SERVER_NAME="; 
	line += _client->header->getHostName();
	_env_vec.push_back(line);
	
	//SERVER_PORT
	std::stringstream ss2;
	line = "SERVER_PORT="; 
	ss2 << _client->header->getHostPort();
	line += ss2.str();
	_env_vec.push_back(line);
	
	//SERVER_PROTOCOL 
	line = "SERVER_PROTOCOL="; 
	line += _client->header->getRequestLine().protocolVersion;
	_env_vec.push_back(line);
	
	//SERVER_SOFTWARE
	line = "SERVER_SOFTWARE=webserv2.0 ("; 
	line += operating_system();
	line += ")"; 
	_env_vec.push_back(line);
}

void	CgiProcessor::create_args_vector()
{
	// this is hardcoded now until parsing for CGI works
	char buf[256];
	getcwd(buf, sizeof(buf));
	std::string s = buf;
	// s += "/srcs/epoll/hello.py";
	s += "/srcs/epoll/hello.py";
	_args_vec.push_back("/usr/bin/python3");
	_args_vec.push_back(s);
}

char**	CgiProcessor::vec_to_chararr(std::vector<std::string> list)
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
void	CgiProcessor::delete_chararr(char ** lines)
{
	if (!lines)
		return ;
	size_t i = 0;
	for(; lines[i]; i++)
		delete [] lines[i];
	delete [] lines[i];
	delete [] lines;
}

int	CgiProcessor::execute()
{
	close(_sockets_tochild[0]);
	close(_sockets_fromchild[0]);

	// closes epollFd
	// closes all ClientFds 
	// closes sockets to child processes in all other Clients
	// closes Server Sockets
	Data::closeAllFds();

 	if (dup2(_sockets_tochild[1], STDIN_FILENO) == -1)
	{
		close(_sockets_fromchild[1]);
		close(_sockets_tochild[1]);
 		return (_stopCgiSetErrorCode(), 1);
	}
	close(_sockets_tochild[1]);
 	if (dup2(_sockets_fromchild[1], STDOUT_FILENO) == -1)
	{
		close(_sockets_fromchild[1]);
 		return (_stopCgiSetErrorCode(), 1);
	}
	close(_sockets_fromchild[1]);
 	int ret = execve(_args[0], _args, _env);
 	return (_stopCgiSetErrorCode(), ret);
}

bool	CgiProcessor::isSocketReady(int socket, int macro)
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
	if (_client->hasWrittenToCgi || !isSocketReady(_client->socketToChild, EPOLLOUT))
		return ;
	std::cout << "writing to child" << std::endl;
	write(_client->socketToChild, "check this out\n", 15);
	_client->hasWrittenToCgi = true;
	Data::epollRemoveFd(_client->socketToChild);
	close(_client->socketToChild);
}

void	CgiProcessor::_readFromChild()
{
	int n = 0;

	if (!_client->hasReadFromCgi && isSocketReady(_client->socketFromChild, EPOLLIN))
	{
		_client->clearRecvLine();
		n = recv(_client->socketFromChild, _client->getRecvLine(), MAXLINE - 1, MSG_DONTWAIT);
		std::cout << "bytes read from child socket: " << n << std::endl;

		// successful read, concat message
		if (n > 0)
			_client->addRecvLineToCgiMessage();

		// invalid read, stop CGI and set errorcode = 500
		if (n < 0)
			_stopCgiSetErrorCode();

		// EOF reached, child has gracefully shutdown connection
		if (n == 0)
			_client->hasReadFromCgi = true;
	}
}

void	CgiProcessor::ioChild()
{
	_writeToChild();
	if (!_client->hasWrittenToCgi)
		return ;

	// running waitpid to know when childprocess has finished and quit if it crashes
	_wait_for_child();
	if (!_client->cgiRunning)
		return ;

	_readFromChild();
	if (_client->hasReadFromCgi)
	{
		Data::epollRemoveFd(_client->socketFromChild);
		close(_client->socketFromChild);
		_client->_cgiOutput = _client->getCgiMessage();
		_client->cgiRunning = false;
	}
	return ;
}

void	CgiProcessor::_wait_for_child()
{
	int		status;
	
	if (_childExited)
		return ;
	_client->waitReturn = waitpid(_pid, &status, WNOHANG);
	if (_client->waitReturn == -1)
	{
		_stopCgiSetErrorCode();
		return ;
	}
	if (WIFEXITED(status))
	{
		// std::cout << "child exited" << std::endl;
		_exitstatus = WEXITSTATUS(status);
		_childExited = true;
	}
	return ;
}

bool	CgiProcessor::_createSockets()
{
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, _sockets_tochild) < 0)
		return (false);
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, _sockets_fromchild) < 0)
		return (false);
	return (true);
}

void	CgiProcessor::_stopCgiSetErrorCode()
{
	_client->setErrorCode(500);
	_client->cgiRunning = false;
}

void	CgiProcessor::_prepareSockets()
{
	close(_sockets_tochild[1]);
	close(_sockets_fromchild[1]);
	Data::epollAddFd(_sockets_tochild[0]);
	Data::epollAddFd(_sockets_fromchild[0]);
	_client->setChildSocket(_sockets_tochild[0], _sockets_fromchild[0]);
}

int CgiProcessor::process()
{
	if (_client->waitReturn > 0)
		return (0);
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
			execute();
			return (_stopCgiSetErrorCode(), 1);
		}

		//closing unused sockets and adding relevant sockets to epoll
		_prepareSockets();
	}
	ioChild();
	return (0);
}
