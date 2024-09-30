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
									Cgi();
									~Cgi();
		void						loop();

	private:
		// bool						_terminate;
		// bool						_sentSigkill;
		// std::clock_t				_shutdownStart;
		char**						_tmp;

		void						_ioChild();
		void 						_cgiClient(Client& client);
		bool						_initArgsList(Client& c, std::vector<std::string>& _argsVec);
		bool						_checkInterpreterScript();
		bool						_checkScriptAbsPath(Client& client, std::vector<std::string>& argsVec);
		void						_stopCgiSetErrorCode(Client& c);
		bool						_createSockets(int* socketsToChild, int* socketsFromChild);
		void						_prepareSockets(Client& client, int* socketsToChild, int* socketsFromChild);
		char**						_create_env();
		char**						_create_argv();
		int							_execute(char** args, char** env, int* socketsToChild, int* socketsFromChild);
		char**						_vecToChararr(std::vector<std::string>);
		void						_deleteChararr(char ** lines);
		void						_createEnvVector(Client& client, std::vector<std::string>& envVec, char **args);
		void						_waitForChild(Client& client);
		void						_handleChildTimeout(Client& client);
		void						_timeoutKillChild();
		bool						_isRegularFile(std::string file);
		void						_handleReturnStatus(int status, Client& client);
		std::string					_getInterpreterPath(Client& client, std::string suffix);
		std::string					_getScriptName(std::string suffix, Client& c);
		char**						_argumentList(Client& client);
		char**						_metaVariables(Client& client, char** args);
		void						_terminateChild();

									Cgi(Cgi const & src);
		Cgi &						operator=(Cgi const & rhs);
};

#endif

