#ifndef CLIENT_HPP
# define CLIENT_HPP
# include <string>
# include <ctime>
# include "../Client/RequestHeader.hpp"
# include "CgiProcessor.hpp"
# include "../Response/Response.hpp"
# include "../Utils/Logger.hpp"
# include "../Server/SocketManager.hpp"
# include <cstddef>
# include <cstring>
# include <iostream>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sstream>
# include <map>
//# include "CgiProcessor.hpp"

# define MAXLINE			4096
// # define MAXLINE			1
# define MAX_TIMEOUT		3000
# define DELETED			-1 

class CgiProcessor;
class Message;

class Client {

	public:
		static int			client_cntr;
		
							// canonical
							Client (int const fd, struct sockaddr client_addr, socklen_t addrlen);
							~Client(void);
							
							// set and get
		Response* 			getResponse() const;
		void				setResponse(Response* respose);
		unsigned long		getId() const;
		int					getFd() const;
		std::clock_t		getStartTime() const;
		std::string			getMessage() const;
		std::string			getCgiMessage() const;
		char*				getRecvLine() const;
		int					getEpollFd() const;
		int					getErrorCode() const;
		bool				getReadHeader() const;
		bool				getReadBody() const;
		bool				getWriteClient() const;
		std::string const &	getClientBody() const;
		CgiProcessor*		getCgi() const;
		std::string			getClientIp() const;
		unsigned short		getClientdPort();
		Client*				getClient()const;
		Message*			getClientMsg()const;
		Message*			getServerMsg()const;
		void				setClientMsg(Message* m);
		void				setServerMsg(Message* m);
		void				setErrorCode(int e);
		void				setReadHeader(bool b);
		void				setReadBody(bool b);
		void				setWriteClient(bool b);
		void				setCgi(CgiProcessor* cgi);
		void				setAddrlen(socklen_t addrlen);
		void				setChildSocket(int in, int out);
		void 				appendClientBody(std::string s);

							//Client specific functions
		void				unsetsocket_tochild();
		void				unsetsocket_fromchild();
		void				addRecvLineToMessage();
		void				addRecvLineToCgiMessage();
		bool				checkTimeout();
		void				resetChildSocketInMap(int fd);
		void				clearMessage();
		void				clearRecvLine();

		/**
		 * @brief Create a Client Header that is stored in _header and should be deleted in destructor
		 * 
		 */
		void				createRequestHeader();
		CgiProcessor*		Cgi;			
		std::string			_cgiOutput;
		bool				cgiChecked;
		// RequestHeader*		header; //client Responsible for deleting
		pid_t				waitReturn;
		int					socketToChild;
		int					socketFromChild;
		bool				hasWrittenToCgi;
		bool				hasReadFromCgi;
		bool				cgiRunning;

	private:
		int					_errorCode;
		unsigned long const	_id;
		int const			_fd;
		std::clock_t const	_start;
		int const			_epollFd;
		char* 				_recvLine;
		CgiProcessor*		_cgi;
		Message*			_clientMsg;	// client owns so it should delete
		Message*			_serverMsg;	// client owns so it should delete
		Response*			_response; // client owns so it should delete
		struct sockaddr		_clientAddr;
		std::string			_clientIp;
		socklen_t			_addrLen;
		double				_clockstop;
		bool				_writeClient;
							Client(void);
							Client(Client const & src);
		Client &			operator=(Client const & rhs);
		void				_init_user_info();
		void				_initVars(void);
};

#endif
