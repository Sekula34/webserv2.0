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
		SocketManager();

	public : 
		/**
		 * @brief Construct a new Socket Manager object with all socket from uniqe Ports
		 * 
		 * @param uniquePorts 
		 */
		SocketManager(const std::vector<int>& uniquePorts);
		SocketManager(const SocketManager& source);
		SocketManager& operator=(const SocketManager& source);
		~SocketManager();

		std::vector<Socket>& getAllSockets(void);
		std::vector<int> getAllListenFd(void) const;
		/**
		 * @brief Get the Max Socket Fd for select, 3, 4 ex will return 4
		 * 
		 * @return const int 
		 */
		 int getMaxSocketFd() const;
};

#endif
