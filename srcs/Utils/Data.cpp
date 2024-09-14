
#include "Data.hpp"	
#include "../Client/Client.hpp"	
#include "../Server/Socket.hpp"	
#include <sys/epoll.h>
#include <vector>

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

int Data::_nfds = 0;
char** Data::_envp = NULL;
int Data::_epollfd = epoll_create(1); // MR_DOUBT: Here we should check if it returned -1.
std::map<int, Client*> emptyClients;
std::map<int, Client*>&	Data::_clients = emptyClients;
std::vector<Socket> *	Data::_serverSockets = NULL; // MR_DOUBT: A pointer to a vector? Or a vector of pointers?
struct epoll_event	Data::_events[MAX_EVENTS];
std::map<std::string, std::string> emptyLanguages;
std::map<std::string, std::string> Data::_cgiLang = emptyLanguages;

int	Data::getEpollFd()
{
	return (_epollfd);
}

std::map<int, Client*> &	Data::getClients()
{
	return (_clients);
}

const std::vector<Socket> &		Data::getServerSockets()
{
	return (*_serverSockets);// MR_DOUBT: Can _serverSockets be NULL? If yes, possible segfault.
}

// MR_DOUBT: Can _serverSockets be NULL? If yes, possible segfault.
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

bool Data::isCgiExtensionValid(std::string extension)
{
	if(_cgiLang.find(extension) == _cgiLang.end())
	{
		return false;
	}
	return true;
}

void	Data::setEnvp(char** envvar)
{
	_envp = envvar; //MR_TODO: Here we could check if env is NULL, and throw an exception.
}

// MR_NOTE: If env is NULL (e.g: env -i), we could segfault
std::string	Data::findStringInEnvp(std::string str)
{
	std::string tmp;
	std::string substr;
	std::size_t found;
	for (size_t i = 0; _envp[i]; i++)
	{
		tmp = _envp[i];
		found = tmp.find(str);
  		if (found == 0)
			break;
	}
	if (found == std::string::npos)
		return (tmp = "");
	found = tmp.find("=");
	if(found != std::string::npos)
		substr = tmp.substr(found + 1);
	return (substr);
}

//MR_DOUBT: This is a setter or a getter?
struct epoll_event*		Data::setEvents() 
{
	return (_events);
}

// MR_DOUBT: Can Client be NULL? If yes, possible segfault.
void	Data::closeAllFds()
{
	// closing EpollFD
	close(_epollfd);

	// closing all Client Fds and all Sockets to possible Child Processes
	std::map<int, Client*>::iterator it = _clients.begin();
	for(; it != _clients.end(); it++)
	{
		close(it->second->getFd());
		if (it->second->socketToChild != DELETED)
			close(it->second->socketToChild);
		if (it->second->socketFromChild != DELETED)
			close(it->second->socketFromChild);
	}

	// closing ServerSockets
	std::vector<int>SocketFds = getServerSocketFds();
	std::vector<int>::const_iterator itv = SocketFds.begin();
	for (; itv != SocketFds.end(); itv++)
		close(*itv);
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

const std::map<std::string, std::string>&	Data::getCgiLang()
{
	return (_cgiLang);
}

void	Data::setCgiLang(std::string suffix, std::string interpreter)
{
	_cgiLang[suffix] = interpreter;	
}

void	Data::setAllCgiLang()
{
	Data::setCgiLang(".py", "python3");
}

void	Data::setServerSockets(std::vector<Socket> * sockets)
{
	_serverSockets = sockets;
}

//============================================================================
//FIXME:================FUNTIONS FOR TESTING OR NOT BEING USED================
//============================================================================
/* 
// MR_DOUBT: What is "int" in _clients? (a fd? or just an id?). Cuz we could do "_clients[fd]"
// MR_NOTE: This function is not being used.
const Client*	Data::getClientByFd(int fd)
{
	std::map<int, Client*>::iterator it = _clients.begin();
	for(; it != _clients.end(); it++)
	{
		if (it->second->getFd() == fd) // MR_DOUBT: Can Client be NULL? If yes, possible segfault.
			return (it->second);
	}
	return (NULL);
}

// MR_NOTE: This function is not being used.
void	Data::setEpollFd(int fd)
{
	_epollfd = fd;
	if (Data::_epollfd == -1)					
		throw std::runtime_error("epoll create failed");
}
 */