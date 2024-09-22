#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "../Message/RequestHeader.hpp"
// # include "CgiProcessor.hpp"
// # include "../Response/Response.hpp"
// # include "../Utils/Logger.hpp"
// # include "../Server/SocketManager.hpp"
// # include <cstddef>
// # include <cstring>
// # include <iostream>
#include <cstddef>
# include <string>
# include <ctime>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
// # include <sstream>
// # include <map>
//# include "CgiProcessor.hpp"

// # define MAXLINE			4096
// # define MAXLINE			1
# define MAX_TIMEOUT		3000
# define DELETED			-1 

class Message;

class Client
{
	public:
		enum	e_fdState
		{
			NONE, // Initial state of fd
			R_RECEIVE, // fd ready to be read
			R_SEND, // fd ready to be writted to
			R_SENDREC, // fd ready to be written to and read from
			CLOSE, // close fd
			CLOSED // fd is closed
		};

		enum	e_clientState
		{
			NEW,	// Client is new and ready to read it's request message
			F_REQUEST,	// Client has finished reading Request
			F_CGIWRITE,	// Client finished writing to CGI
			F_CGIREAD,	// Client finished reading to CGI 
			F_RESPONSE,	// Client has finished writing Response
			RESETME,	// reserved for keep alive option. This should trigger deleting of Messages
			DELETEME	// Client wants to be deleted
		};

		enum	e_clientFdType
		{
			CLIENT_FD = 0,
			TOCHILD_FD = 1,
			FROMCHILD_FD = 2
		};

		enum	e_clientMsgType
		{
			REQ_MSG,
			RESP_MSG,
			CGIRESP_MSG
		};

		typedef std::pair<e_clientFdType, e_fdState> fdTypeStatePair;
		typedef std::map<int, fdTypeStatePair > fdPairsMap;

	public:
		// Methods
		Message*				getMsg(e_clientMsgType type);
		unsigned long			getId() const;
	//	int						getClientFd() const;
		int						getFdByType(e_clientFdType type);

		fdPairsMap&				getClientFds(); // Maybe we could come up with a better name.

		const e_clientState&	getClientState() const;
		unsigned short			getClientPort();
		std::string				getClientIp() const;
		std::clock_t			getStartTime() const;
		int&					getErrorCode();
		// Message*				getRequestMsg()const;
		// Message*				getResponseMsg()const;
		// Message*				getCgiResponseMsg()const;
		bool					checkTimeout();
		void					setClientState(e_clientState state);
		void					setClientFdState(int idx, e_fdState fdState);
		void					setRequestMsg(Message* m);
		void					setResponseMsg(Message* m);
		void					setCgiResponseMsg(Message* m);
		void					setErrorCode(int e);
		void					setAddrlen(socklen_t addrlen);
		void					setChildSocket(int in, int out);
		void					closeSocketToChild();
		void					closeSocketFromChild();
		void					closeClientFds();

		// Attributes
		static size_t			client_cntr;
		static std::map<int, Client*>	clients;

	private:
		// Methods
		void				_initClientIp();
		void				_initVars(int fd);

		// Attributes
		const size_t		_id;
		fdPairsMap			_clientFds; // Cliend fd = 0, socketToChild = 1, socketFromChild = 2
		// const int		_fd;
		e_clientState		_clientState;
		// e_fdState		_stateFd;
		// int				_socketToChild;
		// e_fdState		_stateSocketToChild;
		// int				_socketFromChild;
		// e_fdState		_stateSocketFromChild;
		const std::clock_t	_start;
		double				_clockstop;
		int					_errorCode;
		Message*			_requestMsg;	// client owns so it should delete
		Message*			_responseMsg;	// client owns so it should delete
		Message*			_cgiResponseMsg;
		struct sockaddr		_clientAddr;
		std::string			_clientIp;
		socklen_t			_addrLen;

	public:
						Client (int const fd, struct sockaddr client_addr, socklen_t addrlen);
						~Client(void);

	private:
						Client(void);
						Client(Client const & src);
		Client &		operator=(Client const & rhs);
};

#endif
