#include "Socket.hpp"
#include "../Utils/Logger.hpp"
#include <cstring>
#include <stdexcept>
#include <unistd.h>

//==========================================================================//
// STATIC ATTRIBUTES/METHODS================================================//
//==========================================================================//

// Initializing static attributes
std::vector<Socket>	Socket::_allSockets;

std::vector<Socket>&	Socket::getSockets()
{
	return (_allSockets);
}

void	Socket::closeSockets()
{
	std::vector<Socket> sockets = Socket::getSockets();
	std::vector<Socket>::const_iterator it = sockets.begin();
	for (; it != sockets.end(); ++it)
		close(it->_socketFD);
}

//==========================================================================//
// REGULAR METHODS==========================================================//
//==========================================================================//

const int&	Socket::getSocketFd() const
{
	return (_socketFD);
}

//==========================================================================//
// Constructor, Destructor and OCF Parts ===================================//
//==========================================================================//

// Custom Constructor
Socket::Socket(int portNumber) : _port(portNumber)
{
	_addrlen = sizeof(sockaddr_in);
	_socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if(_socketFD == -1)
	{
		Logger::error("socket function in socket Default constructor failed", "");
		throw std::runtime_error("System call socket failed");
	}

	memset((char *)&_adress, 0, _addrlen);
	_adress.sin_family = AF_INET;
	_adress.sin_addr.s_addr = htonl(INADDR_ANY);
	_adress.sin_port = htons(_port);

	int opt(1);
	int retVal;

	// TODO: check if this is ok with subject. Reuseaddr is to make bind not fail if restart is quick.
	retVal = setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)); 
	if(retVal == -1)
	{
		Logger::error("setsockopt systemcall in socket constructor failed", "");
		throw std::runtime_error("System call setsockopt failed");
	}
	retVal = bind(_socketFD, (struct sockaddr*) &_adress, _addrlen);
	if(retVal == -1)
	{
		Logger::error("Bind function in Socket constructor failed ", "");
		throw std::runtime_error("System call bind failed");
	}
	retVal = listen(_socketFD, backlog);
	if(retVal == -1)
	{
		Logger::error("listen systemcall failed", "");
		throw std::runtime_error("System call listen failed");
	}
	_allSockets.push_back(*this); // Adding socket to _allsockets vector.
	Logger::info("Socket is listening on port: ", _port);
}

// Default Constructor
Socket::Socket() : _port(8080) {}

// Copy Constructor
Socket::Socket(const Socket& source) : 
_port(source._port), _socketFD(source._socketFD),
_adress(source._adress), _addrlen(source._addrlen)
{}

// Destructor
Socket::~Socket() {}

//==========================================================================//
// NOTES:
// 1) We don't close the socket fd because we need it, it will be closed
// by ConnectionManager Destructor (when program ends).
