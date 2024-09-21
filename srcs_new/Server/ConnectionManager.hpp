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
		void	ConnectionManagerLoop();
		// Attributes

	private:
		// Methods
		void		_epollLoop(void);
		// bool		_handleServerSocket(struct epoll_event* events, int idx);
		void		_updateClientFd(Client& client, const int& idx, const struct epoll_event* events);
		void		_addServerSocketsToEpoll(void); // Make arguments const
		void 		_acceptNewClient(int listen_socket); // Make arguments const

		// Attributes
		int							_epollFd;
		std::map<int, Client*>& 	_clients;

	public:
		ConnectionManager(int epollFd, std::map<int, Client*>& clients);
		~ConnectionManager();

	private:
		ConnectionManager();
		ConnectionManager(ConnectionManager& source);
		ConnectionManager& operator=(ConnectionManager& source);

};

#endif
