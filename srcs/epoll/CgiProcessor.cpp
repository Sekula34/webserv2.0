
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





	// create new environment list out of header -> key:value pairs
	// redirect body to STDIN
	// execute script with environment list
	// pipe return of executable back to parent process;

void	CgiProcessor::create_env_vector()
{
	// this will need to be modified and meta-variables added
	for (size_t i = 0; _client->getEnvp()[i]; i++)
		_env_vec.push_back(_client->getEnvp()[i]);
}

void	CgiProcessor::create_args_vector()
{
	// this is hardcoded now until parsing for CGI works
	_args_vec.push_back("/home/gdanis/.brew/bin/python3");
	_args_vec.push_back("/home/gdanis/webserv/srcs/epoll/hello.py");

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

	std::cout << "entered execute" << std::endl;

	if (pipe(exepipefd) == -1)
 		return (_client->setErrorCode(500), 1);

 	write(exepipefd[1], _client->getClientBody().c_str(), _client->getClientBody().size());

 	close(exepipefd[1]);
 	close(_pipefd[0]);

	std::cout << "after write in child process" << std::endl;
 	if (dup2(exepipefd[0], STDIN_FILENO) == -1)
 		return (_client->setErrorCode(500), 1);
 	close(exepipefd[0]);
 	if (dup2(_pipefd[1], STDOUT_FILENO) == -1)
 		return (_client->setErrorCode(500), 1);
 	close(_pipefd[1]);
	std::cerr << "##### about to execute" << std::endl;
	std::cerr << "args 0: " << _args[0] << std::endl;
	std::cerr << "args 1: " << _args[1] << std::endl;
 	int ret = execve(_args[0], _args, NULL);
	std::cerr << "######### EXECVE FAILED" << std::endl;
	perror("execve failed");
 	return (_client->setErrorCode(500), ret);
}

int	CgiProcessor::wait_for_child()
{
	int					status;
	pid_t					waitreturn;

	close (_pipefd[1]);

	std::cout << "waiting for child ..." << std::endl;
	waitreturn = waitpid(_pid, &status, WNOHANG);
	if (WIFEXITED(status))
	{
		std::cout << "exitstatus of child: " << WEXITSTATUS(status) << std::endl;
		
	}
	std::cout << "waiting waitreturn: " << waitreturn <<  std::endl;
	sleep (1);
	std::cout << "got past sleep" << std::endl;
	if (waitreturn == -1)
		return (_client->setErrorCode(500), 1);
	if (waitreturn != 0)
			return (0);
	return (1);
}

int	CgiProcessor::gen_body()
{
	std::string	line;
	char		buffer[MAXLINE];
	int			readsize;

	std::cout << "child has exited" << std::endl;
	while (true)
	{
		memset(buffer, 0, MAXLINE);
		readsize = read(_pipefd[0], buffer, MAXLINE - 1);
		if (readsize == -1)
			return (_client->setErrorCode(500), 1);
		line += buffer;
		if (readsize < MAXLINE - 1)
			break;
	}

	std::cout << "string from child process: " << line << std::endl;
	_cgi_output = line;
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
		std::cout << "forked" << std::endl;

		if (_pid == CHILD)
		{
			if(execute() == -1)
				return (_client->setErrorCode(500), 1);
		}
	}

	if (_pid != CHILD)
	{
		if (wait_for_child() == 0)
			gen_body();
		else 
			return (1);
	}
	return (0);
}
