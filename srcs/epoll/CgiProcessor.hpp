
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
		std::string					getInterpreterPath(std::string suffix);
		std::string					getScriptName(std::string suffix);
		int							getPid();
		void						killChild();
		bool						killedChild;

	private:
		Client *					_client;
		int							_pid;
		int							_socketsToChild[2];
		int							_socketsFromChild[2];
		std::vector<std::string>	_argsVec;
		std::vector<std::string>	_envVec;
		char**						_args;
		char**						_env;
		char**						_tmp;
		bool						_forked;
		int							_exitstatus;
		const std::vector<Socket>&	_allSockets;
		const int &					_nfds;	
		std::string					_interpreterAbsPath;
		std::string					_scriptAbsPath;
		std::string					_scriptName;
		std::string					_scriptLocation;
		bool						_kill;
		std::clock_t				_shutdownStart;

		void						_ioChild();
		void						_initScriptVars();
		bool						_checkInterpreterScript();
		void						_readFromChild();
		void						_writeToChild();
		void						_stopCgiSetErrorCode();
		bool						_createSockets();
		void						_prepareSockets();
		bool						_isSocketReady(int socket, int macro);
		char**						_create_env();
		char**						_create_argv();
		int							_execute();
		char**						_vecToChararr(std::vector<std::string>);
		void						_deleteChararr(char ** lines);
		void						_createEnvVector();
		void						_createArgsVector();
		void						_waitForChild();
		bool						_isRegularFile(std::string file);
		void						_timeoutKillChild();

									CgiProcessor(void);
									CgiProcessor(CgiProcessor const & src);
		CgiProcessor &				operator=(CgiProcessor const & rhs);
};

#endif

