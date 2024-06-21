#include "Socket.hpp"
#include <asm-generic/socket.h>
#include <cstdio>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>



#include <iostream>


Socket::Socket() : _port(8080)
{
	
}

/**
 * @brief Construct a new Socket:: Socket object with port number 
 * 
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

	int retVal;
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
	std::cout << "Socket is listening on port " << _port << std::endl; 
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
