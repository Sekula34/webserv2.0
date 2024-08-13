
#ifndef CGIPROCESSOR_HPP
# define CGIPROCESSOR_HPP
# include "Client.hpp"

# include <iostream>
# include <fcntl.h>
# include <unistd.h>
# include <sys/wait.h>
# include <string.h>

# define CHILD 0

class Client;

class CgiProcessor {

	public:
						// canonical
						CgiProcessor(Client* client);
						~CgiProcessor(void);

						// set and get

						//CgiProcessor specific functions
		int				process(void);

	private:
		Client *		_client;
		int				_pid;
		int				_pipefd[2]; // 0 read, 1 write
		bool			_forked;

		char**			create_env();
		char**			create_argv();
		int				execute();
		int				gen_body();
		int				wait_for_child();

						CgiProcessor(void);
						CgiProcessor(CgiProcessor const & src);
		CgiProcessor &	operator=(CgiProcessor const & rhs);
};

#endif

