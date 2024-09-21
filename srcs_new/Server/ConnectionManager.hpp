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
		// Client*		_findClientInClients(int client_fd);
		bool		_handleServerSocket(struct epoll_event* events, int idx);
		// void		_addServerSocketsToEpoll(void);
		void		_addServerSocketsToEpoll();
		void 		_acceptNewClient(int listen_socket);
		// Attributes
		int							_epollFd;
		// std::list<Client*>& 		_clients;
		std::map<int, Client*>& 	_clients;

	public:
		// ConnectionManager(int epollFd, std::list<Client*>& clients);
		ConnectionManager(int epollFd, std::map<int, Client*>& clients);
		~ConnectionManager();

	private:
		ConnectionManager();
		ConnectionManager(ConnectionManager& source);
		ConnectionManager& operator=(ConnectionManager& source);

};

#endif
