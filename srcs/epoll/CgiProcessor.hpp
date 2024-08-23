
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
class Socket;

class CgiProcessor {

	public:
									CgiProcessor(Client& client);
									~CgiProcessor(void);
		int							process(void);
		void						ioChild();

	private:
		Client *					_client;
		int							_pid;
		int							_sockets_tochild[2];
		int							_sockets_fromchild[2];
		std::vector<std::string>	_args_vec;
		std::vector<std::string>	_env_vec;
		char**						_args;
		char**						_env;
		char**						_tmp;
		bool						_forked;
		int							_exitstatus;
		const std::vector<Socket>&	_allSockets;
		const int &					_nfds;	
		bool						_exited;

		void						_readFromChild();
		void						_writeToChild();
		void						_stopCgiSetErrorCode();
		bool						_createSockets();
		void						_prepareSockets();
		bool						isSocketReady(int socket, int macro);
		char**						create_env();
		char**						create_argv();
		int							execute();
		int							gen_body();
		int							read_from_child();
		char**						vec_to_chararr(std::vector<std::string>);
		void						delete_chararr(char ** lines);
		void						create_env_vector();
		void						create_args_vector();
		void						_wait_for_child();

									CgiProcessor(void);
									CgiProcessor(CgiProcessor const & src);
		CgiProcessor &				operator=(CgiProcessor const & rhs);
};

#endif

