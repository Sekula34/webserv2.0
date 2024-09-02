
#include "Message.hpp"
#include "Node.hpp"
#include <sstream>

/******************************************************************************/
/*                               Constructors                                 */
/******************************************************************************/

Message::Message (void)
{

	// std::cout << "Message default constructor called" << std::endl;
	_chain.push_back(Node("", HEADER));
	_it = _chain.begin();
	_chunked = false;
	_trailer = false;
	_state = INCOMPLETE;
}

/******************************************************************************/
/*                                Destructor                                  */
/******************************************************************************/

Message::~Message (void)
{
	// std::cout << "Message destructor called" << std::endl;
}

/******************************************************************************/
/*                             Copy Constructor                               */
/******************************************************************************/

Message::Message(Message const & src)
{
	//std::cout << "Message copy constructor called" << std::endl;
	*this = src;
}

/******************************************************************************/
/*                      Copy Assignment Operator Overload                     */
/******************************************************************************/

Message &	Message::operator=(Message const & rhs)
{
	//std::cout << "Message Copy assignment operator called" << std::endl;
	if (this != &rhs)
	{
	}
	return (*this);
}

int	Message::getState() const
{
	return (_state);
}

void	Message::printChain()
{
	for(std::list<Node>::iterator it = _chain.begin(); it != _chain.end(); it++)
	{
		if (it->getType() == HEADER)
			std::cout << "Node Type: HEADER, string: " << std::endl;
		if (it->getType() == BODY)
			std::cout << "Node Type: BODY, string: " << std::endl;
		if (it->getType() == CHUNK)
			std::cout << "Node Type: CHUNK, string: " << std::endl;
		if (it->getType() == LCHUNK)
			std::cout << "Node Type: LAST CHUNK, string: " << std::endl;
 		std::cout << it->getStringUnchunked();
		if (it->getType() != BODY)
			std::cout << std::endl;
	}
}

void	Message::_addNewNode()
{
	// create REGULAR BODY NODE if message unchunked and header is complete
	if (_it->getType() == HEADER && !_chunked)
		_chain.push_back(Node("", BODY));

	// create CHUNKED BODY NODE if message is chunked and body is complete
	if (_it->getType() != LCHUNK && _chunked)
		_chain.push_back(Node("", CHUNK));

	// create TRAILER NODE if message is chunked and has trailer and last chunk is complete
	if (_it->getType() == LCHUNK && _trailer)
		_chain.push_back(Node("", TRAILER));
	_it++;
	if (_it->getType() == BODY)
		_it->setBodySize(27);
}

size_t	calcChunkSize(std::string s)
{
	size_t	x;   
	std::stringstream ss;
	ss << std::hex << s;
	ss >> x;
	return (x);	
}


void	Message::_isNodeComplete()
{
	// is HEADER complete?
	if (_it->getType() == HEADER)
	{
		if (_it->getStringUnchunked().find("\r\n\r\n") != std::string::npos)	
			_it->setState(COMPLETE);
	}

	// is CHUNK HEADER complete?
	if (_it->getType() == CHUNK && !_it->getChunkHeader())
	{
		if (_it->getStringChunked().find("\r\n") != std::string::npos)
			_it->setChunkHeader(true);
	}

	// is CHUNK complete?
	if (_it->getType() == CHUNK && _it->getChunkHeader())
	{
		size_t left = 0;
		size_t right = 0;

		left = _it->getStringChunked().find("\r\n");
		right = _it->getStringChunked().rfind("\r\n");

		if (left != std::string::npos && right != std::string::npos && left != right)	
			_it->setState(COMPLETE);
	}

	// is LCHUNK complete?
	if (_it->getType() == LCHUNK)
	{
		if (_it->getStringChunked() == "0\r\n\r\n")	
			_it->setState(COMPLETE);
	}
}


void	Message::_parseNode()
{
	// if message is chunked -> read chunk header and set _btr
	// if chunk header is size 0 set Type to LCHUNK
	if (_it->getType() == CHUNK && _it->getChunkHeader() && _it->getChunkSize() == 0)
	{
		// save the size of the chunk header, in order to be able to calculate
		// the full length of chunk: chunk header + chunk body defined by hex number in chunk header
		_it->setChunkHeaderSize(_it->getStringChunked().size());

		// save the size of the expected chunk body (save the hex number from first line)
		_it->setChunkSize(calcChunkSize(_it->getStringChunked()));

		if (_it->getChunkSize() == 0)
			_it->setType(LCHUNK);
	}
	
	if (_it->getState() != COMPLETE)
		return ;

	if (_it->getType() == LCHUNK && !_trailer)
		_state = COMPLETE;

	// if header, create new ClientHeader with Filips code
	if (_it->getType() == HEADER)
	{
		_chunked = true;
	}
	
	// if Trailer, complete the header with info from trailer
}

void	Message::bufferToNodes(char* buffer, size_t num)
{
	size_t	bufferPos = 0;
	while (bufferPos < num && _state == INCOMPLETE)
	{
		_it->concatString(buffer, bufferPos, num);
		_isNodeComplete();
		_parseNode();
		if (_it->getState() == COMPLETE && (bufferPos < num) && _state == INCOMPLETE)
			_addNewNode();
	}
}
