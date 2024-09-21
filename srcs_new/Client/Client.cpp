
#include "Client.hpp"
#include <cstddef>
#include <cstring>
#include <iostream>
#include "../Message/Message.hpp"
#include "../Utils/Logger.hpp"
#include "../Utils/Data.hpp"
#include <sstream>

//==========================================================================//
// STATIC ATTRIBUTES/METHODS================================================//
//==========================================================================//

// Initializing static attributes
int	Client::client_cntr = 0;

//==========================================================================//
// REGULAR METHODS==========================================================//
//==========================================================================//

const Client::e_clientState&		Client::getClientState() const
{
	return (_clientState);
}

std::clock_t	Client::getStartTime() const
{
	return (_start);
}

unsigned long	Client::getId() const
{
	return (_id);
}

int	Client::getClientFd() const
{
	int ret = 0;
	if (_clientFds.size() > 0)
		ret = _clientFds.begin()->first;
	else
	 	Logger::error("F@ck no fd in client with id ", _id);
	return (ret);
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

void	Client::setClientState(e_clientState state)
{
	_clientState = state;
}

void	Client::setClientFdState(int idx, e_fdState fdState)
{
	if (idx > _clientFds.size())
	{
		Logger::error("Trying to change the state of a non-existing fd; your idx is: ", idx);
		return ;
	}
	_clientFds[idx].second = fdState;
}

void Client::_initVars(int fd)
{

	// _socketFromChild = DELETED;
	// _socketToChild = DELETED;
	_clientFds.push_back(fdStatePair(fd, NONE));
	_initClientIp();
	_errorCode = 0;
	_clockstop = 1000;
	_requestMsg = NULL;
	_responseMsg = NULL;
}

void	Client::setChildSocket(int to, int from)
{
	// _socketToChild = to;
	// _socketFromChild = from;
	_clientFds.push_back(std::pair<int, e_fdState>(to, NONE));
	_clientFds.push_back(std::pair<int, e_fdState>(from, NONE));
}

void	Client::closeSocketToChild()
{
	// _socketToChild = DELETED;
	if (_clientFds.size() < 2)
		Logger::error("F@ck no socketToChild in client with id ", _id);
	else
		_clientFds[1].second = CLOSE;
}

void	Client::closeSocketFromChild()
{
	// _socketFromChild = DELETED;
	if (_clientFds.size() < 3)
		Logger::error("F@ck no socketFromChild in client with id ", _id);
	else
		_clientFds[2].second = CLOSE;
}

// TODO: Long discussion with Filip about this function.
unsigned short	Client::getClientPort()
{
	struct sockaddr_in local_addr;
	socklen_t local_addr_len = sizeof(local_addr);
	if (getsockname(getClientFd(), (struct sockaddr *)&local_addr, &local_addr_len) == -1)
	{
		setErrorCode(500);
		return (0);
	}
	return (ntohs(local_addr.sin_port));
}

void	Client::_initClientIp()
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

void	Client::closeClientFds()
{
	fdPairsVec::iterator it = _clientFds.begin();
	for (; it != _clientFds.end(); ++it)
	{
		if (it->second != CLOSED)
		{
			close(it->first);
			it->second = CLOSED;
		}
	}
}

//==========================================================================//
// Constructor, Destructor and OCF Parts ===================================//
//==========================================================================//

// Custom Constructor
Client::Client (int const fd, struct sockaddr clientAddr, socklen_t addrLen):
	_id(++client_cntr),
	// _fd(fd),
	_start(std::clock()),
	_clientAddr(clientAddr),
	_addrLen(addrLen)
{
	// _clientFds.push_back(fdStatePair(fd, UNSET));
	_initVars(fd);
	// _initClientIp();
	Logger::info("Client constructed, unique ID: ", _id);
	Logger::info("Client Fd: ", getClientFd());
}

// Default Constructor
Client::Client (void): 
_id(0),
_start(std::clock()) {}

// Destructor
Client::~Client (void)
{
	delete _requestMsg;
	delete _responseMsg;
	delete _cgiResponseMsg;
	_requestMsg = NULL;
	_responseMsg = NULL;
	_cgiResponseMsg = NULL;
	Logger::info("Client destructed, unique ID: ", _id);
	Logger::info("Closing Fd: ", getClientFd());
	// if (_socketToChild != DELETED)
	// 	close(_socketToChild);
	// if (_socketFromChild != DELETED)
	// 	close(_socketFromChild);
	// close (_fd);
	closeClientFds();
}

// Copy Constructor
Client::Client(Client const & src):
_id(++client_cntr),
// _fd(src._fd),
_clientFds(src._clientFds),
_start(std::clock())
{
	*this = src;
}

// Copy Assignment Operator
Client &	Client::operator=(const Client&)
{
	return (*this);
}
