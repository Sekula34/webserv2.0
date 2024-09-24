#ifndef DUMMYCLIENT_HPP
# define DUMMYCLIENT_HPP

// # include "DummyMessage.hpp"
# include <string>
# include <iostream>

class DummyMessage;

class DummyClient
{
	public:
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

		enum	e_clientMsgType
		{
			REQ_MSG,
			RESP_MSG,
			CGIRESP_MSG
		};

	public:
		// Methods
		DummyMessage*			getMsg(e_clientMsgType type);
		unsigned long			getId() const;
		int&					getFd();
		const e_clientState&	getClientState() const;
		unsigned short			getClientPort();
		std::string				getClientIp() const;
		int&					getErrorCode();
		void					setClientState(e_clientState state);
		void					setRequestMsg(DummyMessage* m);
		void					setResponseMsg(DummyMessage* m);
		void					setCgiResponseMsg(DummyMessage* m);
		void					setErrorCode(int e);
		// Attributes

	private:
		// Methods
		// Attributes
		unsigned long	_id;
		int				_fd;
		e_clientState	_clientState;
		int				_errorCode;
		int				_clientAddr;
		std::string		_clientIp;
		int				_addrLen;
		unsigned short	_port;
		DummyMessage*	_requestMsg;
		DummyMessage*	_responseMsg;
		DummyMessage*	_cgiResponseMsg;

	public:
		DummyClient(const int fd, int _clientAddr, int _addrLen);
		// ~DummyClient();

	// Overloaded insertion operator (for testing).
	friend std::ostream&	operator<<(std::ostream& out, const DummyClient::e_clientState& clientState);
	friend std::ostream&	operator<<(std::ostream& out, const DummyClient& client);
};

#endif
