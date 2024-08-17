#ifndef CONNECTIONDISPATCHER_HPP
# define CONNECTIONDISPATCHER_HPP
#include <cstddef>
#include <sys/epoll.h>
#include <sys/select.h>
#include "SocketManager.hpp"
#include "../Parsing/ServersInfo.hpp"
#include "../epoll/Client.hpp"
#include "../epoll/Client.hpp"

#define MAX_EVENTS		10
#define MAX_WAIT		-1 //0 epoll coplete non block 6,7 % CPU
#define READY			0 
#define NOTREADY		1 
#define ADD				2 
#define DELETE			3 


/**
 * @brief class for handling select creating servers and give them tasks
 * 
 */
class ConnectionDispatcher 
{
	public :
		ConnectionDispatcher(SocketManager& sockets, ServersInfo& serverInfo, char** envp);
		ConnectionDispatcher(ConnectionDispatcher& source);
		ConnectionDispatcher& operator=(ConnectionDispatcher& source);
		~ConnectionDispatcher();

		std::map<int, Client *>	clients; //maybe private
		std::map<int, bool>	child_sockets; //maybe private
		struct epoll_event	events[MAX_EVENTS]; // maybe private

		void 	mainLoopEpoll(void);
		int		epollfd; //turn this in private later 
	
		Client*	find_client_in_clients(int client_fd, std::map<int, Client *> & clients);
		bool	read_client(struct epoll_event* events, std::map<int, Client *> & clients, Client * client, int & n, int idx);
		bool	read_header(struct epoll_event* events, std::map<int, Client *> & clients, Client* client,  int idx);
		void	write_client(struct epoll_event* events, std::map<int, Client *> & clients, Client* client,  int idx);
		void	handle_client(struct epoll_event* events, std::map<int, Client *> & clients, int idx);
		void	clients_remove_fd(std::map<int, Client*> & clients, Client* client);
		void	handle_child_sockets();
		void	epoll_remove_fd(struct epoll_event* events, Client* client);
		void	epoll_add_fd(int epollfd, int clientfd);
	
	private :
		std::map<int,int>	_child_sockets;
		bool				_is_childsocket(int fd);
		SocketManager &_sockets;
		ServersInfo &_serversInfo;

		void _addServerSocketsToEpoll(void);
		bool	run_cgi(Client* client);
		
		/**
		 * @brief 
		 * 
		 * @param idx 
		 * @return true if client is accepted
		 * @return false if fd is client 
		 */
		bool	_isServerSocket(size_t idx);
		void 	_epoll_accept_client(int epollfd, int listen_socket);


		void _processAnswer(Client& client);

		/**
		 * @brief creates Response instace that is forwarded to client. Client is resposible for deleting
		 * 
		 * @param client in to which Response Belong
		 * @param responseServer server that is resposible for generating response instance
		 */
		void _createAndDelegateResponse(Client& client, const ServerSettings* responseServer);
		void __addServerSocketsToEpoll(void);

		/**
		 * @brief printing something so i know i am not stuck somewhere, and deleteing in immidietly.
		 * commnet this function when doing stress test cuz this is blocking.
		 * commnet this while using valgrind cuz it allocates a lot in long run
		 */
		void _notStuckMessage(void) const;
		void	_check_cgi(Client* client);
		char**	_envp;
		
};

#endif
