#include "Socket.hpp"
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include "../Utils/Logger.hpp"

//STATIC ATTRIBUTES/METHODS==================================================//
std::vector<Socket>	Socket::_allSockets;

const std::vector<int>	Socket::getSocketFDs(void)
{
	std::vector<int> fds;
	std::vector<Socket>::const_iterator it = _allSockets.begin();
	for (; it != _allSockets.end(); ++it)
		fds.push_back(it->_socketFD);
	return (fds);
}

//Constructor, Desructor and other stuff====================================//
// Custom constructor
Socket::Socket(int portNumber) : _port(portNumber)
{
	_addrlen = sizeof(sockaddr_in);
	_socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if(_socketFD == -1)
	{
		perror("socket function in socket Default constructor failed");
		throw std::runtime_error("System call socket failed");
	}

	memset((char *)&_adress, 0, _addrlen);
	_adress.sin_family = AF_INET;
	_adress.sin_addr.s_addr = htonl(INADDR_ANY);
	_adress.sin_port = htons(_port);

	int opt(1);
	int retVal;
	// TODO: check if this is ok with subjcet. Reuseaddr is to make bind not fail if restart is quick.
	retVal = setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)); 
	if(retVal == -1)
	{
		perror("setsockopt systemcall in socket constructor failed");
		throw std::runtime_error("System call setsockopt failed");
	}
	retVal = bind(_socketFD, (struct sockaddr*) &_adress, _addrlen);
	if(retVal == -1)
	{
		perror("Bind function in Socket constructor failed ");
		throw std::runtime_error("System call bind failed");
	}
	retVal = listen(_socketFD, backlog);
	if(retVal == -1)
	{
		perror("listen systemcall failed");
		throw std::runtime_error("System call listen failed");
	}

	_allSockets.push_back(*this); // Adding soket to _allsocket vector.
	Logger::info("Socket is listening on port:", _port);
}

// Default constructor
Socket::Socket() : _port(8080) {}

// Copy constructor
Socket::Socket(const Socket& source) : 
_port(source._port), _socketFD(source._socketFD),
_adress(source._adress), _addrlen(source._addrlen)
{}

// Destructor
Socket::~Socket() {}
