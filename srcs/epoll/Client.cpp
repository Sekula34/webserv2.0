
#include "Client.hpp"

/******************************************************************************/
/*                               Constructors                                 */
/******************************************************************************/
int	Client::client_cntr = 0;

Client::Client (void):_id(++client_cntr), _fd(0), _write(true), _start(std::clock())
{
	// std::cout << "Client default constructor called" << std::endl;
}

Client::Client (int const fd):_id(++client_cntr), _fd(fd), _write(true), _start(std::clock())
{
	// std::cout << "Client default constructor called" << std::endl;
}

/******************************************************************************/
/*                                Destructor                                  */
/******************************************************************************/

Client::~Client (void)
{
	// std::cout << "Client destructor called" << std::endl;
}

/******************************************************************************/
/*                             Copy Constructor                               */
/******************************************************************************/

Client::Client(Client const & src):_id(++client_cntr), _fd(src._fd), _start(std::clock())
{
	//std::cout << "Client copy constructor called" << std::endl;
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

/******************************************************************************/
/*                               Error Handling                               */
/******************************************************************************/

/******************************************************************************/
/*                            O-Stream Overload                               */
/******************************************************************************/

/******************************************************************************/
/*                          Class Specific Functions                          */
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

bool	Client::check_timeout(std::clock_t time) const
{
	double	duration = (double(time - _start) * 1000) / CLOCKS_PER_SEC;
	if ( duration> MAX_TIMEOUT)
		return (false);
	return (true);
}

std::string	Client::getMessage() const
{
	return (_message);
}

void		Client::addToMessage(char* buffer)
{
	_message += buffer;
}

void		Client::setNoWrite()
{
	_write = false;
}
