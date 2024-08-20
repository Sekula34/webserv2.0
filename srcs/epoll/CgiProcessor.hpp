
#ifndef CGIPROCESSOR_HPP
# define CGIPROCESSOR_HPP
# include "Client.hpp"

# include <iostream>
# include <fcntl.h>
# include <unistd.h>
# include <sys/wait.h>
# include <string.h>
# include <vector>
#include <stdio.h>

# define CHILD 0

class Client;

class CgiProcessor {

	public:
						// canonical
						CgiProcessor(Client* client);
						~CgiProcessor(void);

						// set and get
		// std::string		getCgiResponse() const;
		// void			setCgiOutput(std::string  s);

						//CgiProcessor specific functions
		int				process(void);

	private:
		Client *		_client;
		int				_pid;
		// int				_pipefd[2]; // 0 read, 1 write
		int				_sockets[2];
		std::vector<std::string>	_args_vec;
		std::vector<std::string>	_env_vec;
		char**			_args;
		char**			_env;
		char**			_tmp;
		bool			_forked;
		int				_exitstatus;

		char**			create_env();
		char**			create_argv();
		int				execute();
		int				gen_body();
		int				read_from_child();
		char**			vec_to_chararr(std::vector<std::string>);
		void			delete_chararr(char ** lines);
		void			create_env_vector();
		void			create_args_vector();
		pid_t			wait_for_child();

						CgiProcessor(void);
						CgiProcessor(CgiProcessor const & src);
		CgiProcessor &	operator=(CgiProcessor const & rhs);
};

#endif

