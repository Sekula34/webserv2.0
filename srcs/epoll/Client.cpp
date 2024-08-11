
#include "Client.hpp"
#include <cstring>
#include <iostream>
#include "../Utils/Logger.hpp"

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
	//std::cout << "Client constructed, unique ID: " << _id << " FD: " << _fd << std::endl;
}

/******************************************************************************/
/*                                Destructor                                  */
/******************************************************************************/

Client::~Client (void)
{
	delete [] _recvline;
	std::cout << "Client with ID: " << _id <<  " destructed" << std::endl;
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

void Client::_initVars(void)
{
	_readheader = true;
	_readbody = false;
	_writeclient = false;
	_recvline = new unsigned char[MAXLINE];
	memset(_recvline, 0, MAXLINE);
	
	header = NULL;
}
