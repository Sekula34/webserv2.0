# include "Io.hpp"
# include "../Client/Client.hpp"
# include "../Message/Message.hpp"


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
		Client::e_clientFdType fdType;
		// if Client state == NEW, then receive into requestMsg
		if (client.getClientState() == Client::NEW)
		{
			fdType = Client::CLIENT_FD;
			message = client.getMsg(Client::REQ_MSG);
		}
		// if Client state != NEW then we receive into cgiResponse Message 
		else
		{
			fdType = Client::FROMCHILD_FD;
			message = client.getMsg(Client::CGIRESP_MSG);
		}
		Client::fdStatePair& fdPair = client.getClientFds()[fdType];

		// If the state of the file descripter allows us to receive -> we receive
		if (fdPair.second  == Client::R_RECEIVE || fdPair.second  == Client::R_SENDREC)
			_receiveMsg(client, fdPair, message);
		return ;
	}
	_sendMsg(client);
}	

void	Io::_sendMsg(Client& client)
{

}

void	Io::_receiveMsg(Client& client, Client::fdStatePair& fdPair, Message* message)
{
	int 	readValue = 0;
	// char*	buffer = new char[MAXLINE];
	// char	buffer[MAXLINE];

	//_client->clearRecvLine();
	readValue = recv(fdPair.first, _buffer, MAXLINE, MSG_DONTWAIT | MSG_NOSIGNAL);

	// successful read -> concat message
	if (readValue > 0)
		message->bufferToNodes(_buffer, readValue);

	// failed read -> stop CGI and set errorcode = 500
	if (readValue < 0)
	{
		Logger::warning("failed tor read from Child Process", true);
		_stopCgiSetErrorCode();
	}

	// EOF reached, child has gracefully shutdown connection
	if (readValue == 0 && _client->waitReturn != 0)
	{
		if (!_client->getServerMsg()->getHeader())
			_client->getServerMsg()->_createHeader();
		_client->getServerMsg()->setState(COMPLETE);
		_client->hasReadFromCgi = true;
		// _client->getServerMsg()->printChain();
		// copy the error code in the CgiResponseHeader into client
		// so that errors from CGI can be processed
		if (_client->getServerMsg() && _client->getServerMsg()->getHeader() && !_client->getErrorCode())
			_client->setErrorCode(_client->getServerMsg()->getHeader()->getHttpStatusCode());
	}

}


//==========================================================================//
// Constructor, Destructor and OCF Parts ===================================//
//==========================================================================//

// Custom Constructor
Io::Io() : _buffer(new char[MAXLINE]) {}

// Destructor
Io::~Io()
{
	delete[] _buffer;
}
