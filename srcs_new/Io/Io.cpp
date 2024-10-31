# include "Io.hpp"
# include "../Client/Client.hpp"
# include "../Message/Message.hpp"
# include "../Message/Node.hpp"
# include "../Utils/Logger.hpp"
# include "../Message/RequestHeader.hpp"
# include <unistd.h>


//==========================================================================//
// STATIC METHODS===========================================================//
//==========================================================================//


// select the correct Fd type and the correct message that we want to do io with
// the selection happens based on the client state
// if we want to receive the Client request, we need the ClientFd and the request message
static Message*	setFdTypeAndMsg(Client& client, FdData::e_fdType& fdType)
{
	Message* message = NULL;
	if (client.getClientState() == Client::DO_REQUEST)
	{
		fdType = FdData::CLIENT_FD;
		message = client.getMsg(Client::REQ_MSG);
	}
	else if (client.getClientState() == Client::DO_CGISEND)
	{
		fdType = FdData::TOCHILD_FD;
		message = client.getMsg(Client::REQ_MSG);
	}
	else if (client.getClientState() == Client::DO_CGIREC)
	{
		fdType = FdData::FROMCHILD_FD;
		// message = client.getMsg(Client::CGIRESP_MSG);
		message = client.getMsg(Client::RESP_MSG);
	}
	else if (client.getClientState() == Client::DO_RESPONSE)
	{
		fdType = FdData::CLIENT_FD;
		message = client.getMsg(Client::RESP_MSG);
	}
	return message;
}

static void	setFinishedSending(Client& client, FdData& fdData, int error)
{
	if (fdData.type == FdData::CLIENT_FD)
		client.setClientState(Client::DELETEME);
	else if (fdData.type == FdData::TOCHILD_FD)
	{
		if (error == 0)
			client.setClientState(Client::DO_CGIREC);
		// else
		// 	client.setClientState(Client::DO_RESPONSE);
	}
	fdData.state = FdData::CLOSE;
}

static void	setFinishedReceiving(Client& client, FdData& fdData, Message* message)
{
	// PRINT THE MESSAGE THAT WE ARE RECEIVING TO THE CLIENT
	message->printChain();	

	if (fdData.type == FdData::FROMCHILD_FD)
	{
		client.setClientState(Client::DO_RESPONSE);
		fdData.state = FdData::CLOSE;
	}

	// CLIENT CLOSES CONNECTION WITHOUT SENDING ANYTHING. USUALLY BROWSER
	if (fdData.type == FdData::CLIENT_FD && message->getChain().begin()->getStringUnchunked().empty())
	{
		client.setClientState(Client::DELETEME);
		return ;
	}

	// IF MESSAGE OR ITS HEADER IS NOT COMPLETE, FINISH HEADER, SET MESSAGE AS COMPLETE
	if (!message->getHeader())
	{
		try
		{
			// Logger::info("Successfully received bytes: ", recValue);
			message->createHeader();
		}
		catch(std::bad_alloc& a)
		{
			Logger::error("in create Cgi Header: ", a.what());
			// client.setClientState(Client::CRITICAL_ERROR);
			client.setClientState(Client::DELETEME);
		}
	}
	message->getChain().begin()->setState(COMPLETE);
	message->setState(COMPLETE);
	message->resetIterator();
}

void	clearBuffer(char* buffer)
{
	if (!buffer)
		return ;
	for (size_t i = 0; i < MAXLINE; ++i)
		buffer[i] = 0;
}

// Helper function for _sendMsg
// static void		skipHeader(Message* message)
// {
// 	RequestHeader& header = *static_cast<RequestHeader*>(message->getHeader());
// 	std::string method = header.getRequestLine().requestMethod;
// 	if (method == "POST")
// 	{
// 		// skip the header since we need to send the body.
// 		message->advanceIterator();
// 	}
// }

// Method to send messages.
// This function doesnt differentiate if its a Browser or if it's a cgi script.
void	Io::_sendMsg(Client& client, FdData& fdData, Message* message)
{
 	int sendValue = 0;
	if (message->getState() != COMPLETE)
		return ;
	if (client.getCgiFlag() == true && client.getWaitReturn() != 0 && client.getClientState() == Client::DO_RESPONSE)
		return ;
	const std::list<Node>::iterator& it = message->getIterator();
	// Skipp message header if its post request to cgi
	if (fdData.type == FdData::TOCHILD_FD && it->getType() == HEADER)
	{
 		message->advanceIterator();
		// skipHeader(message);
	}

	if (it != message->getChain().end())
		sendValue = send(fdData.fd, it->getString().c_str() + message->getBytesSent(), it->getString().size() - message->getBytesSent(), MSG_DONTWAIT | MSG_NOSIGNAL);
	// sendValue = send(fdData.fd, messageStr.c_str() + message->getBytesSent(), 1, MSG_DONTWAIT | MSG_NOSIGNAL);

	// START TESTING
	// std::string whichFd;
	// if (fdData.type == FdData::CLIENT_FD)
	// 	whichFd = "Sent to Client";
	// else if (fdData.type == FdData::TOCHILD_FD || fdData.type == FdData::FROMCHILD_FD)
	// 	whichFd = "Sent to Child";
	// whichFd += ": ---------------- This is what is being sent ---------------- Bytes: ";
	// Logger::warning(whichFd, sendValue);
	// Logger::chars(it->getString().substr(message->getBytesSent(), sendValue), 3);
	// END TESTING

	if (sendValue > 0)
	{
		// Logger::info("Succesfully sent bytes:", sendValue);
		// Logger::chars(it->getString(), true);
		message->setBytesSent(message->getBytesSent() + sendValue);
		if (message->getBytesSent() == it->getString().size())
		{
			message->advanceIterator();
			message->setBytesSent(0);
		}
	}

	// RETURN IF FULL MESSAGE COULD NOT BE SENT YET
	if (it != message->getChain().end() && message->getBytesSent() < it->getString().size() && sendValue > 0)
		return ;

	// IF UNABLE TO SEND FULL MESSAGE, LOG ERR AND SET ERR CODE
	if (sendValue < 0 || (it != message->getChain().end() && sendValue == 0))
	{
		Logger::error("failed to send message String in Client with ID:", client.getId());
		if (sendValue == 0)
			client.setErrorCode(500);
		else
		{
			client.setClientState(Client::DELETEME);
			return ;
		}
	}
	// PRINT THE MESSAGE THAT WE ARE SENDING TO THE CLIENT
	message->printChain();	

	setFinishedSending(client, fdData, client.getErrorCode());
}

void	Io::_receiveMsg(Client& client, FdData& fdData, Message* message)
{
	int 	recValue = 0;

	clearBuffer(_buffer);
		recValue = recv(fdData.fd, _buffer, MAXLINE, MSG_DONTWAIT | MSG_NOSIGNAL);

	// START TESTING
	// std::string whichFd;
	// if (fdData.type == FdData::CLIENT_FD)
	// 	whichFd = "Recv from Client";
	// else if (fdData.type == FdData::TOCHILD_FD || fdData.type == FdData::FROMCHILD_FD)
	// 	whichFd = "Recv from Child";
	// whichFd += ": -------------- This is what is being received -------------- Bytes: ";
	// Logger::warning(whichFd, recValue);
	// Logger::chars(_buffer, 3);
	// END TESTING

	// SUCCESSFUL READ -> CONCAT MESSAGE
	if (recValue > 0)
	{

		// std::cout << "read " << recValue << "bytes: " << _buffer <<  std::endl;
		try
		{
			// Logger::info("Successfully received bytes: ", recValue);
			message->bufferToNodes(_buffer, recValue);
		}
		catch(std::bad_alloc& a)
		{
			Logger::error("in receiveMsg: ", a.what());
			if (client.getClientState() == Client::DO_CGIREC) 
				client.setClientState(Client::CRITICAL_ERROR);
			else
				client.setClientState(Client::DELETEME);
		}
	}

	if (client.getCgiFlag() == true && client.getWaitReturn() == 0)
		return ;

	// if (client.getWaitReturn() != 0 && client.getClientState() == Client::RESETME)
	// {
	// 	client.setClientState(Client::DELETEME);
	// 	return ;
	// }

	// FINISHED READING because either complete message, or connection was shutdown
	if (recValue <= 0 || message->getState() == COMPLETE)
	{
		// Logger::warning("stopping receiving with recValue: ", recValue);
		if (recValue < 0)
		{
			// client.setErrorCode(500);
			client.setClientState(Client::DELETEME);
		}
		setFinishedReceiving(client, fdData, message);
	}
}

static void	checkHeaderBeforeComplete(Client& client, FdData& fdData, Message* message)
{
	std::list<Node>::iterator it = message->getIterator();
	// if (client.getClientState() != Client::DO_REQUEST || it->getType() != HEADER
	// 	|| it->getState() != INCOMPLETE || client.getClientState() == Client::DELETEME)
	if (client.getClientState() != Client::DO_REQUEST || it->getType() != HEADER
		|| it->getState() != INCOMPLETE)
		return ;
	const std::string& fullStr = it->getStringUnchunked();
	std::string newLineStr = "";
		size_t pos = fullStr.rfind("\r\n");
	if ( pos != std::string::npos)
	{
		newLineStr = fullStr.substr(0, pos + 2);
		// RequestHeader requestHeader(newLineStr + "\r\n", errorCode);
		if (RequestHeader::checkHeaderByLine(newLineStr) == false)
		{
			client.setClientState(Client::DO_RESPONSE);
			client.setErrorCode(400);
			setFinishedReceiving(client, fdData, message);
		}
	}
}

// static void aggressiveReceiveCheck(Client& client, FdData& fdData, Message* message)
// {
// 	std::list<Node>::iterator it = message->getIterator();
// 	if (client.getClientState() == Client::DO_REQUEST
// 		&& it->getType() == HEADER
// 		&& it->getState() == INCOMPLETE
// 		&& !it->getStringUnchunked().empty()
// 		&& fdData.type == FdData::CLIENT_FD
// 		&& !(fdData.state == FdData::R_RECEIVE || fdData.state == FdData::R_SENDREC))
// 	{
// 		client.setClientState(Client::DO_RESPONSE);
// 		client.setErrorCode(400);
// 		setFinishedReceiving(client, fdData, message);
// 	}
// }

void	Io::_ioClient(Client& client)
{
	if (client.getClientState() == Client::DELETEME)
		return ;
	FdData::e_fdType fdType;
	Message* message = setFdTypeAndMsg(client, fdType);

	// if (!message || (fdType == FdData::TOCHILD_FD || fdType == FdData::FROMCHILD_FD)
	if (!message)
	{
		client.setClientState(Client::DELETEME);
		return ;
	}
	
	// SELECTING CORRECT FDDATA INSTANCE IN CLIENT
	FdData& fdData = client.getFdDataByType(fdType);
	// 	fdType << ", size of fds: " << client.getClientFds().size() <<
	// 	", fd: "<< fdData.fd << " fd state: "<< fdData.state <<  std::endl;
	
	if ((client.getClientState() == Client::DO_REQUEST
		|| client.getClientState() == Client::DO_CGIREC)
		&&(fdData.state  == FdData::R_RECEIVE
		|| fdData.state  == FdData::R_SENDREC))
	{
			if (message->getChain().begin()->getState() == COMPLETE && client.getIsRequestChecked() != true)
				return ;
			_receiveMsg(client, fdData, message);
	}

	else if ((client.getClientState() == Client::DO_RESPONSE
		|| client.getClientState() == Client::DO_CGISEND)
		&&(fdData.state  == FdData::R_SEND
		|| fdData.state  == FdData::R_SENDREC))
	{
		_sendMsg(client, fdData, message);
	}

	// CHECK THE HEADER OF CLIENT BEFORE IT IS COMPLETE FOR ERRORS (catch invalid short header)
	checkHeaderBeforeComplete(client, fdData, message);

	// IF AFTER FIRST SUCCESSFUL RECEIVING OF THE REQUEST MESSAGE THERE IS A TIME WHEN WE CAN'T READ FROME
	// CLIENT AND THE MESSAGE HEADER IS INCOMPLETE, THERE IS AN ERR.
	// aggressiveReceiveCheck(client, fdData, message);
}

void	Io::ioLoop()
{
	std::map<int, Client*>::iterator it = Client::clients.begin();
	for (; it != Client::clients.end(); ++it)
		_ioClient(*(it->second));
}


//==========================================================================//
// Constructor, Destructor and OCF Parts ===================================//
//==========================================================================//

// Default Constructor
Io::Io() : _buffer(new char[MAXLINE]) {}

// Destructor
Io::~Io()
{
	delete[] _buffer;
}

// Copy Contructor
Io::Io(const Io& src) : _buffer(src._buffer) {}

// Copy Assigment Operator
Io&		Io::operator=(const Io& src)
{
	if (this != &src)
		_buffer = src._buffer;
	return (*this);
}
