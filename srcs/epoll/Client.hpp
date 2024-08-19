

#ifndef CLIENT_HPP
# define CLIENT_HPP
# include <string>
# include <ctime>
# include "../Client/ClientHeader.hpp"
# include "../epoll/CgiProcessor.hpp"
#include <cstddef>
#include <cstring>
#include <iostream>
#include "../Utils/Logger.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include <map>
# include "../Response/Response.hpp"
//# include "CgiProcessor.hpp"

// #define MAXLINE			4096
// #define MAXLINE			493
# define MAXLINE			50000
# define MAX_TIMEOUT		10000
#define NONE			0 
#define ADD				1 
#define DELETE			2 
#define TOBEDELETE		3 

class CgiProcessor;


class Client {

	public:
		static int			client_cntr;
		
							// canonical
							Client (int const fd, int const epollfd, std::map<int,
			   						Client*>* child_sockets, struct sockaddr client_addr);
							~Client(void);
							
							// set and get
		Response* 			getResponse() const;
		void				setResponse(Response* respose);
		unsigned long		getId() const;
		int					getFd() const;
		std::clock_t		getStartTime() const;
		std::string			getMessage() const;
		unsigned char*		getRecvLine() const;
		int					getEpollFd() const;
		int					getErrorCode() const;
		bool				getReadHeader() const;
		bool				getReadBody() const;
		bool				getWriteClient() const;
		std::string const &	getClientBody() const;
		CgiProcessor*		getCgi() const;
		std::string			getClientIp() const;
		void				setErrorCode(int e);
		void				setReadHeader(bool b);
		void				setReadBody(bool b);
		void				setWriteClient(bool b);
		void				setCgi(CgiProcessor* cgi);
		void				setAddrlen(socklen_t addrlen);
		void				setChildSocket(int fd);
		void				unsetChildSocket();

							//Client specific functions
		void				addRecvLineToMessage();
		bool				check_timeout() const;
		void				resetChildSocketInMap(int fd);
		void				clearMessage();
		void				clearRecvLine();

		/**
		 * @brief Create a Client Header that is stored in _header and should be deleted in destructor
		 * 
		 */
		void				createClientHeader();
		CgiProcessor*		Cgi;			
		bool				cgi_checked;
		ClientHeader*		header; //client Responsible for deleting
		pid_t				waitreturn;
		int					childSocketStatus;
		int					childSocket;

	private:
		int					_errorCode;
		unsigned long const	_id;
		int const			_fd;
		std::clock_t const	_start;
		int const			_epollfd;
		std::string			_message;
		std::string			_client_body; //maybe this will be replaced by body class
		std::string			_response_body; // this will be replaced by respnse class
		unsigned char*		_recvline;
		bool				_readheader;
		bool				_readbody;
		bool				_writeclient;
		std::map<int, Client*>*	_child_sockets;
		CgiProcessor*		_cgi;
		Response*			_response; // client owns so it should delete
		struct sockaddr		_client_addr;
							Client(void);
							Client(Client const & src);
		Client &			operator=(Client const & rhs);
		std::string			_client_ip;
		socklen_t			_addrlen;

		void				_init_user_info();
		void				_initVars(void);
};

#endif
