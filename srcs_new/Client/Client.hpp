#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "FdData.hpp"
# include <cstddef>
# include <string>
# include <ctime>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <vector>
# include <map>

# define MAX_TIMEOUT		3000

class Message;

class Client
{
	public:

		enum	e_clientState
		{
			DO_REQUEST,	// Client should or does already read the Client request
			DO_CGIREC,	// Client should or does already receive from CGI 
			DO_CGISEND,	// Client should or does already send to CGI
			DO_RESPONSE,// Client should or does already send response Response
			RESETME,	// reserved for keep alive option. This should trigger deleting of Messages
			DELETEME	// Client wants to be deleted
		};

		enum	e_clientMsgType
		{
			REQ_MSG,
			RESP_MSG,
			CGIRESP_MSG
		};

	public:
		// Methods
		Message*				getMsg(e_clientMsgType type);
		unsigned long			getId() const;
		FdData&					getFdDataByType(FdData::e_fdType type);
		FdData&					getFdDataByFd(int fd);
		std::vector<FdData>&	getClientFds(); // Maybe we could come up with a better name.
		const e_clientState&	getClientState() const;
		unsigned short			getClientPort();
		std::string				getClientIp() const;
		std::clock_t			getStartTime() const;
		int&					getErrorCode();
		bool					checkTimeout();
		void					setClientState(e_clientState state);
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
		static size_t					client_cntr;
		static std::map<int, Client*>	clients;

	private:
		// Methods
		void				_initClientIp();
		void				_initVars(int fd);

		// Attributes
		const size_t		_id;
		std::vector<FdData>	_clientFds; // Cliend fd = 0, socketToChild = 1, socketFromChild = 2
		e_clientState		_clientState;
		const std::clock_t	_start;
		int					_errorCode;
		Message*			_requestMsg;	// client owns so it should delete
		Message*			_responseMsg;	// client owns so it should delete
		Message*			_cgiResponseMsg;
		struct sockaddr		_clientAddr;
		std::string			_clientIp;
		socklen_t			_addrLen;
		// double				_clockstop;

	public:
						Client (int const fd, struct sockaddr client_addr, socklen_t addrlen);
						~Client(void);

	private:
						Client(void);
						Client(Client const & src);
		Client &		operator=(Client const & rhs);
};

#endif
