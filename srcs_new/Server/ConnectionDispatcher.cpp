#include "ConnectionDispatcher.hpp"
#include "Socket.hpp"
#include <stdexcept>

//REGULAR METHODS===========================================================//

/* CREATE CLIENT FD BY CALLING ACCEPT ON LISTEN SOCKET, CREATE CLIENT INSTANCE
ADD INSTANCE TO CLIENTS MAP. MAP KEY: CLIENT FD, MAP VALUE: CLIENT INSTANCE POINTER */
// void	ConnectionDispatcher::_epoll_accept_client(int listen_socket)
static void	_epollAcceptClient(int listen_socket)
{
	struct sockaddr client_addr;
	socklen_t addrlen = sizeof(client_addr);
	int	clientfd;

	// ACCEPT RETURNS CLIENT FD
	clientfd = accept(listen_socket, &client_addr, &addrlen);
	if (clientfd == -1)
		throw std::runtime_error("accept error");

	// CREATE NEW CLIENT INSTANCE WITH CLIENT FD CONSTRUCTOR
	Client * newClient = new Client(clientfd, client_addr, addrlen);

	// IF CLIENT FD ALREADY EXISTS IN MAP, THEN SET NOWRITE IN THE CLIENT INSTANCE.
	// WE DON'T IMMEADIATELY DELETE CLIENT BECAUSE IT MIGHT BE PROCESSING.
	// IN THAT CASE THE  CLIENT INSTANCE NEEDS TO FINISH ITS JOB
	// AND HAS TO BE DELETED -> THIS CASE IS NOT YET TAKEN CARE OF
	// RIGHT NOW MEMORY LEAK!!! ADD TO GRAVEYARD VECTOR??
	if (_clients.find(clientfd) != _clients.end())
		_clients[clientfd]->setWriteClient(false);
	_clients[clientfd] = newClient;

	// ADD CLIENT FD TO THE LIST OF FDS THAT EPOLL IS WATCHING FOR ACTIVITY
	Data::epollAddFd(clientfd);
}

bool ConnectionDispatcher::_handleServerSocket(size_t idx)
{
	for(size_t i = 0; i < Socket::getSocketFDs().size(); i++)
	{
		if (Data::setEvents()[idx].data.fd == Socket::getSocketFDs()[i])
		{
			_epollAcceptClient(Socket::getSocketFDs()[i]);
			return true;
		}
	}
	return false;
}
