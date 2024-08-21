
#include "Client.hpp"
#include <cstddef>
#include <cstring>
#include <iostream>
#include "../Utils/Logger.hpp"
#include <sstream>


/******************************************************************************/
/*                               Constructors                                 */
/******************************************************************************/
int	Client::client_cntr = 0;

Client::Client (void):  allSockets(NULL), _id(0), _fd(0), _start(std::clock()), _epollfd(0)
{ 	
	// std::cout << "Client default constructor called" << std::endl;
}

Client::Client (int const fd, int const epollfd, std::map<int, Client*> * child_sockets, struct sockaddr client_addr, std::vector<Socket>* allSockets): allSockets(allSockets), _id(++client_cntr),
					_fd(fd), _start(std::clock()), _epollfd(epollfd), _child_sockets(child_sockets), _cgi(NULL), _client_addr(client_addr)
{
	// we will need to do new cgi somewhere else, this is just for testing


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
	close (_fd);
	if (socketstatus_tochild != DELETED)
		close(socket_tochild);
	if (socketstatus_fromchild != DELETED)
		close(socket_fromchild);
	delete [] _recvline;
	delete header;
	delete _response;
	delete _cgi;
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

std::string	Client::getCgiMessage() const
{
	return (_cgimessage);
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

std::string const &	Client::getClientBody() const
{
	return (_client_body);
}

CgiProcessor*	Client::getCgi() const
{
	return(_cgi);
}

std::string	Client::getClientIp() const
{
	return (_client_ip);
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

void	Client::setCgi(CgiProcessor* cgi)
{
	_cgi = cgi;
}

void	Client::setAddrlen(socklen_t addrlen)
{
	_addrlen = addrlen;
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
}

void	Client::addRecvLineToCgiMessage()
{
	_cgimessage += (char *)_recvline;
}

void	Client::clearRecvLine()
{
	memset(_recvline, 0, MAXLINE);
}

void	Client::clearMessage()
{
	// _message.clear();
	// _message = "";
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
	socketstatus_fromchild = NONE;
	socketstatus_tochild = NONE;
	socket_fromchild = -1;
	socket_tochild = -1;
	waitreturn = 0;
	cgi_checked = false;
	_errorCode = 0;
	_readheader = true;
	_readbody = false;
	_writeclient = false;
	_recvline = new unsigned char[MAXLINE];
	memset(_recvline, 0, MAXLINE);
	header = NULL;
	_response = NULL;
	_client_body = "";
	_cgi_output = "";
	Cgi = NULL;
}
void	Client::setChildSocket(int to, int from)
{
	socketstatus_tochild = ADD;
	socketstatus_fromchild = ADD;
	socket_tochild = to;
	socket_fromchild = from;
}

void	Client::unsetsocket_tochild()
{
	socketstatus_tochild = DELETE;
}

void	Client::unsetsocket_fromchild()
{
	socketstatus_fromchild = DELETE;
}

void	Client::_init_user_info()
{
	std::string address;
	std::stringstream ss;

	struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&_client_addr;
	unsigned long num  = pV4Addr->sin_addr.s_addr;
	num = ntohl(num);
	ss << int((num&0xFF000000)>>24);
	ss << ".";
	ss << int((num&0xFF0000)>>16);
	ss << ".";
	ss << int((num&0xFF00)>>8);
	ss << ".";
	ss << int(num&0xFF);
	_client_ip = ss.str();
}
