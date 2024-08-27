
#include "Client.hpp"
#include <cstddef>
#include <cstring>
#include <iostream>
#include "../Utils/Logger.hpp"
#include "../Utils/Data.hpp"
#include <sstream>


/******************************************************************************/
/*                               Constructors                                 */
/******************************************************************************/
int	Client::client_cntr = 0;

Client::Client (void): 
_id(0),
_fd(0),
_start(std::clock()),
_epollFd(0)
{ 	
	// std::cout << "Client default constructor called" << std::endl;
}

Client::Client (int const fd, struct sockaddr clientAddr, socklen_t addrLen):
	_id(++client_cntr),
	_fd(fd),
	_start(std::clock()),
	_epollFd(Data::getEpollFd()),
	_cgi(NULL),
	_clientAddr(clientAddr),
	_addrLen(addrLen)
{
	_initVars();
	_init_user_info();
	Logger::info("Client constructed, unique ID: "); std::cout << _id;
	std::cout << " FD: "; std::cout << _fd << std::endl;
}

/******************************************************************************/
/*                                Destructor                                  */
/******************************************************************************/

Client::~Client (void)
{
	Logger::info("Client destructed, unique ID: "); std::cout << _id;
	std::cout << " FD: "; std::cout << _fd << std::endl;
	close (_fd);
	if (socketToChild != DELETED)
		close(socketToChild);
	if (socketFromChild != DELETED)
		close(socketFromChild);
	delete [] _recvLine;
	delete header;
	delete _response;
	delete _cgi;
}

/******************************************************************************/
/*                             Copy Constructor                               */
/******************************************************************************/

Client::Client(Client const & src):
_id(++client_cntr),
_fd(src._fd),
_start(std::clock()),
_epollFd(src._epollFd)
{
	//std::cout << "Client copy constructor called" << std::endl;
	_recvLine = new unsigned char[MAXLINE];
	memset(_recvLine, 0, MAXLINE);
	*this = src;
}

/******************************************************************************/
/*                      Copy Assignment Operator Overload                     */
/******************************************************************************/

Client &	Client::operator=(Client const & rhs)
{
	//std::cout << "Client Copy assignment operator called" << std::endl;
	if (this != &rhs)
	{
		_readHeader = rhs._readHeader;
		_readBody = rhs._readBody;
		_writeClient = rhs._writeClient;
	}
	return (*this);
}

/******************************************************************************/
/*                          Setters and Getters                               */
/******************************************************************************/

std::clock_t	Client::getStartTime() const
{
	return (_start);
}


Response* Client::getResponse()const 
{
	return(_response);
}

void Client::setResponse(Response* response)
{
	if(_response != NULL)
	{
		Logger::warning("Setting response in Client but it already have one. Possible leak", true);
		return;
	}
	_response = response;
}

unsigned long	Client::getId() const
{
	return (_id);
}

int	Client::getFd() const
{
	return (_fd);
}

std::string	Client::getMessage() const
{
	return (_message);
}

std::string	Client::getCgiMessage() const
{
	return (_cgiMessage);
}

unsigned char*	Client::getRecvLine() const
{
	return (_recvLine);
}

int	Client::getEpollFd() const
{
	return (_epollFd);
}
bool	Client::getReadHeader() const
{
	return (_readHeader);
}

bool	Client::getReadBody() const
{
	return (_readBody);
}

bool	Client::getWriteClient() const
{
	return (_writeClient);
}

int	Client::getErrorCode() const
{
	return (_errorCode);
}

std::string const &	Client::getClientBody() const
{
	return (_clientBody);
}

CgiProcessor*	Client::getCgi() const
{
	return(_cgi);
}

std::string	Client::getClientIp() const
{
	return (_clientIp);
}

void	Client::setReadHeader(bool b)
{
	_readHeader = b;
}

void	Client::setReadBody(bool b)
{
	_readBody = b;
}

void	Client::setWriteClient(bool b)
{
	_writeClient = b;
}

void	Client::setErrorCode(int c)
{
	_errorCode = c;
}

void	Client::setCgi(CgiProcessor* cgi)
{
	_cgi = cgi;
}

void	Client::setAddrlen(socklen_t addrLen)
{
	_addrLen = addrLen;
}

/******************************************************************************/
/*                               Error Handling                               */
/******************************************************************************/

/******************************************************************************/
/*                            O-Stream Overload                               */
/******************************************************************************/

/******************************************************************************/
/*                          Class Specific Functions                          */
/******************************************************************************/


bool	Client::checkTimeout()
{
	double diff = (static_cast<double>(std::clock() - _start) * 1000) / CLOCKS_PER_SEC;
	if (diff > MAX_TIMEOUT)
		return (false);
	// if (diff > _clockstop) 
	// {
	// 	std::cout << "id: " << _id << ", " << diff / 1000 << " sec" << std::endl;
	// 	_clockstop += 1000;
	// }
	return (true);
}

void		Client::addRecvLineToMessage()
{
	_message += (char *)_recvLine;
}

void	Client::addRecvLineToCgiMessage()
{
	_cgiMessage += (char *)_recvLine;
}

void	Client::clearRecvLine()
{
	memset(_recvLine, 0, MAXLINE);
}

void	Client::clearMessage()
{
}

void Client::createClientHeader()
{
	if(header != NULL)
	{
		return;
	}
	header = new ClientHeader(this->getMessage());
	Logger::info("Client header created with : "); std::cout << _message;
	if(header->getErrorCode() != 0)
	{
		Logger::warning("Client Header have error", false); std::cout << header->getErrorCode() << std::endl;
		setErrorCode(header->getErrorCode());
	}
}

void Client::_initVars(void)
{
	hasWrittenToCgi = false;
	hasReadFromCgi = false;
	socketFromChild = DELETED;
	socketToChild = DELETED;
	waitReturn = 0;
	cgiChecked = false;
	_errorCode = 0;
	_readHeader = true;
	_readBody = false;
	_writeClient = false;
	_recvLine = new unsigned char[MAXLINE];
	memset(_recvLine, 0, MAXLINE);
	header = NULL;
	_response = NULL;
	_clientBody = "";
	_cgiOutput = "";
	Cgi = NULL;
	cgiRunning = true;
	_clockstop = 1000;
}
void	Client::setChildSocket(int to, int from)
{
	socketToChild = to;
	socketFromChild = from;
}

void	Client::unsetsocket_tochild()
{
	socketToChild = DELETED;
}

void	Client::unsetsocket_fromchild()
{
	socketFromChild = DELETED;
}

unsigned short	Client::getClientdPort()
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
