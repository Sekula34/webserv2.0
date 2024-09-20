#ifndef CONNECTIONDISPATCHER_HPP
# define CONNECTIONDISPATCHER_HPP

# include <sys/epoll.h>

class Client;
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
		void 		_epollAcceptClient(int listen_socket);
		// Attributes

	public:
		ConnectionDispatcher();
		~ConnectionDispatcher();

	private:
		ConnectionDispatcher(ConnectionDispatcher& source);
		ConnectionDispatcher& operator=(ConnectionDispatcher& source);

};

#endif