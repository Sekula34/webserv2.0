#ifndef CONNECTIONDISPATCHER_HPP
# define CONNECTIONDISPATCHER_HPP

# include <sys/epoll.h>
# include <map>

class Client;
class CgiProcessor;
// class SocketManager;

class ConnectionDispatcher 
{
	public:
		// Methods
		
		// Attributes

	private:
		// Methods
		void		_epollLoop(void);
		Client*		_findClientInClients(int client_fd);
		bool		_handleServerSocket(size_t idx);
		void		_addServerSocketsToEpoll(void);
		// void 		_epollAcceptClient(int listen_socket);
		// Attributes
		const int						_epollfd;
		int								_nfds; // fds which have activity returned by epoll_wait
		std::map<int, Client*>			_clients;
		std::map<int, CgiProcessor>		_cgis;

	public:
		ConnectionDispatcher();
		~ConnectionDispatcher();

	private:
		ConnectionDispatcher(ConnectionDispatcher& source);
		ConnectionDispatcher& operator=(ConnectionDispatcher& source);
};

#endif