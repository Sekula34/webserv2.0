#ifndef CONNECTIONDISPATCHER_HPP
# define CONNECTIONDISPATCHER_HPP
#include "SocketManager.hpp"
#include "../Parsing/ServersInfo.hpp"
#include <cstddef>
#include <sys/select.h>
#include "../Client/ClientHeaderManager.hpp"
#include "../Response/ClientResponseManager.hpp"
#include "../epoll/Client.hpp"
#include "sys/epoll.h"
#define MAX_EVENTS		10
#define MAX_WAIT		-1 //0 epoll coplete non block 6,7 % CPU


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

		std::map<int, Client *>	clients; //maybe private
		struct epoll_event	events[MAX_EVENTS]; // maybe private

		void mainLoop(void);
		void mainLoopEpoll(void);
		int	epollfd; //turn this in private later 
		void epoll_add_listen_sock(int listen_sock); //pribvate probably 
		void epoll_add_client(int epollfd, int listen_socket);
void	epoll_remove_client(struct epoll_event* events, std::map<int, Client*> & clients, Client* client);
	
	Client*	find_client_in_clients(int client_fd, std::map<int, Client *> & clients);
	bool	read_client(struct epoll_event* events, std::map<int, Client *> & clients, Client * client, int & n, int idx);
bool	read_header(struct epoll_event* events, std::map<int, Client *> & clients, Client* client,  int idx);
	void	write_client(struct epoll_event* events, std::map<int, Client *> & clients, Client* client,  int idx);
	void	handle_client(struct epoll_event* events, std::map<int, Client *> & clients, int idx);
	
	private :
		SocketManager &_sockets;
		ServersInfo &_serversInfo;
		ClientHeaderManager _clientHeaders;
		ClientResponseManager _clientResponses;
		struct timeval _selectTimeout;

		void _addServerSocketsToEpoll(void);
		
		/**
		 * @brief 
		 * 
		 * @param idx 
		 * @return true if client is accepted
		 * @return false if fd is client 
		 */
		bool _acceptClient(size_t idx);

		/**
		 * @brief all clientcommunication FDs, if fd is close it should be 
		 * removed from this
		 * 
		 */
		std::vector<int> _communicationFds;
		/**
		 * @brief temp goes in select and will be destroyed every time
		 * 
		 */
		fd_set _readSetTemp, _writeSetTemp, _errorSetTemp;
		/**
		 * @brief master is permanent 
		 * 
		 */
		fd_set _readSetMaster, _writeSetMaster, _errorSetMaster;

		/**
		 * @brief check if cliend fd already exit in _communictaionFDs
		 * this function is called before i want to add new communication
		 * if clienCommuncationFd is alredy in vector that means that some browser/curl closed that fd so i need to remove it. Remove it from all client structutres like headers aswell
		 * @param clientCommunicationSocket new fd that system return and need check
		 */
		void _clientFdCheck(int clientCommunicationFd);

		void _processAnswer(Client& client);

		/**
		 * @brief fill vector with all socket that have something to read and are ready
		 * 
		 * @return std::vector<Socket> which can be empty if nothing is ready 
		 */
		std::vector<Socket> _getAllReadyToReadSockets();
		std::vector<int> _getReadyToReadCommunicationFds();
		void __addServerSocketsToEpoll(void);
		int _getMaxFd(void) const;

		/**
		 * @brief communictaionFD header is fully read and now it should generate Client Response
		 * 
		 * @param communicationFd fd that has fully read client header
		 */
		void _generateClientResponse(int communicationFd);
		/**
		 * @brief function that handles Ready File Descriptor when select return > 0
		 * 
		 */
		void _handleReadyFd(void);
		/**
		 * @brief accept all connection and put communication socket in _communicatonFds
		 * this should be closed after writing. Put socket to be ready to read
		 * 
		 * @param readySockets 
		 */
		void _handleAllReadySockets(std::vector<Socket>& readySockets);
		/**
		 * @brief read readyCleint fds and reutrn string
		 * 
		 * @param clientFd 
		 * @return std::string 
		std::string _readClientFd(int clientFd);
		 */
		
		/**
		 * @brief read CommunictaionFd
		 * 
		 * @param readReadyClientFds fd that are ready to read
		 */
		void _handleAllReadyToReadCommunicationFds(std::vector<int>& readReadyClientFds);
		/**
		 * @brief remoeves client from _communicationfd and close connection
		 * 
		 * @param clientFD fd to find, close and forget from everything
		 */
		void _removeClient(int clientFD);
		/**
		 * @brief closes all client fds;
		 * 
		 */
		void _closeAllClients(void);

		/**
		 * @brief printing something so i know i am not stuck somewhere, and deleteing in immidietly.
		 * commnet this function when doing stress test cuz this is blocking.
		 * commnet this while using valgrind cuz it allocates a lot in long run
		 */
		void _notStuckMessage(void) const;
		//ConnectionDispatcher();
};

#endif
