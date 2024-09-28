# include "Io.hpp"
# include "../Client/Client.hpp"
# include "../Message/Message.hpp"
# include "../Message/Node.hpp"
# include "../Utils/Logger.hpp"


//==========================================================================//
// STATIC METHODS===========================================================//
//==========================================================================//


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
	if (fdData.type == FdData::FROMCHILD_FD)
	{
		client.setClientState(Client::DO_RESPONSE);
		fdData.state = FdData::CLOSE;
	}

	// IF MESSAGE OR ITS HEADER IS NOT COMPLETE, FINISH HEADER, SET MESSAGE AS COMPLETE
	if (!message->getHeader())
		message->_createHeader(); // TODO: Check _header because it uses new.
	message->setState(COMPLETE);
}

void	clearBuffer(char* buffer)
{
	if (!buffer)
		return ;
	for (size_t i = 0; i < MAXLINE; ++i)
		buffer[i] = 0;
}

void	Io::_sendMsg(Client& client, FdData& fdData, Message* message)
{
 	int sendValue = 0;
	const std::list<Node>::iterator& it = message->getIterator();
	// std::string messageStr = "HTTP/1.1 200 OK\r\nContent-Length: 18\r\nConnection: close\r\n\r\n<p>hello there</p>";
	// std::string messageStr = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
	// if (message->getBytesSent() == 0 && it != message->getChain().begin())
	// {
		// Logger::info("String Response created: ", "\n" + messageStr);
	// 	message->resetIterator();
	// }

	sendValue = send(fdData.fd, it->getString().c_str() + message->getBytesSent(), it->getString().size() - message->getBytesSent(), MSG_DONTWAIT | MSG_NOSIGNAL);
	// sendValue = send(fdData.fd, messageStr.c_str() + message->getBytesSent(), 1, MSG_DONTWAIT | MSG_NOSIGNAL);
	// sleep(1);

	if (sendValue > 0)
	{
		Logger:: chars(it->getString(), true);
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

	// if unable to send full message, log error and set error Code
	if (sendValue < 0 || (it != message->getChain().end() && sendValue == 0))
	{
		Logger::error("failed to send message String in Client with ID:", client.getId());
		client.setErrorCode(500);
	}
	setFinishedSending(client, fdData, client.getErrorCode());
}

void	Io::_receiveMsg(Client& client, FdData& fdData, Message* message)
{
	int 	recValue = 0;

	clearBuffer(_buffer);

	
	recValue = recv(fdData.fd, _buffer, MAXLINE, MSG_DONTWAIT | MSG_NOSIGNAL);

	// SUCCESSFUL READ -> CONCAT MESSAGE
	if (recValue > 0)
	{
		Logger::info("Successfully received bytes: ", recValue);
		message->bufferToNodes(_buffer, recValue);
	}

	// FINISHED READING because either complete message, or connection was shutdown
	if (recValue <= 0 || message->getState() == COMPLETE)
	{
		if (recValue < 0)
			client.setErrorCode(500);
		setFinishedReceiving(client, fdData, message);
	}
}

void	Io::_ioClient(Client& client)
{
	FdData::e_fdType fdType;
	Message* message = setFdTypeAndMsg(client, fdType);

	// SELECTING CORRECT FDDATA INSTANCE IN CLIENT
	FdData& fdData = client.getFdDataByType(fdType);
	if (!message)
		return ; // TODO: Stop Loop / delete client, panic?
	
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
