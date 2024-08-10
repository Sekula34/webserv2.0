
#include "Client.hpp"

/******************************************************************************/
/*                               Constructors                                 */
/******************************************************************************/
int	Client::client_cntr = 0;

Client::Client (void):_id(++client_cntr), _fd(0), _start(std::clock()), _epollfd(0), _write(true)
{
	_recvline = new uint8_t[MAXLINE];
	memset(_recvline, 0, MAXLINE);
	// std::cout << "Client default constructor called" << std::endl;
}

Client::Client (int const fd, int const epollfd):_id(++client_cntr), _fd(fd), _start(std::clock()), _epollfd(epollfd), _write(true)
{
	_recvline = new uint8_t[MAXLINE];
	memset(_recvline, 0, MAXLINE);
	// std::cout << "Client default constructor called" << std::endl;
}

/******************************************************************************/
/*                                Destructor                                  */
/******************************************************************************/

Client::~Client (void)
{
	delete _recvline;
	// std::cout << "Client destructor called" << std::endl;
}

/******************************************************************************/
/*                             Copy Constructor                               */
/******************************************************************************/

Client::Client(Client const & src):_id(++client_cntr), _fd(src._fd), _start(std::clock()), _epollfd(src._epollfd)
{
	//std::cout << "Client copy constructor called" << std::endl;
	_recvline = new uint8_t[MAXLINE];
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
		_write = rhs._write;
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

uint8_t*	Client::getRecvLine() const
{
	return (_recvline);
}

int	Client::getEpollFd() const
{
	return (_epollfd);
}

void		Client::setNoWrite()
{
	_write = false;
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
