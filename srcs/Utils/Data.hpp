
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
		static const std::map<int, Client*> &	getClients();
		static const Client *					getClientByFd(int fd);
		static const std::vector<Socket> &		getServerSockets();
		static const std::vector<int> 			getServerSocketFds();
		static void								closeAllFds();
		static void								epollAddFd(int fd);
		static void								epollRemoveFd(int fd);

												~Data(void);

	private:
		static int								_epollfd;
		static std::map<int, Client*> &			_clients;
		static std::vector<Socket> &			_serverSockets;
		static struct epoll_event				_events[MAX_EVENTS];

												Data(void);
												Data(Data const & src);
		Data &									operator=(Data const & rhs);
};

#endif

