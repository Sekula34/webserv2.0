
#include "Client.hpp"
#include <cstddef>
#include <cstring>
#include <iostream>
#include "../Message/Message.hpp"
#include "../Utils/Logger.hpp"
#include "../Utils/Data.hpp"
#include <sstream>


int	Client::client_cntr = 0;



std::clock_t	Client::getStartTime() const
{
	return (_start);
}


unsigned long	Client::getId() const
{
	return (_id);
}

int	Client::getFd() const
{
	return (_fd);
}

Message*	Client::getRequestMsg()const
{
	return (_requestMsg);
}

Message*	Client::getResponseMsg()const
{
	return (_responseMsg);
}

Message*	Client::getCgiResponseMsg()const
{
	return (_cgiResponseMsg);
}

int	Client::getErrorCode() const
{
	return (_errorCode);
}

std::string	Client::getClientIp() const
{
	return (_clientIp);
}

void	Client::setErrorCode(int c)
{
	_errorCode = c;
}


void	Client::setAddrlen(socklen_t addrLen)
{
	_addrLen = addrLen;
}

void	Client::setRequestMsg(Message* m)
{
	_requestMsg = m;
}

void	Client::setResponseMsg(Message* m)
{
	_responseMsg = m;
}

void	Client::setCgiResponseMsg(Message* m)
{
	_cgiResponseMsg = m;
}

bool	Client::checkTimeout()
{
	double diff = (static_cast<double>(std::clock() - _start) * 1000) / CLOCKS_PER_SEC;
	if (diff > MAX_TIMEOUT)
	{
		// Logger::warning("removing Client due to timeout", true);
		return (false);
	}
	// if (diff > _clockstop) 
	// {
	// 	std::cout << "id: " << _id << ", " << diff / 1000 << " sec" << std::endl;
	// 	_clockstop += 1000;
	// }
	return (true);
}


void Client::_initVars(void)
{

	_socketFromChild = DELETED;
	_socketToChild = DELETED;
	_errorCode = 0;
	_clockstop = 1000;
	_requestMsg = NULL;
	_responseMsg = NULL;
}

void	Client::setChildSocket(int to, int from)
{
	_socketToChild = to;
	_socketFromChild = from;
}

void	Client::unsetsocket_tochild()
{
	_socketToChild = DELETED;
}

void	Client::unsetsocket_fromchild()
{
	_socketFromChild = DELETED;
}

unsigned short	Client::getClientPort()
{
	struct sockaddr_in local_addr;
    socklen_t local_addr_len = sizeof(local_addr);
    if (getsockname(_fd, (struct sockaddr *)&local_addr, &local_addr_len) == -1)
	{
		setErrorCode(500);
		return (0);
	}
	return (ntohs(local_addr.sin_port));
}

void	Client::_init_user_info()
{
	std::string address;
	std::stringstream ss;

	struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&_clientAddr;
	unsigned long num  = pV4Addr->sin_addr.s_addr;

	num = ntohl(num);
	ss << int((num&0xFF000000)>>24);
	ss << ".";
	ss << int((num&0xFF0000)>>16);
	ss << ".";
	ss << int((num&0xFF00)>>8);
	ss << ".";
	ss << int(num&0xFF);
	_clientIp = ss.str();
}

//==========================================================================//
// Constructor, Destructor and OCF Parts ===================================//
//==========================================================================//

// Custom Constrcutor
Client::Client (int const fd, struct sockaddr clientAddr, socklen_t addrLen):
	_id(++client_cntr),
	_fd(fd),
	_start(std::clock()),
	_clientAddr(clientAddr),
	_addrLen(addrLen)
{
	_initVars();
	_init_user_info();
	Logger::info("Client constructed, unique ID: ", _id);
	Logger::info("Client FD: ", _fd);
}

// Default Constructor
Client::Client (void): 
_id(0),
_fd(0),
_start(std::clock()) {}

// Destructor
Client::~Client (void)
{
	if (_socketToChild != DELETED)
		close(_socketToChild);
	if (_socketFromChild != DELETED)
		close(_socketFromChild);
	delete _requestMsg;
	delete _responseMsg;
	delete _cgiResponseMsg;
	_requestMsg = NULL;
	_responseMsg = NULL;
	_cgiResponseMsg = NULL;
	Logger::info("Client destructed, unique ID: ", _id);
	Logger::info("Closing FD: ", _fd);
	close (_fd);
}

// Copy Constructor
Client::Client(Client const & src):
_id(++client_cntr),
_fd(src._fd),
_start(std::clock())
{
	*this = src;
}

// Copy Assignment Operator
Client &	Client::operator=(const Client&)
{
	return (*this);
}
