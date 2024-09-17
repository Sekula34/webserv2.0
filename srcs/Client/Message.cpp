
#include "Message.hpp"
#include "Node.hpp"
#include "RequestHeader.hpp"
#include "../Response/ResponseHeader.hpp"
#include "Client.hpp"
#include "../Utils/Logger.hpp"
#include "../Parsing/ParsingUtils.hpp"
#include <sstream>
#include <cmath>



#define MAX_CHUNKSIZE	30

/******************************************************************************/
/*                               Constructors                                 */
/******************************************************************************/

Message::Message (bool request)
{

	// std::cout << "Message default constructor called" << std::endl;
	_request = request;
	_chain.push_back(Node("", HEADER, _request));
	_it = _chain.begin();
	_chunked = false;
	_trailer = false;
	_state = INCOMPLETE;
	_header = NULL;
}

Message::Message (void)
{
	// std::cout << "Message default constructor called" << std::endl;
	_request = true;
	_chain.push_back(Node("", HEADER, _request));
	_it = _chain.begin();
	_chunked = false;
	_trailer = false;
	_state = INCOMPLETE;
	_header = NULL;
}

/******************************************************************************/
/*                                Destructor                                  */
/******************************************************************************/

Message::~Message (void)
{
	delete _header;
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

AHeader*	Message::getHeader() const
{
	return (_header);
}

const std::list<Node>&	Message::getChain() const
{
	return (_chain);
}

const std::string	Message::getBodyString() 
{
	std::list<Node>::iterator it;

	if (_chunked)
	{
		_chunksToBody();
		_chunked = false;
	}
	_findBody(it);
	if (it == _chain.begin())
		return ("");
	return (it->getStringUnchunked());
}

const std::list<Node>::iterator& 	Message::getIterator()
{
	return (_it);
}

void	Message::setState(int s)
{
	_state = s;
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


void	Message::_findBody(std::list<Node>::iterator& it)
{
	it = _chain.begin();
	for (; it != _chain.end(); it++)
	{
		if(it->getType() == BODY) 
			return;
	}
	it = _chain.begin();
}

size_t	Message::_calcOptimalChunkSize(std::list<Node>::iterator& it)
{
	_findBody(it);
	if (it == _chain.begin() || MAX_CHUNKSIZE <= 0)
	{
		std::cout << "Can not calculate maximum chunk size!" << std::endl;
		return (0);
	}
	int ceiling = std::ceil(static_cast<double>(it->getBodySize()) / static_cast<double>(MAX_CHUNKSIZE));
	int result = std::ceil(static_cast<double>(it->getBodySize()) / ceiling);
	return (result);
}

Node	Message::_newChunkNode(size_t size)
{
	Node	node;
	node.setType(CHUNK);
	node.setState(COMPLETE);
	node.setChunkHeader(true);
	node.setChunkSize(size);

	return (node);
}

void	Message::_bodyToChunks()
{
	std::list<Node>::iterator it;
	std::list<Node>::iterator currentNode;
	size_t	optimalSize = _calcOptimalChunkSize(it);
	size_t	remainSize = it->getBodySize();
	std::string str = it->getStringUnchunked();	
	size_t	sizeInNode = 0;
	size_t	strPos = 0;
	
	currentNode = _chain.begin();

	_chain.pop_back();
	while (remainSize)
	{
		if (remainSize > optimalSize)
			sizeInNode = optimalSize;
		else
			sizeInNode = remainSize;
		remainSize -= sizeInNode;
		_chain.push_back((_newChunkNode(sizeInNode)));
		currentNode++;
		currentNode->setStringChunked(str.substr(strPos, sizeInNode));	
		strPos +=sizeInNode;
	}
	_chain.push_back(_newChunkNode(0));
	_chain.back().setType(LCHUNK);
	_chain.back().setString("0\r\n\r\n");
}

void	Message::_chunksToBody()
{
	std::string str= "";
	std::list<Node>::iterator it = _chain.begin();
	std::list<Node>::iterator tmp;
	it++;
	while (it != _chain.end())
	{
		str += it->getStringUnchunked();
		tmp = it;	
		it++;
		_chain.erase(tmp);
	}
	_chain.push_back(Node(str, BODY, _request));
	_chain.back().setBodySize(str.size());
	_chain.back().setState(COMPLETE);
}

void	Message::_createHeader()
{
	if(_header != NULL)
		return;
	if (_request)
		_header = new RequestHeader(_chain.begin()->getStringUnchunked());
	else
		_header = ResponseHeader::createCgiResponseHeader(_chain.begin()->getStringUnchunked(), "\n");

	 
		// _header = new ResponseHeader(_chain.begin()->getStringUnchunked());
	// Logger::info("Client header created with : "); std::cout << _message;
	if(_header && _header->getHttpStatusCode() != 0)
	{
		Logger::warning("Found Error in Client Header", false); std::cout << _header->getHttpStatusCode() << std::endl;
		// need to pass this to Client!!
		// setErrorCode(_header->getHttpStatusCode());
	}
}

void	Message::_headerInfoToNode()
{
	if (!_header)
		return ;
	// set BODY SIZE from header
	std::map<std::string, std::string>::const_iterator found = _header->getHeaderFieldMap().find("Content-Length");
	if (found != _header->getHeaderFieldMap().end())
	{
		_ss.clear();
		_ss.str("");
		_ss << _header->getHeaderFieldMap().at("Content-Length");
		int num;
		_ss >> num;
		_it->setBodySize(num);
	}
	if (_header->getHeaderFieldMap().find("Trailer") != _header->getHeaderFieldMap().end())
		_trailer = true;
	found = _header->getHeaderFieldMap().find("Transfer-Encoding");
	if (found != _header->getHeaderFieldMap().end() && found->second.find("chunked") != std::string::npos)
		_chunked = true;
}

void	Message::_addNewNode()
{
	// create REGULAR BODY NODE if message unchunked and header is complete
	if (_it->getType() == HEADER && !_chunked && _header)
			_chain.push_back(Node("", BODY, _it->getBodySize(), _request));

	// create CHUNKED BODY NODE if message is chunked and body is complete
	if (_it->getType() != LCHUNK && _chunked)
		_chain.push_back(Node("", CHUNK, _request));

	// create TRAILER NODE if message is chunked and has trailer and last chunk is complete
	if (_it->getType() == LCHUNK && _trailer)
		_chain.push_back(Node("", TRAILER, _request));
	_it++;
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


void	Message::_isNodeComplete(size_t bufferPos, size_t num)
{
	// is HEADER of TRAILER complete?
	if (_it->getType() == HEADER || _it->getType() == TRAILER)
	{
		std::string del;
		if (_request)
			del = "\r\n\r\n";
		else
			del = "\n\n";
		if (_it->getStringUnchunked().find(del) != std::string::npos)	
			_it->setState(COMPLETE);
		(void)bufferPos;
		(void)num;
		// if (num < MAXLINE && bufferPos == num)
		// 	_it->setState(COMPLETE);
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

void	Message::_parseNode(size_t bufferPos, size_t num)
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

	// if header, create new RequestHeader
	if (_it->getType() == HEADER)
	{
		_createHeader();
		_headerInfoToNode();

		// if body size is 0 and message not chunked and message is a request then Message is complete
		if (!_chunked && _it->getBodySize() == 0 && _request)
			_state = COMPLETE;
		(void)bufferPos;
		(void)num;
		// if (num < MAXLINE && bufferPos == num)
		// 	_state = COMPLETE;
	}
	
	// if Trailer, complete the header with info from trailer
}

void	Message::bufferToNodes(char* buffer, size_t num)
{
	size_t	bufferPos = 0;
	while (bufferPos < num && _state == INCOMPLETE)
	{
		if (_it->getState() == COMPLETE)
			_addNewNode();
		_it->concatString(buffer, bufferPos, num);
		_isNodeComplete(bufferPos, num);
		_parseNode(bufferPos, num);
		// _checkNode();
		// if (num < MAXLINE && bufferPos == num && _it->getType() == HEADER && _it->getState() == COMPLETE)
		// 	_state = COMPLETE;
		if (_it->getState() == COMPLETE && bufferPos < num && _state == INCOMPLETE)
			_addNewNode();
	}
	// printChain();
}
