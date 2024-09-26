#ifndef CGIPROCESSOR_HPP
# define CGIPROCESSOR_HPP

# include "../Client/Client.hpp"
# include <iostream>
# include <fcntl.h>
# include <unistd.h>
# include <sys/wait.h>
# include <string.h>
# include <vector>
# include <stdio.h>

# define CHILD 0

class Client;
class Socket;

class Cgi {

	public:
									Cgi(Client& client);
									~Cgi(void);
		int							loop(void);
		std::string					getInterpreterPath(std::string suffix);
		std::string					getScriptName(std::string suffix, Client& c);
		int							getPid();
		void						terminateChild();
		bool						sentSigterm;

	private:
		int							_pid;
		int							_socketsToChild[2];
		int							_socketsFromChild[2];
		std::vector<std::string>	_argsVec;
		std::vector<std::string>	_envVec;
		char**						_args;
		char**						_env;
		char**						_tmp;
		int							_exitstatus;
		std::string					_interpreterAbsPath;
		std::string					_scriptAbsPath;
		std::string					_scriptName;
		std::string					_scriptLocation;
		bool						_terminate;
		bool						_sentSigkill;
		std::clock_t				_shutdownStart;

		void						_ioChild();
		void 						_cgiClient(Client& client);
		void						_initScriptVars(Client& c);
		bool						_checkInterpreterScript();
		void						_stopCgiSetErrorCode(Client& c);
		bool						_createSockets();
		void						_prepareSockets(Client& client);
		char**						_create_env();
		char**						_create_argv();
		int							_execute();
		char**						_vecToChararr(std::vector<std::string>);
		void						_deleteChararr(char ** lines);
		void						_createEnvVector(Client& client);
		void						_createArgsVector();
		void						_waitForChild(Client& client);
		void						_handleChildTimeout(Client& client);
		void						_timeoutKillChild();
		bool						_isRegularFile(std::string file);
		void						_handleReturnStatus(int status, Client& client);

									Cgi(void);
									Cgi(Cgi const & src);
		Cgi &				operator=(Cgi const & rhs);
};

#endif

