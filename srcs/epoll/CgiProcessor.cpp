
#include "../Server/SocketManager.hpp"
#include "../Server/Socket.hpp"
#include "../Utils/Data.hpp"
#include "CgiProcessor.hpp"

/******************************************************************************/
/*                               Constructors                                 */
/******************************************************************************/

CgiProcessor::CgiProcessor (void):
_allSockets(Data::getServerSockets())
{
	// std::cout << "CgiProcessor default constructor called" << std::endl;
}

CgiProcessor::CgiProcessor (Client* c):
_allSockets(Data::getServerSockets())
{
	_client = c;
	_args = NULL;
	_env = NULL;
	_tmp = NULL;
	_forked = false;
	create_env_vector();
	create_args_vector();
	_env = vec_to_chararr(_env_vec);
	_args = vec_to_chararr(_args_vec);
	// std::cout << "CgiProcessor default constructor called" << std::endl;
}

/******************************************************************************/
/*                                Destructor                                  */
/******************************************************************************/

CgiProcessor::~CgiProcessor (void)
{
	std::cout << "CgiProcessor destructor called" << std::endl;
	delete_chararr(_tmp);
	delete_chararr(_args);
	delete_chararr(_env);
}

/******************************************************************************/
/*                             Copy Constructor                               */
/******************************************************************************/

CgiProcessor::CgiProcessor(CgiProcessor const & src):
_allSockets(Data::getServerSockets())
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

/******************************************************************************/
/*                          Setters and Getters                               */
/******************************************************************************/

// std::string		CgiProcessor::getCgiResponse() const
// {
// 	return (_cgi_output);
// }
//
// void	CgiProcessor::setCgiOutput(std::string s)
// {
// 	_cgi_output = s;
// }

/******************************************************************************/
/*                               Error Handling                               */
/******************************************************************************/

/******************************************************************************/
/*                            O-Stream Overload                               */
/******************************************************************************/

/******************************************************************************/
/*                          Class Specific Functions                          */
/******************************************************************************/

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
//
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
	
	// _args_vec.push_back("/home/gdanis/.brew/bin/python3");
	_args_vec.push_back("/usr/bin/python3");
	_args_vec.push_back("/home/gabor/webserv/srcs/epoll/hello.py");
	// _args_vec.push_back("/home/gabor/webserv/srcs/epoll/hello.py");

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

 	if (dup2(_sockets_tochild[1], STDIN_FILENO) == -1)
 		return (_client->setErrorCode(500), 1);
	close(_sockets_tochild[1]);
 	if (dup2(_sockets_fromchild[1], STDOUT_FILENO) == -1)
 		return (_client->setErrorCode(500), 1);
	close(_sockets_fromchild[1]);
	close(Data::getEpollFd());
	close(_client->getFd());
	for(size_t i = 0; i < _allSockets.size(); i++)
		close(Data::getServerSocketFds()[i]);
 	int ret = execve(_args[0], _args, _env);
 	return (_client->setErrorCode(500), ret);
}

void	CgiProcessor::ioChild()
{
	int n = 0;
	Client* client = findSocketClient(socket);
	if (!client)
		return (std::cout << "no client for this socket, FATAL ERROR!", false);
	if (!client->hasWrittenToCgi && Data::setEvents()[idx].events & EPOLLOUT && client->socket_tochild == socket)
	{
 		write(socket, "check this out\n", 15);
		client->hasWrittenToCgi = true;
		client->unsetsocket_tochild();
		return (true);
	}
	if (socket == client->socket_tochild)
		return (true);

		wait_for_child();
	if (!client->hasReadFromCgi)
	{
		if (!read_fd(socket, client, n, idx))
			return (true);
		std::cout << "bytes read from child socket: " << n << std::endl;
		if (n > 0)
			client->addRecvLineToCgiMessage();
		if (n < 0)
			return (std::cout << "error: received, in handleChildSocket n: " << n << std::endl, true);
		if (n < MAXLINE - 1)
			client->hasReadFromCgi = true;
	}
	if (client->waitreturn)
	{
		client->unsetsocket_fromchild();
		client->_cgi_output = client->getCgiMessage();
	}
	return (true);
}

pid_t	CgiProcessor::wait_for_child()
{
	int		status;

	_client->waitreturn = waitpid(_pid, &status, WNOHANG);
	if (_client->waitreturn == -1)
		return (_client->setErrorCode(500), 1);
	if (WIFEXITED(status))
	{
		std::cout << "child exited" << std::endl;
		_exitstatus = WEXITSTATUS(status);
	}
	return (_client->waitreturn);
}

// create new function write_to_child
 	// write(_sockets[0], _client->getClientBody().c_str(), _client->getClientBody().size());
 	// write(_sockets[0], "check this out\n", 15);


// CHECKER FOR PYTHON
	// what ending is this? eg "script.py" -> language = python 
	// do we support this language?
	// is the interpreter for this language installed on this system?
	// in this case argv = {"pthon3", "script.py"}




// RETURN = TRUE -> return in handle client because CGI is still runnig
// RETURN = FALSE ->  contiunue in handle client
int CgiProcessor::process()
{
	if (_client->waitreturn > 0)
		return (_client->waitreturn);
	if (!_forked)
	{
		_forked = true;
		if (socketpair(AF_UNIX, SOCK_STREAM, 0, _sockets_tochild) < 0)
			return (_client->setErrorCode(500), 1);
		if (socketpair(AF_UNIX, SOCK_STREAM, 0, _sockets_fromchild) < 0)
			return (_client->setErrorCode(500), 1);

		if ((_pid = fork()) == -1)
			return (_client->setErrorCode(500), 1);
		if (_pid == CHILD)
		{
			if(execute() == -1)
				return (_client->setErrorCode(500), 1);
		}
		close(_sockets_tochild[1]);
		close(_sockets_fromchild[1]);
		_client->setChildSocket(_sockets_tochild[0], _sockets_fromchild[0]);
	}
	if (_pid != CHILD)
	{
		ioChild();
		return (_client->waitreturn);
	}
	return (0);
}
