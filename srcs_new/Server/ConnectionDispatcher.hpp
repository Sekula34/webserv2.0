#ifndef CONNECTIONDISPATCHER_HPP
# define CONNECTIONDISPATCHER_HPP

# include <sys/epoll.h>

class Client;
class SocketManager;

class ConnectionDispatcher 
{
	public:
		// Methods
		
		// Attributes

	private:
		// Methods
		void		_mainLoopEpoll(void);
		Client*		_findClientInClients(int client_fd);
		bool		_handleServerSocket(size_t idx);
		void		_addServerSocketsToEpoll(void);
		void 		_epoll_accept_client(int listen_socket);
		// Attributes

	public:
		ConnectionDispatcher(SocketManager& sockets, ServersInfo& serverInfo);
		~ConnectionDispatcher();

	private:
		ConnectionDispatcher(ConnectionDispatcher& source);
		ConnectionDispatcher& operator=(ConnectionDispatcher& source);

};

#endif