
#include "CgiProcessor.hpp"

/******************************************************************************/
/*                               Constructors                                 */
/******************************************************************************/

CgiProcessor::CgiProcessor (void)
{
	// std::cout << "CgiProcessor default constructor called" << std::endl;
}

CgiProcessor::CgiProcessor (Client* c): _client(c)
{
	// std::cout << "CgiProcessor default constructor called" << std::endl;
}

/******************************************************************************/
/*                                Destructor                                  */
/******************************************************************************/

CgiProcessor::~CgiProcessor (void)
{
	std::cout << "CgiProcessor destructor called" << std::endl;
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


// char**	CgiProcessor::create_env()
// {

// }

// char**	CgiProcessor::create_argv()
// {

// }

// int	CgiProcessor::execute()
// {
// 	int exepipefd[2];
// 	char** env = create_env();
// 	char** argv = create_env();

// 	if (pipe(exepipefd) == -1)
// 		return (_client->setErrorCode(500), 1);

// 	write(exepipefd[1], _client->getBody().c_str(), _client->getBody().size());

// 	close(exepipefd[1]);
// 	close(_pipefd[0]);

// 	if (dup2(exepipefd[0], STDIN_FILENO) == -1)
// 		return (_client->setErrorCode(500), 1);
// 	close(exepipefd[0]);
// 	if (dup2(_pipefd[1], STDOUT_FILENO) == -1)
// 		return (_client->setErrorCode(500), 1);
// 	close(_pipefd[1]);
// 	execve("script.py", argv, env);
// 	return 0;
// }

// int	CgiProcessor::wait_for_child()
// {
// 	int					status;
// 	int					waitreturn;

// 	close (_pipefd[1]);

// 	std::cout << "waiting for child ..." << std::endl;
// 	waitreturn = waitpid(_pid, &status, WNOHANG);
// 	if (waitreturn == -1)
// 		return (_client->setErrorCode(500), 1);
// 	if (waitreturn != 0)
// 			return (0);
// 	return (1);
// }

// int	CgiProcessor::gen_body()
// {
// 	std::string			line;
// 	unsigned char		buffer[MAXLINE];

// 	std::cout << "child has exited" << std::endl;
// 	memset(buffer, 0, MAXLINE);

// 	int readsize = read(_pipefd[0], buffer, MAXLINE - 1);
// 	if (readsize == -1)
// 		return (_client->setErrorCode(500), 1);

// 	std::cout << "string from child process: " << buffer << std::endl;
// 	close (_pipefd[0]);
// 	return (1);
// }

// // CHECKER FOR PYTHON
// 	// what ending is this? eg "script.py" -> language = python 
// 	// do we support this language?
// 	// is the interpreter for this language installed on this system?
// 	// in this case argv = {"pthon3", "script.py"}

// int CgiProcessor::process()
// {
// 	if (pipe(_pipefd) == -1)
// 		return (_client->setErrorCode(500), 1);

// 	if (!_forked)
// 	{
// 		_forked = true;
// 		_pid = fork();

// 		if (_pid == CHILD)
// 		{
// 			if(execute() == 1)
// 				return (_client->setErrorCode(500), 1);
// 		}
// 	}

// 	if (_pid != CHILD)
// 	{
// 		if (wait_for_child() == 0)
// 			gen_body();
// 	}
// 	return (0);
// }
