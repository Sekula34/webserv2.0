#include "Socket.hpp"
#include <asm-generic/socket.h>
#include <cstdio>
#include <netinet/in.h>
#include <ostream>
#include <stdexcept>
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include "../Utils/Logger.hpp"



#include <iostream>



Socket::Socket() : _port(8080)
{
	
}

/**
 * @brief Construct a new Socket:: Socket object with port number 
 * socket is add to static vector allSockets that need to be close with Socket::closeAllsockets
 * @param portNumber port Number on which socket will listen
 * @throw runtime_error if some of system calls : socket, setsockopt, bind or listen failed
 */
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

	int retVal; // MR_DOUBT: SO_REUSEADDR if for reusing address. we want that, correct? so localhost is available.
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
	Logger::info("Socket is listening on port:");
	std::cout << _port << std::endl;
}

Socket::Socket(const Socket& source) : 
_port(source._port), _socketFD(source._socketFD), _adress(source._adress), _addrlen(source._addrlen)
{

}

Socket& Socket::operator=(const Socket& source)
{
	_socketFD = source._socketFD;
	return (*this);
}

Socket::~Socket()
{
	//this is move to SocketManager
	//close(_socketFD);
}

/**
 * @brief return Socket Fd
 * 
 * @return const int& 
 */
const int& Socket::getSocketFd(void) const 
{
	return(_socketFD);
}

// MR_DOUBT: This function is not being used.
int Socket::getCommunicationSocket(void)
{
	int communicationSocket;
	communicationSocket =  accept(_socketFD, (struct sockaddr *) &_adress, (socklen_t *) &_addrlen);
	if(communicationSocket == -1)
	{
		perror("accept failed");
		throw std::runtime_error("system function accept failed");
	}
	return communicationSocket;
}


std::ostream& operator<<(std::ostream& os, const Socket& socket)
{
	os<< "Socket with port: " << socket._port << std::endl;
	os<< "Socket fd is :" << socket._socketFD << std::endl;
	return os;
}
