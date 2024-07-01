#ifndef CONNECTIONDISPATCHER_HPP
# define CONNECTIONDISPATCHER_HPP
#include "SocketManager.hpp"
#include "../Parsing/ServersInfo.hpp"
#include <sys/select.h>
#include "../Client/ClientHeaderManager.hpp"

/**
 * @brief class for handling select creating servers and give them tasks
 * 
 */
class ConnectionDispatcher 
{
	private :
		SocketManager &_sockets;
		ServersInfo &_serversInfo;
		ClientHeaderManager _clientHeaders;
		struct timeval _selectTimeout;

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
		/**
		 * @brief fill vector with all socket that have something to read and are ready
		 * 
		 * @return std::vector<Socket> which can be empty if nothing is ready 
		 */
		std::vector<Socket> _getAllReadyToReadSockets();
		std::vector<int> _getReadyToReadCommunicationFds();
		void _setAllServerListenSocketsForRead(void);
		int _getMaxFd(void) const;
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
		void _removeAllClients(void);
		//ConnectionDispatcher();
	public :
		ConnectionDispatcher(SocketManager& sockets, ServersInfo& serverInfo);
		ConnectionDispatcher(ConnectionDispatcher& source);
		ConnectionDispatcher& operator=(ConnectionDispatcher& source);
		~ConnectionDispatcher();

		void mainLoop(void);

};

#endif
