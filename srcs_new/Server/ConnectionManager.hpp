#ifndef CONNECTIONMANAGER_HPP
# define CONNECTIONMANAGER_HPP

# include <sys/epoll.h>
// # include <list>
#include <map>

class Client;
// class SocketManager;

class ConnectionManager 
{
	public:
		// Methods
		void		epollLoop(void);
		// void	ConnectionManagerLoop();
		// Attributes

	private:
		// Methods
		// bool		_handleServerSocket(struct epoll_event* events, int idx);
		void		_handleClient(Client& client, const int& idx, struct epoll_event* events);
		void		_addServerSocketsToEpoll(void); // Make arguments const
		void 		_acceptNewClient(int listen_socket); // Make arguments const
		void		_handleCgiFds(Client& client, const int& idx, struct epoll_event* events);

		// Attributes
		int							_epollFd;
		std::map<int, Client*>& 	_clients;

	public:
		ConnectionManager(int epollFd);
		~ConnectionManager();

	private:
		ConnectionManager();
		ConnectionManager(ConnectionManager& source);
		ConnectionManager& operator=(ConnectionManager& source);
};

#endif

//============================================================================
// NOTES/IDEAS:
// 1) If we pass too much epol struct, then maybe make it an attribute
// 2) Make epoll operations one function
// 3) Instead of having a _client vector of pairs of fd and e_state; we could have
// a map wgich keey is fd and value is pair of fd_type and fd_state.