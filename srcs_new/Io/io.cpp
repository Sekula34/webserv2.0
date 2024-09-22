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

	// IF client state is new or we have just finished writing to CGI -> we want to receive
	if (client.getClientState() == Client::NEW || client.getClientState() == Client::F_CGIWRITE)
	{
		FdData::e_fdType fdType;
		// if Client state == NEW, then receive into requestMsg
		if (client.getClientState() == Client::NEW)
		{
			fdType = FdData::CLIENT_FD;
			message = client.getMsg(Client::REQ_MSG);
		}
		// if Client state != NEW then we receive into cgiResponse Message 
		else
		{
			fdType = FdData::FROMCHILD_FD;
			message = client.getMsg(Client::CGIRESP_MSG);
		}
		FdData& fdData = client.getFdDataByType(fdType);
		if (!message)
			return ; // TODO: Stop Loop / delete client, panic?
		// If the state of the file descripter allows us to receive -> we receive
		if (fdData.state  == FdData::R_RECEIVE || fdData.state  == FdData::R_SENDREC)
			_receiveMsg(client, fdData, message);
		return ;
	}
	_sendMsg(client);
}	

void	Io::_sendMsg(Client& client)
{

}

void	Io::_receiveMsg(Client& client, FdData& fdData, Message* message)
{
	int 	readValue = 0;

	readValue = recv(fdData.fd, _buffer, MAXLINE, MSG_DONTWAIT | MSG_NOSIGNAL);

	// successful read -> concat message
	if (readValue > 0)
		message->bufferToNodes(_buffer, readValue);

	// failed read
	if (readValue < 0)
	{
		Logger::warning("failed to read with return Value: ", readValue);
		client.setErrorCode(500);
		if (fdData.type == FdData::CLIENT_FD)
			client.setClientState(Client::F_REQUEST);
		else if (fdData.type == FdData::FROMCHILD_FD)
		{
			client.setClientState(Client::F_CGIREAD);
			fdData.state = FdData::CLOSE;
		}
	}

	// EOF reached, child has gracefully shutdown connection
	// TODO: implement this when CGI is refactored
	// if (readValue == 0 && _client->waitReturn != 0)
	if (readValue == 0)
	{
		Client::e_clientMsgType msgType;
		if (fdData.type == FdData::CLIENT_FD)
			msgType = Client::REQ_MSG;
		else
		 	msgType = Client::CGIRESP_MSG;
		if (!client.getMsg(msgType)->getHeader())
			client.getMsg(msgType)->_createHeader(); // TODO: Check _headwer because it uses new.
		client.getMsg(msgType)->setState(COMPLETE);

		// TODO: implement this when CGI is refactored
		//_client->hasReadFromCgi = true;

		// copy the error code in the CgiResponseHeader into client
		// so that errors from CGI can be processed
		// if (client.getMsg(msgType) && client.getMsg(msgType)->getHeader() && !client.getErrorCode())
		// 	client.setErrorCode(client.getMsg(msgType)->getHeader().getHttpStatusCode());
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
