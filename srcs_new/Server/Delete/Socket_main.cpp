#include "Socket.hpp"
#include <vector>
#include <iostream>
#include <unistd.h>

int	main()
{
	{
		Socket	socket01(8080);
		Socket	socket02(9090);
	}
	std::vector<Socket> sockets = Socket::getSockets();
	for (std::vector<Socket>::const_iterator it = sockets.begin(); it != sockets.end(); ++it)
		std::cout << it->getSocketFD() << std::endl;
	// std::vector<int> closeSockets = Socket::getSockets();
	for (std::vector<Socket>::const_iterator it = sockets.begin(); it != sockets.end(); ++it)
		close(it->getSocketFD());
}
