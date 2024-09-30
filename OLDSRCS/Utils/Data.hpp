
#ifndef DATA_HPP
# define DATA_HPP

# include <iostream>
# include <map>
# include <vector>
# include <sys/epoll.h>
# define MAX_EVENTS		40 // what happens if we exceed this?

class Client;
class Socket;

class Data {

	public:

		static int								getEpollFd();
		static std::map<int, Client*> &			getClients();
		static const Client *					getClientByFd(int fd);
		static const std::vector<Socket> &		getServerSockets();
		static const std::vector<int> 			getServerSocketFds();
		static int &							getNfds();
		static const std::map<std::string, std::string>&	getCgiLang();
		static bool								isCgiExtensionValid(std::string cgiExtension);
		static struct epoll_event *				setEvents(); 
		static void								setCgiLang(std::string suffix, std::string interpreter);
		static void								setAllCgiLang(void);
		static void								closeAllFds();
		static void								epollAddFd(int fd);
		static void								epollRemoveFd(int fd);
		static void								setEpollFd(int fd);
		static void								setServerSockets(std::vector<Socket> * sockets);
		static void								setEnvp(char** envp);
		static std::string						findStringInEnvp(std::string str);


												~Data(void);

	private:
		static char**							_envp;
		static int								_epollfd;
		static std::map<int, Client*> &			_clients;
		static std::vector<Socket> *			_serverSockets;
		static struct epoll_event				_events[MAX_EVENTS];
		static int								_nfds;
		static std::map<std::string, std::string>	_cgiLang; //this is map to fill it .py python3 before parsing in constructor of cgi. Config file wil check this 

												Data(void);
												Data(Data const & src);
		Data &									operator=(Data const & rhs);
};

#endif
