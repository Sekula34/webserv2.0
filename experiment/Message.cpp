
#include "Message.hpp"
#include "Node.hpp"
#include <sstream>
#include <cmath>

#define MAX_CHUNKSIZE	20

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
		if (it->getType() == TRAILER)
			std::cout << "Node Type: TRAILER, string: " << std::endl;
 		std::cout << it->getStringUnchunked() << std::endl << std::endl;
	}
}

void	Message::_addNewNode()
{
	// create REGULAR BODY NODE if message unchunked and header is complete
	if (_it->getType() == HEADER && !_chunked)
	{
		// PARSING, THIS IS TEMPORARY AND WILL BE DONE BY FILIPS CLASS
		std::string target("Content-Length:");
		size_t found =_it->getStringUnchunked().find(target);
		found += target.length();
		size_t endl = _it->getStringUnchunked().find("\n", found);
		std::string number = _it->getStringUnchunked().substr(found + 1, endl - found);
		_ss.clear();
		_ss.str("");
		_ss << number;
		size_t bodySize;
		_ss >> bodySize;
		_it->setBodySize(bodySize);
		found = _it->getStringUnchunked().find("Trailer");
		if (found != std::string::npos)
			_trailer = true;
		found = _it->getStringUnchunked().find("chunked");
		if (found != std::string::npos)
			_chunked = true;
		// TEMPORARY CODE END
		

		if (!_chunked)
			_chain.push_back(Node("", BODY));
	}

	// create CHUNKED BODY NODE if message is chunked and body is complete
	if (_it->getType() != LCHUNK && _chunked)
		_chain.push_back(Node("", CHUNK));

	// create TRAILER NODE if message is chunked and has trailer and last chunk is complete
	if (_it->getType() == LCHUNK && _trailer)
		_chain.push_back(Node("", TRAILER));
	_it++;
	if (_it->getType() == BODY)
		_it->setBodySize(_chain.begin()->getBodySize());
}

size_t	Message::_calcChunkSize(std::string s)
{
	size_t	x;   
	_ss.clear();
	_ss.str("");
	_ss << std::hex << s;
	_ss >> x;
	if (_ss.fail())
		_state = ERROR;
	return (x);	
}


void	Message::_isNodeComplete()
{
	// is HEADER of TRAILER complete?
	if (_it->getType() == HEADER || _it->getType() == TRAILER)
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

	// is CHUNK or LCHUNK complete?
	if ((_it->getType() == CHUNK || _it->getType() == LCHUNK) && _it->getChunkHeader())
	{
		if(_it->getChunkHeaderSize() + _it->getChunkSize() + 2 == _it->getStringChunked().length())
			_it->setState(COMPLETE);
		if (_it->getState() == COMPLETE
	  		&& (_it->getStringChunked()[_it->getStringChunked().length() - 2] != '\r'
	  		|| _it->getStringChunked()[_it->getStringChunked().length() - 1] != '\n'))
			_state = ERROR;
	}

	// is BODY complete?
	if (_it->getType() == BODY)
	{
		if (_it->getBodySize() == _it->getStringUnchunked().size())	
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
		_it->setChunkSize(_calcChunkSize(_it->getStringChunked()));

		if (_it->getChunkSize() == 0)
			_it->setType(LCHUNK);
	}
	
	if (_it->getState() != COMPLETE)
		return ;

	if ((_it->getType() == LCHUNK && !_trailer)
		|| _it->getType() == BODY || _it->getType() == TRAILER)
		_state = COMPLETE;

	// if header, create new ClientHeader with Filips code
	if (_it->getType() == HEADER)
	{
	}
	
	// if Trailer, complete the header with info from trailer
}

size_t	Message::_calcChunkDivisor()
{
	std::list<Node>::iterator it = _chain.begin();
	for (; it != _chain.end(); it++)
	{
		if(it->getType() == BODY) 
			break;
	}
	if (it == _chain.end() || MAX_CHUNKSIZE <= 0)
	{
		std::cout << "Can not calculate maximum chunk size!" << std::endl;
		return (0);
	}
	int ceiling = std::ceil(static_cast<double>(it->getBodySize()) / static_cast<double>(MAX_CHUNKSIZE));
	std::cout << "Max Chunksize: " << ceiling << std::endl;
	return (ceiling);
}

void	Message::bufferToNodes(char* buffer, size_t num)
{
	size_t	bufferPos = 0;
	while (bufferPos < num && _state == INCOMPLETE)
	{
		_it->concatString(buffer, bufferPos, num);
		_isNodeComplete();
		_parseNode();
		// _checkNode();
		if (_it->getState() == COMPLETE && bufferPos < num && _state == INCOMPLETE)
			_addNewNode();
	}
}
