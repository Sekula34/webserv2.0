
#include "Client.hpp"
#include <cstddef>
#include <cstring>
#include <iostream>
#include "../Utils/Logger.hpp"
#include <unistd.h>


/******************************************************************************/
/*                               Constructors                                 */
/******************************************************************************/
int	Client::client_cntr = 0;

Client::Client (void):_id(++client_cntr), _fd(0), _start(std::clock()), _epollfd(0) 
{
	_initVars();
	// std::cout << "Client default constructor called" << std::endl;
}

Client::Client (int const fd, int const epollfd):_id(++client_cntr), _fd(fd), _start(std::clock()), _epollfd(epollfd)
{
	_initVars();
	Logger::info("Client constructed, unique ID: "); std::cout << _id;
	std::cout << " FD: "; std::cout << _fd << std::endl;
}

/******************************************************************************/
/*                                Destructor                                  */
/******************************************************************************/

Client::~Client (void)
{
	close (_fd);
	delete [] _recvline;
	delete header;
	delete _response;
	Logger::info("Destructed client with ID: "); std::cout << _id << std::endl;
}

/******************************************************************************/
/*                             Copy Constructor                               */
/******************************************************************************/

Client::Client(Client const & src):_id(++client_cntr), _fd(src._fd), _start(std::clock()), _epollfd(src._epollfd)
{
	//std::cout << "Client copy constructor called" << std::endl;
	_recvline = new unsigned char[MAXLINE];
	memset(_recvline, 0, MAXLINE);
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
		_readheader = rhs._readheader;
		_readbody = rhs._readbody;
		_writeclient = rhs._writeclient;
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
		Logger::warning("Setting response in client but client already have one. Possible leak", true);
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

unsigned char*	Client::getRecvLine() const
{
	return (_recvline);
}

int	Client::getEpollFd() const
{
	return (_epollfd);
}
bool	Client::getReadHeader() const
{
	return (_readheader);
}

bool	Client::getReadBody() const
{
	return (_readbody);
}

bool	Client::getWriteClient() const
{
	return (_writeclient);
}

int	Client::getErrorCode() const
{
	return (_errorCode);
}

std::string const &	Client::getBody() const
{
	return (_body);
}

void	Client::setReadHeader(bool b)
{
	_readheader = b;
}

void	Client::setReadBody(bool b)
{
	_readbody = b;
}

void	Client::setWriteClient(bool b)
{
	_writeclient = b;
}

void	Client::setErrorCode(int c)
{
	_errorCode = c;
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


bool	Client::check_timeout() const
{
	if ( ((static_cast<double>(std::clock() - _start) * 1000)
		/ CLOCKS_PER_SEC) > MAX_TIMEOUT)
		return (false);
	return (true);
}

void		Client::addRecvLineToMessage()
{
	_message += (char *)_recvline;
	memset(_recvline, 0, MAXLINE);
}

void Client::createClientHeader()
{
	if(header != NULL)
	{
		Logger::warning("You are trying to create header but this already exist. Could be reason for leak");
		return;
	}
	header = new ClientHeader(this->getMessage());
	Logger::info("Client header created with : "); std::cout << _message;
}

void Client::_initVars(void)
{
	_readheader = true;
	_readbody = false;
	_writeclient = false;
	_recvline = new unsigned char[MAXLINE];
	memset(_recvline, 0, MAXLINE);
	
	header = NULL;
	_response = NULL;
}
