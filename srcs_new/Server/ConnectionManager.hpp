#ifndef CONNECTIONMANAGER_HPP
# define CONNECTIONMANAGER_HPP

# include <sys/epoll.h>
// # include <list>
#include <map>

#define MAX_EVENTS		40 // what happens if we exceed this?

class Client;
// class SocketManager;

class ConnectionManager 
{
	public:
		// Methods
		void		epollLoop(void);
		static void	closeClientFds();
		static const int& getEpollFd();
		// Attributes

	private:
		// Methods
		void		_handleClient(Client& client, const int& idx);
		void		_addServerSocketsToEpoll(void); // Make arguments const
		void 		_acceptNewClient(int listen_socket); // Make arguments const
		void		_handleCgiFds(const int& idx);
		void		_addChildSocketsToEpoll();
		void		_removeFromEpollUnclosedFds(Client& client);

		// Attributes
		static int					_epollFd;
		std::map<int, Client*>& 	_clients;
		struct epoll_event			_events[MAX_EVENTS];

	public:
		ConnectionManager();
		~ConnectionManager();

	private:
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
