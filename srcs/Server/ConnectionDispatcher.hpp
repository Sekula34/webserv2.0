#ifndef CONNECTIONDISPATCHER_HPP
# define CONNECTIONDISPATCHER_HPP
#include <cstddef>
#include <sys/epoll.h>
#include <sys/select.h>
#include "SocketManager.hpp"
#include "../Parsing/ServersInfo.hpp"
#include "../Client/Client.hpp"
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
		Client*						findClientInClients(int client_fd);
		bool						readFd(int fd, Client & client, int & n, int idx);
		bool						readHeader(Client& client,  int idx);
		void						writeClient(Client& client,  int idx);
		void						clientsRemoveFd(Client* client);
		SocketManager &				_sockets;
	
	private :
		std::map<int, Client *>&	_clients;
		ServersInfo &				_serversInfo;
		const int					_epollfd;
		int &						_nfds;
		Client* 					_isClient(int fd);
		bool						_handleServerSocket(size_t idx);
		void						_handleClient(Client& client, int idx);
		bool						_checkReceiveError(Client& client, int n, int peek);
		void						_checkEndHeader(Client& client, int n);
		void						_concatMessageAndPeek(Client* client, int n, int & peek);
		void						_addServerSocketsToEpoll(void);
		void						_checkCgi(Client& client);
		void 						_parseCgiURLInfo(const LocationSettings& cgiLocation, Client& client);
		void						_setCgiPathInfo(const std::string& fullUrl, const std::string fileName, Client& client);
		bool						_isCgiPathInfoValid(std::string pathInfo);
		std::vector<LocationSettings>::const_iterator	_setCgiLocation(Client& client, const ServerSettings& cgiServer, bool& foundLoc);
		void						_runCgi(Client& client);
		void 						_epoll_accept_client(int listen_socket);
		void						_processAnswer(Client& client);
		bool						_catchEpollErrorAndSignal();
		void						_shutdownCgiChildren();

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
