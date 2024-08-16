
#include "EpollHandler.hpp"

/******************************************************************************/
/*                               Constructors                                 */
/******************************************************************************/

EpollHandler::EpollHandler (void)
{
	//std::cout << "EpollHandler default constructor called" << std::endl;
}

/******************************************************************************/
/*                                Destructor                                  */
/******************************************************************************/

EpollHandler::~EpollHandler (void)
{
	//std::cout << "EpollHandler destructor called" << std::endl;
}

/******************************************************************************/
/*                             Copy Constructor                               */
/******************************************************************************/

EpollHandler::EpollHandler(EpollHandler const & src)
{
	//std::cout << "EpollHandler copy constructor called" << std::endl;
	*this = src;
}

/******************************************************************************/
/*                      Copy Assignment Operator Overload                     */
/******************************************************************************/

EpollHandler &	EpollHandler::operator=(EpollHandler const & rhs)
{
	//std::cout << "EpollHandler Copy assignment operator called" << std::endl;
	if (this != &rhs)
	{
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

void	EpollHandler::epoll_add_fd(int epollfd, int clientfd)
{
	// STRUCT NEEDED FOR EPOLL TO SAVE FLAGS INTO (SETTINGS)
	struct epoll_event	ev;

	// SETTING UP EV EVENTS 'SETTINGS' STRUCT FOR LISTEN_SOCKET
	ev.events = EPOLLIN | EPOLLOUT;
	ev.data.fd = clientfd;

	// ADDING LISTEN_SOCKET TO EPOLL WITH THE EV 'SETTINGS' STRUCT
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev) == -1)
		throw std::runtime_error("epoll_ctl - conn_socket error");
}
