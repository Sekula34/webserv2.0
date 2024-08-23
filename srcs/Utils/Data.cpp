
#include "Data.hpp"	
#include "../epoll/Client.hpp"	
#include "../Server/Socket.hpp"	
#include <sys/epoll.h>

/******************************************************************************/
/*                               Constructors                                 */
/******************************************************************************/

Data::Data (void)
{
	std::cout << "Data default constructor called" << std::endl;
}

/******************************************************************************/
/*                                Destructor                                  */
/******************************************************************************/

Data::~Data (void)
{
	std::cout << "Data destructor called" << std::endl;
}

/******************************************************************************/
/*                             Copy Constructor                               */
/******************************************************************************/

Data::Data(Data const & src)
{
	//std::cout << "Data copy constructor called" << std::endl;
	*this = src;
}

/******************************************************************************/
/*                      Copy Assignment Operator Overload                     */
/******************************************************************************/

Data &	Data::operator=(Data const & rhs)
{
	//std::cout << "Data Copy assignment operator called" << std::endl;
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

int Data::_nfds = 0;
char** Data::envp = NULL;
int Data::_epollfd = epoll_create(1);
std::map<int, Client*> emptyClients;
std::map<int, Client*>&	Data::_clients = emptyClients;
std::vector<Socket> *	Data::_serverSockets = NULL;
struct epoll_event	Data::_events[MAX_EVENTS];


int	Data::getEpollFd()
{
	return (_epollfd);
}

std::map<int, Client*> &	Data::getClients()
{
	return (_clients);
}

const Client*	Data::getClientByFd(int fd)
{
	std::map<int, Client*>::iterator it = _clients.begin();
	for(; it != _clients.end(); it++)
	{
		if (it->second->getFd() == fd)
			return (it->second);
	}
	return (NULL);
}

const std::vector<Socket> &		Data::getServerSockets()
{
	return (*_serverSockets);
}

const std::vector<int> 		Data::getServerSocketFds()
{
	std::vector<int> listenFds;
	for(size_t i = 0; i < _serverSockets->size(); i++)
	{
		int oneFd = (*_serverSockets)[i].getSocketFd();
		listenFds.push_back(oneFd);
	}
	return listenFds;
}
int &	Data::getNfds()
{
	return (_nfds);
}

struct epoll_event*		Data::setEvents() 
{
	return (_events);
}

void	Data::closeAllFds()
{
}

void	Data::epollAddFd(int fd)
{
	// STRUCT NEEDED FOR EPOLL TO SAVE FLAGS INTO (SETTINGS)
	struct epoll_event	ev;

	// SETTING UP EV EVENTS 'SETTINGS' STRUCT FOR LISTEN_SOCKET
	ev.events = EPOLLIN | EPOLLOUT;
	ev.data.fd = fd;

	// ADDING LISTEN_SOCKET TO EPOLL WITH THE EV 'SETTINGS' STRUCT
	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &ev) == -1)
		throw std::runtime_error("epoll_ctl error: adding file descriptor to epoll failed");
}

void	Data::epollRemoveFd(int fd)
{
	// REMOVE THE FD OF THIS CLIENT INSTANCE FROM EPOLLS WATCH LIST
	if (epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, _events) == -1)
		throw std::runtime_error("epoll_ctl error: removing file descriptor from epoll failed");
}
void	Data::setEpollFd(int fd)
{
	_epollfd = fd;
	if (Data::_epollfd == -1)					
		throw std::runtime_error("epoll create failed");
}
void	Data::setServerSockets(std::vector<Socket> * sockets)
{
	_serverSockets = sockets;
}
