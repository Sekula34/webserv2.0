# include <iostream>
# include <fcntl.h>
# include <unistd.h>
# include <sys/wait.h>
# include <string.h>

# define CHILD 0
# define MAXLINE 4096


void	cgi(unsigned char* buffer)
{
	// create new environment list out of header -> key:value pairs
	// redirect body to STDIN
	// execute script with environment list
	// pipe return of executable back to parent process;
}

void	execute_cgi()
{
	char env[12][10];


}

int main(int argc, char* argv[], char **env)
{

	int pid;
	int pipefd[2]; // 0 read, 1 write
	
	(void)argc;
	if (pipe(pipefd) == -1)
		return (1);

	pid = fork();
	if (pid == CHILD)
	{
		int exepipefd[2];

		if (pipe(exepipefd) == -1)
			return (1);
		write(exepipefd[1], "testi", 5);

		close(exepipefd[1]);
		close(pipefd[0]);

		if (dup2(exepipefd[0], STDIN_FILENO) == -1)
			return (1);
		close(exepipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) == -1)
			return (1);
		close(pipefd[1]);
		sleep(3);
		execve("script.py", argv, env);
	}

	if (pid != CHILD)
	{
		int					status;
		int					waitreturn;
		std::string			line;
		unsigned char		buffer[MAXLINE];

		close (pipefd[1]);

		std::cout << "waiting for child ..." << std::endl;
		while (true)
		{
			sleep(1);
			std::cout << "waiting" << std::endl;
			waitreturn = waitpid(pid, &status, WNOHANG);
			// waitreturn = 1;
			if (waitreturn == -1)
				return (1);
			if (waitreturn != 0)
					break ;
		}

		std::cout << "child has exited" << std::endl;
		memset(buffer, 0, MAXLINE);

		int readsize = read(pipefd[0], buffer, MAXLINE - 1);
		if (readsize == -1)
			return (1);

		std::cout << "string from child process: " << buffer << std::endl;
		close (pipefd[0]);
	}
}
