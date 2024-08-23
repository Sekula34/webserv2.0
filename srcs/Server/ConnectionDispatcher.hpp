#ifndef CONNECTIONDISPATCHER_HPP
# define CONNECTIONDISPATCHER_HPP
#include <cstddef>
#include <sys/epoll.h>
#include <sys/select.h>
#include "SocketManager.hpp"
#include "../Parsing/ServersInfo.hpp"
#include "../epoll/Client.hpp"
#include "../epoll/Client.hpp"
#include "../Utils/Data.hpp"



/**
 * @brief class for handling select creating servers and give them tasks
 * 
 */
class ConnectionDispatcher 
{
	public :
		ConnectionDispatcher(SocketManager& sockets, ServersInfo& serverInfo);
		ConnectionDispatcher(ConnectionDispatcher& source);
		ConnectionDispatcher& operator=(ConnectionDispatcher& source);
		~ConnectionDispatcher();


		void						mainLoopEpoll(void);
		Client*						find_client_in_clients(int client_fd);
		bool						read_fd(int fd, Client * client, int & n, int idx);
		bool						read_header(Client* client,  int idx);
		void						write_client(Client* client,  int idx);
		void						clients_remove_fd(Client* client);
		Client*						findSocketClient(int socket);
		SocketManager &				_sockets;
	
	private :
		std::map<int, Client *>&	_clients; //maybe private
		std::map<int,Client*>		_child_sockets;
		ServersInfo &				_serversInfo;
		const int					_epollfd;
		int &						_nfds;
		Client* 					_isClient(int fd);
		bool						_isChildSocket(int fd);
		bool						_handleServerSocket(size_t idx);
		bool						_handleChildSocket(int socket, size_t idx);
		// void						_prepareChildSockets();
		void						_handleClient(Client* client, int idx);
		bool						_checkReceiveError(Client* client, int n, int peek);
		void						_checkEndHeader(Client* client, int n);
		void						_concatMessageAndPeek(Client* client, int n, int & peek);
		void						_addServerSocketsToEpoll(void);
		void						_run_cgi(Client* client);
		void 						_epoll_accept_client(int listen_socket);
		void						_processAnswer(Client& client);
		void						_check_cgi(Client* client);
		bool						_catchEpollErrorAndSignal();

		/**
		 * @brief creates Response instace that is forwarded to client. Client is resposible for deleting
		 * 
		 * @param client in to which Response Belong
		 * @param responseServer server that is resposible for generating response instance
		 */
		void _createAndDelegateResponse(Client& client, const ServerSettings* responseServer);

		/**
		 * @brief printing something so i know i am not stuck somewhere, and deleteing in immidietly.
		 * commnet this function when doing stress test cuz this is blocking.
		 * commnet this while using valgrind cuz it allocates a lot in long run
		 */
		void _notStuckMessage(void) const;
		
};

#endif
