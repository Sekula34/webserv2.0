
#include "Client.hpp"
#include <cstddef>
#include <cstring>
#include <exception>
#include <iostream>
#include "../Message/Message.hpp"
#include "../Utils/Logger.hpp"
// #include "../Utils/Data.hpp"
#include "FdData.hpp"
#include <sstream>
#include <algorithm>
#include <vector>


//==========================================================================//
// STATIC ATTRIBUTES/METHODS================================================//
//==========================================================================//

// Initializing static attributes
size_t	Client::client_cntr = 0;

std::map<int, Client*>	Client::clients;

//==========================================================================//
// REGULAR METHODS==========================================================//
//==========================================================================//

Message*	Client::getMsg(e_clientMsgType type)
{
	try
	{
		if (type == REQ_MSG)
		{	
			if (!_requestMsg)
				_requestMsg = new Message(true, _errorCode);
			return (_requestMsg);
		}
		if (type == RESP_MSG)
		{	
			if (!_responseMsg)
				_responseMsg = new Message(false, _errorCode);
			return (_responseMsg);
		}
	}
	catch (std::exception& e)
	{
		Logger::error("F@ck could not create new Message in client: ", _id);
	}
	return (NULL);
}

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

FdData&		Client::getFdDataByType(FdData::e_fdType type)
{
	FdData::findType functor(type);

    std::vector<FdData>::iterator it = std::find_if(_clientFds.begin(), _clientFds.end(), functor);
    if(it == _clientFds.end())
	{
		Logger::error("F@ck, looking for a Fd type that does not exist in this FdData instance of client with id: ", _id);
		return (*_clientFds.begin());
	}
	return (*it);
}

FdData&		Client::getFdDataByFd(int fd)
{
	FdData::findFd functor(fd);

    std::vector<FdData>::iterator it = std::find_if(_clientFds.begin(), _clientFds.end(), functor);
    if(it == _clientFds.end())
	{
		// sleep (1);
		// Logger::error("F@ck, looking for a Fd that is not amongst client fds. Client id: ", _id);
		// Logger::error("fd: ", fd);
		return (*_clientFds.begin());
	}
	return (*it);
}

std::vector<FdData>&	Client::getClientFds()
{
	return (_clientFds);
}

int&	Client::getErrorCode()
{
	return (_errorCode);
}

std::string	Client::getClientIp() const
{
	return (_clientIp);
}

const VirtualServer* Client::getVirtualServer() const
{
	return _virtualServer;
}

const bool&				Client::getCgiFlag() const
{
	return (_cgiFlag);
}

const int&	Client::getWaitReturn() const
{
	return (_waitReturn);
}

const int&	Client::getChildPid() const
{
	return (_childPid);
}

const int&	Client::getSignalSent() const
{
	return (_signalSent);
}

void Client::setVirtualServer(const VirtualServer& vs)
{
	_virtualServer = &vs;
}

void	Client::setErrorCode(int c)
{
	if(_errorCode != 0)
	{
		Logger::error("You are overwriting client error code, old code is: ", _errorCode);
		Logger::info("New error code is: ", c);
	}
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

void	Client::setCgiFlag(bool b)
{
	_cgiFlag = b;
}

void	Client::setClientState(e_clientState state)
{
	_clientState = state;
}

void	Client::setWaitReturn(int num)
{
	_waitReturn = num;
}

void	Client::setSignalSent(int num)
{
	_signalSent = num;
}

void	Client::setChildSocket(int to, int from)
{
	// Logger::info("adding socket TOCHILD to client Fds", to);
	_clientFds.push_back(FdData(to, FdData::TOCHILD_FD));
	// Logger::info("adding socket FROMCHILD to client Fds", from);
	_clientFds.push_back(FdData(from, FdData::FROMCHILD_FD));
}

void	Client::setFileFd(int fd)
{
	// Logger::info("adding socket TOCHILD to client Fds", to);
	_clientFds.push_back(FdData(fd, FdData::TOFILE));
}

void	Client::setChildPid(int pid)
{
	_childPid = pid;
}

bool	Client::checkTimeout(double maxtime)
{
	double diff = (static_cast<double>(std::clock() - _start) * 1000) / CLOCKS_PER_SEC;
	if (diff > maxtime)
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

void Client::_initVars(int fd)
{
	// _socketFromChild = DELETED;
	// _socketToChild = DELETED;
	_clientState = DO_REQUEST;
	_clientFds.push_back(FdData(fd, FdData::CLIENT_FD));
	// _clientFds.push_back(fdStatePair(fd, NONE));
	_initClientIp();
	_errorCode = 0;
	// _clockstop = 1000;
	_requestMsg = new Message(true, _errorCode);
	_responseMsg = new Message(false, _errorCode);
	// _cgiResponseMsg = NULL;
	clients[fd] = this;
	_isRequestChecked = false;
	_cgiFlag = false;
	_waitReturn = 0;
	_childPid = 0;
	_signalSent = 0;
}


void	Client::closeSocketToChild()
{
	// _socketToChild = DELETED;
	if (_clientFds.size() < 2)
		Logger::error("F@ck no socketToChild in client with id ", _id);
	else
	{
		FdData& fdData =  getFdDataByType(FdData::TOCHILD_FD);
		fdData.state = FdData::CLOSE;
		// _clientFds.state = CLOSE;
	}
}

void	Client::closeSocketFromChild()
{
	// _socketFromChild = DELETED;
	if (_clientFds.size() < 3)
		Logger::error("F@ck no socketFromChild in client with id ", _id);
	else
	{
		FdData& fdData =  getFdDataByType(FdData::FROMCHILD_FD);
		fdData.state = FdData::CLOSE;
	}
}

// TODO: Long discussion with Filip about this function.
unsigned short	Client::getClientPort()
{
	struct sockaddr_in local_addr;
	socklen_t local_addr_len = sizeof(local_addr);
	FdData& fdData = getFdDataByType(FdData::CLIENT_FD);
	if (getsockname(fdData.fd, (struct sockaddr *)&local_addr, &local_addr_len) == -1)
	{
		setErrorCode(500);
		return (0);
	}
	return (ntohs(local_addr.sin_port));
}

const bool&	Client::getIsRequestChecked() const
{
	return (_isRequestChecked);
}

void	Client::setIsRequestChecked()
{
	_isRequestChecked = true;
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
	std::vector<FdData>::iterator it = _clientFds.begin();
	for (; it != _clientFds.end(); ++it)
	{
	
		if (it->state != FdData::CLOSED)
		{
			close(it->fd);
			it->state = FdData::CLOSED;
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
	_addrLen(addrLen),
	_virtualServer(NULL)
{
	// _clientFds.push_back(fdStatePair(fd, UNSET));
	_initVars(fd);
	// _initClientIp();
	Logger::info("Client constructed, unique ID: ", _id);
	Logger::info("Client Fd: ", getFdDataByType(FdData::CLIENT_FD).fd);
}

// Default Constructor
Client::Client(): 
_id(0),
_start(std::clock()),
_virtualServer(NULL)
{}

// Destructor
Client::~Client()
{
	delete _requestMsg;
	_requestMsg = NULL;
	delete _responseMsg;
	_responseMsg = NULL;
	// delete _cgiResponseMsg;
	// _cgiResponseMsg = NULL;
	Logger::info("Closing Fd: ", getFdDataByType(FdData::CLIENT_FD).fd);
	closeClientFds();
	clients.erase(getFdDataByType(FdData::CLIENT_FD).fd);
	Logger::info("Client destructed, unique ID: ", _id);
}

// Copy Constructor
Client::Client(Client const & src):
_id(++client_cntr),
// _fd(src._fd),
_clientFds(src._clientFds),
_start(std::clock()),
_errorCode(src._errorCode),
_virtualServer(NULL)
{
	*this = src;
	Logger::warning("Copy Constructor of Client called", src.getId());
}

// Copy Assignment Operator
Client&	Client::operator=(const Client&)
{
	return (*this);
}
