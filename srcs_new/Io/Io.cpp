# include "Io.hpp"
# include "../Client/Client.hpp"
# include "../Message/Message.hpp"
# include "../Message/Node.hpp"
# include "../Utils/Logger.hpp"


//==========================================================================//
// STATIC METHODS===========================================================//
//==========================================================================//

void	Io::ioLoop()
{
	std::map<int, Client*>::iterator it = Client::clients.begin();
	for (; it != Client::clients.end(); ++it)
		_ioClient(*(it->second));
}

void	Io::_ioClient(Client& client)
{
	Message* message = NULL;

	// IF CLIENT STATE IS NEW OR WE HAVE JUST FINISHED WRITING TO CGI -> WE WANT TO RECEIVE
	if (client.getClientState() == Client::NEW || client.getClientState() == Client::F_CGIWRITE)
	{
		FdData::e_fdType fdType;

		// IF CLIENT STATE == NEW, THEN RECEIVE INTO REQUESTMSG, USING CLIENT FD
		if (client.getClientState() == Client::NEW)
		{
			fdType = FdData::CLIENT_FD;
			message = client.getMsg(Client::REQ_MSG);
		}

		// IF CLIENT STATE != NEW THEN WE RECEIVE INTO CGIRESPONSE MESSAGE, USING FROMCHILD FD
		else
		{
			fdType = FdData::FROMCHILD_FD;
			message = client.getMsg(Client::CGIRESP_MSG);
		}
		
		// SELECTING CORRECT FDDATA INSTANCE IN CLIENT
		FdData& fdData = client.getFdDataByType(fdType);
		if (!message)
			return ; // TODO: Stop Loop / delete client, panic?
		
		// IF THE STATE OF THE FILE DESCRIPTER ALLOWS US TO RECEIVE -> WE RECEIVE
		if (fdData.state  == FdData::R_RECEIVE || fdData.state  == FdData::R_SENDREC)
			_receiveMsg(client, fdData, message);

		// TODO implement this:
		// IF RECEIVING HAS STARTED, MSG STRING IS NOT EMPTY, MSG STATE IS INCOMPLETE
		// AND WE ARE SUDDENLY NOT ALLOWED TO RECEIVE -> WE CLOSE MESSAGE,
		// BECAUSE CLIENT HAS PROBABLY CLOSED SOCKET
		return ;
	}
	_sendMsg(client, fdData, message);
}	

void	setFinishedSending(Client& client, FdData& fdData, int error)
{
	if (fdData.type == FdData::CLIENT_FD)
		client.setClientState(Client::DELETEME);
	else if (fdData.type == FdData::TOCHILD_FD)
	{
		if (!error)
			client.setClientState(Client::F_CGIWRITE);
		else
			client.setClientState(Client::DELETEME);
	}
	fdData.state = FdData::CLOSE;
}

void	Io::_sendMsg(Client& client, FdData& fdData, Message* message)
{
 	int writeValue = 0;
	std::string messageStr = "HTTP/1.1 200\r\nOK Content-Length: 0\r\nConnection: close\r\n\r\n";
	Logger::info("String Response created: ", messageStr);

	writeValue = send(fdData.fd, messageStr.c_str() + message->getBytesSent(), messageStr.size() - message->getBytesSent(), MSG_DONTWAIT | MSG_NOSIGNAL);
	Logger::info("bytes written to CGI child process: ", writeValue);
	message->setBytesSent(message->getBytesSent() + writeValue);

	// RETURN IF FULL MESSAGE COULD NOT BE SENT
	if (message->getBytesSent() < messageStr.size() && writeValue > 0)
		return ;

	// if unable to send full message, log error and set error Code
	if (writeValue < 0 || (writeValue == 0 && message->getBytesSent() < messageStr.size()))
	{
		Logger::error("failed to send message String in Client with ID:", client.getId());
		client.setErrorCode(500);
	}

	setFinishedSending(client, fdData, client.getErrorCode());
	// closing Socket and and removing it from epoll
	// Logger::warning("removing FD from epoll: ");
	// std::cout << "FD: " << _socketsToChild[0] << " Id: " << _client->getId() << std::endl;
	// Data::epollRemoveFd(_client->socketToChild);
	// close(_client->socketToChild);
	// _client->socketToChild = DELETED;
}

void	setFinishedReceiving(Client& client, FdData& fdData, Message* message)
{
	if (fdData.type == FdData::CLIENT_FD)
		client.setClientState(Client::F_REQUEST);
	else if (fdData.type == FdData::FROMCHILD_FD)
	{
		client.setClientState(Client::F_CGIREAD);
		fdData.state = FdData::CLOSE;
	}
	// IF MESSAGE OR ITS HEADER IS NOT COMPLETE, FINISH HEADER, SET MESSAGE AS COMPLETE
	if (!message->getHeader())
		message->_createHeader(); // TODO: Check _header because it uses new.
	message->setState(COMPLETE);
}

void	Io::_receiveMsg(Client& client, FdData& fdData, Message* message)
{
	int 	readValue = 0;

	readValue = recv(fdData.fd, _buffer, MAXLINE, MSG_DONTWAIT | MSG_NOSIGNAL);

	// SUCCESSFUL READ -> CONCAT MESSAGE
	if (readValue > 0)
		message->bufferToNodes(_buffer, readValue);

	// READ FAILED
	if (readValue < 0)
	{
		setFinishedReceiving(client, fdData, message);
		Logger::warning("failed to read with return Value: ", readValue);
		client.setErrorCode(500);
	}

	// EOF reached, child has gracefully shutdown connection
	// TODO: implement this when CGI is refactored, not sure it is needed though
	// if (readValue == 0 && _client->waitReturn != 0)
	if (readValue == 0 || message->getState() == COMPLETE)
	{
		setFinishedReceiving(client, fdData, message);

		// DEBUG
		client.getMsg(Client::REQ_MSG)->printChain();
	}
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
