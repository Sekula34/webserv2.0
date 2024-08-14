
#include "CgiProcessor.hpp"

/******************************************************************************/
/*                               Constructors                                 */
/******************************************************************************/

CgiProcessor::CgiProcessor (void)
{
	// std::cout << "CgiProcessor default constructor called" << std::endl;
}

CgiProcessor::CgiProcessor (Client* c): _client(c), _args(NULL), _env(NULL),
	_tmp(NULL), _forked(false)
{

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

CgiProcessor::CgiProcessor(CgiProcessor const & src)
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

std::string		CgiProcessor::getCgiResponse() const
{
	return (_cgi_output);
}

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

// create new environment list out of header -> key:value pairs
// find out what part of envp should be sent to execve
// create args checker -> check whether interpreter exists and binary exists
// create timeout checker so that child process is terminated after certain time
// Because you won’t call the CGI directly, use the full path as PATH_INFO.
// Just remember that, for chunked request, your server needs to unchunk
//		it, the CGI will expect EOF as end of the body.
// Same things for the output of the CGI. If no content_length is returned
//		from the CGI, EOF will mark the end of the returned data.
// Your program should call the CGI with the file requested as first argument.
// The CGI should be run in the correct directory for relative path file access.
//


void	CgiProcessor::create_env_vector()
{
	std::string	line;

	//  don't understand what this is
	line = "AUTH_TYPE=Basic ";											
	_env_vec.push_back(line);

	// will need to make this depend on content length of body when available
	line = "CONTENT_LENGTH=0";											
	_env_vec.push_back(line);

	// we need to set this if POST type request
	// line = "CONTENT_TYPE=application/x-www-form-urlencoded"; 		
	// _env_vec.push_back(line);
	
	line = "GATEWAY_INTERFACE=CGI/1.1"; 
	_env_vec.push_back(line);

	// Because you won’t call the CGI directly, use the full path as PATH_INFO.
	line = "PATH_INFO="; 
	line += _client->header->getRequestedUrl();
	_env_vec.push_back(line);

	// The CGI should be run in the correct directory for relative path file access.
	// line = "PATH_TRANSLATED="; 
	// line += "/var/www/html/extra/path/info ";
	// _env_vec.push_back(line);
	
	// everything after the '?' in URI
	std::string url = _client->header->getRequestedUrl();
	size_t found  = url.find('?');
	if (found != std::string::npos && found != url.size() - 1)
	{
		line = "QUERY_STRING="; 
		line += url.substr(found + 1, url.size());
		_env_vec.push_back(line);
	}

	// using forbidden function here -> inet-ntoa
	line = "REMOTE_ADDR="; 
	line += _client->getClientIp();
	_env_vec.push_back(line);

	
/*	get info from getHeaderFields
	std::map<std::string, std::string>::const_iterator it = _client->header->getHeaderFields().begin();
	for (; it != _client->header->getHeaderFields().end(); it++)
	{
		line.clear();
		line += it->first;
		line += "=";
		line += it->second;
		std::cout << line << std::endl;
		_env_vec.push_back(line);
	}
*/
}

void	CgiProcessor::create_args_vector()
{
	// this is hardcoded now until parsing for CGI works
	
	// _args_vec.push_back("/home/gdanis/.brew/bin/python3");
	// _args_vec.push_back("/home/gdanis/webserv/srcs/epoll/hello.py");
	_args_vec.push_back("/usr/bin/python3");
	_args_vec.push_back("/home/gabor/webserv/srcs/epoll/hello.py");

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
	int exepipefd[2];

	std::cout << "entered execute..." << std::endl;


	if (pipe(exepipefd) == -1)
 		return (_client->setErrorCode(500), 1);

 	write(exepipefd[1], _client->getClientBody().c_str(), _client->getClientBody().size());
 	write(exepipefd[1], "check this out\n", 15);

 	close(exepipefd[1]);
 	close(_pipefd[0]);

 	if (dup2(exepipefd[0], STDIN_FILENO) == -1)
 		return (_client->setErrorCode(500), 1);
 	close(exepipefd[0]);
 	if (dup2(_pipefd[1], STDOUT_FILENO) == -1)
 		return (_client->setErrorCode(500), 1);
 	close(_pipefd[1]);
 	int ret = execve(_args[0], _args, _env);
	std::cerr << "######### EXECVE FAILED" << std::endl;
 	return (_client->setErrorCode(500), ret);
}

int	CgiProcessor::wait_for_child()
{
	int					status;
	pid_t					waitreturn;

	close (_pipefd[1]);

	waitreturn = waitpid(_pid, &status, WNOHANG);
	if (WIFEXITED(status))
		_exitstatus = WEXITSTATUS(status);
	if (waitreturn == -1)
		return (_client->setErrorCode(500), 1);
	return (waitreturn);
}

int	CgiProcessor::gen_body()
{
	char		buffer[MAXLINE];
	int			readsize;

	while (true)
	{
		memset(buffer, 0, MAXLINE);
		readsize = read(_pipefd[0], buffer, MAXLINE - 1);
		if (readsize == -1)
			return (_client->setErrorCode(500), 1);
		_cgi_output += buffer;
		if (readsize < MAXLINE - 1)
			break;
	}
	close (_pipefd[0]);
	return (1);
}

// CHECKER FOR PYTHON
	// what ending is this? eg "script.py" -> language = python 
	// do we support this language?
	// is the interpreter for this language installed on this system?
	// in this case argv = {"pthon3", "script.py"}

int CgiProcessor::process()
{
	if (!_forked)
	{
		_forked = true;
		if (pipe(_pipefd) == -1)
			return (_client->setErrorCode(500), 1);
		_pid = fork();

		if (_pid == CHILD)
		{
			if(execute() == -1)
				return (_client->setErrorCode(500), 1);
		}
	}

	if (_pid != CHILD)
	{
		if (wait_for_child() > 0)
			gen_body();
		else 
			return (1);
	}
	return (0);
}
