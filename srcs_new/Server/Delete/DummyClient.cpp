#include "DummyClient.hpp"
#include "DummyMessage.hpp"
#include <iostream>

//==========================================================================//
// REGULAR METHODS==========================================================//
//==========================================================================//

DummyMessage*			DummyClient::getMsg(e_clientMsgType type)
{
	try
	{
		if (type == REQ_MSG)
		{	
			if (!_requestMsg)
				_requestMsg = new DummyMessage("test", _errorCode);
			return (_requestMsg);
		}
		if (type == RESP_MSG)
		{	
			if (!_responseMsg)
				_responseMsg = new DummyMessage("test", _errorCode);
			return (_responseMsg);
		}
		if (type == CGIRESP_MSG)
		{	
			if (!_cgiResponseMsg)
				_cgiResponseMsg = new DummyMessage("test", _errorCode);
			return (_cgiResponseMsg);
		}
	}
	catch (std::exception& e)
	{
		// Logger::error("F@ck could not create new Message in client: ", _id);
		std::cout << "F@ck could not create new Message in client: " << _id << std::endl;
	}
	return (NULL);
}

unsigned long			DummyClient::getId() const
{
	return (_id);
}

int&					DummyClient::getFd()
{
	return (_fd);
}

const DummyClient::e_clientState&	DummyClient::getClientState() const
{
	return (_clientState);
}

unsigned short			DummyClient::getClientPort()
{
	return (_port);
}

std::string				DummyClient::getClientIp() const
{
	return (_clientIp);
}

int&					DummyClient::getErrorCode()
{
	return (_errorCode);
}

void					DummyClient::setClientState(e_clientState state)
{
	_clientState = state;
}

void					DummyClient::setRequestMsg(DummyMessage* m)
{
	_requestMsg = m;
}

void					DummyClient::setResponseMsg(DummyMessage* m)
{
	_responseMsg = m;
}

void					DummyClient::setCgiResponseMsg(DummyMessage* m)
{
	_cgiResponseMsg = m;
}

void					DummyClient::setErrorCode(int e)
{
	_errorCode = e;
}

//==========================================================================//
// Constructor, Destructor and OCF Parts ===================================//
//==========================================================================//

// Constructor
DummyClient::DummyClient(const int fd, int clientAddr, int addrLen)
: _fd(fd), _clientAddr(clientAddr), _addrLen(addrLen)
{
	_id = 1;
	_clientState = NEW;
	_errorCode = 0;
	_clientIp = std::string("123.456.789");
	_port = 8080;
	_requestMsg = NULL;
	_responseMsg = NULL;
	_cgiResponseMsg = NULL;
}

// Overloaded Insertion Operator.
std::ostream&	operator<<(std::ostream& out, const DummyClient::e_clientState& clientState)
{
	std::string state;
	if (clientState == DummyClient::NEW)
		state = "NEW";
	else if (clientState == DummyClient::F_REQUEST)
		state = "F_REQUEST";
	else if (clientState == DummyClient::F_CGIWRITE)
		state = "F_CGIWRITE";
	else if (clientState == DummyClient::F_CGIREAD)
		state = "F_CGIREAD";
	else if (clientState == DummyClient::F_RESPONSE)
		state = "F_RESPONSE";
	else if (clientState == DummyClient::RESETME)
		state = "RESETME";
	else if (clientState == DummyClient::DELETEME)
		state = "DELETEME";
	else
	 	state = "Unknown";

	out << "_clientState:" << state;
	return (out);
}

// Overloaded Insertion Operator (For testing).
std::ostream&	operator<<(std::ostream& out, const DummyClient& client)
{
	out << "CLIENT INFO: ";
	out << "--id:" << client._id;
	out << "--fd:" << client._fd;
	out << "--"<< client._clientState;
	out << "--errorCode:" << client._errorCode;
	out << "--Addr:" << client._clientAddr;
	out << "--IP:" << client._clientIp;
	out << "--AddrLen:" << client._addrLen;
	out << "--port:" << client._port;
	return (out);
}

// std::cout << "-----------------DEBUGGING-----------------" << std::endl;
