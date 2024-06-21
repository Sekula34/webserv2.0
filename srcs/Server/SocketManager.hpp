#ifndef SOCKETMANAGER_HPP
# define SOCKETMANAGER_HPP
# include <vector>
# include "Socket.hpp"

/**
 * @brief Class that creates and store all Sockets in _allSockets
 * on destruction close all Sockets
 */
class SocketManager 
{
	private :
		std::vector<Socket> _allSockets;

	public : 
		SocketManager();
		/**
		 * @brief Construct a new Socket Manager object with all socket from uniqe Ports
		 * 
		 * @param uniquePorts 
		 */
		SocketManager(std::vector<int>& uniquePorts);
		SocketManager(const SocketManager& source);
		SocketManager& operator=(const SocketManager& source);
		~SocketManager();

		std::vector<Socket>& getAllSockets(void);
};

#endif
