#ifndef CGIPROCESSOR_HPP
# define CGIPROCESSOR_HPP

# include "../Client/Client.hpp"
# include <fcntl.h>
# include <unistd.h>
# include <sys/wait.h>
# include <vector>

# define CHILD 0

class Client;
class Socket;

class Cgi {

	public:
		// Methods
		void		loop(); // runs CGI within the main loop

	private:
		// Methods
		void 		_cgiClient(Client& client);
		int			_execute(char** args, char** env, int* socketsToChild, int* socketsFromChild);
		void		_handleReturnStatus(int status, Client& client);
		void		_waitForChild(Client& client);
		void		_stopCgiSetErrorCode(Client& c);
		char**		_argumentList(Client& client);
		char**		_metaVariables(Client& client, char** args);
		char**		_create_env();
		char**		_create_argv();
		char**		_vecToChararr(std::vector<std::string>);
		void		_deleteChararr(char ** lines);
		void		_createEnvVector(Client& client, std::vector<std::string>& envVec, char **args);
		bool		_initArgsList(Client& c, std::vector<std::string>& _argsVec);
		bool		_createSockets(int* socketsToChild, int* socketsFromChild);
		void		_prepareSockets(Client& client, int* socketsToChild, int* socketsFromChild);
		bool		_isRegularFile(std::string file);
		bool		_checkInterpreterScript();
		bool		_checkScriptAbsPath(Client& client, std::vector<std::string>& argsVec);
		std::string	_getInterpreterPath(Client& client, std::string suffix);
		std::string	_getScriptName(std::string suffix, Client& c);
	
		// Attributes
		char**		_tmp;

	public:
					Cgi();
					~Cgi();

	private:
					Cgi(Cgi const & src);
		Cgi &		operator=(Cgi const & rhs);
};

#endif
