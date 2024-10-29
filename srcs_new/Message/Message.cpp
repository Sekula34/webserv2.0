
#include "Message.hpp"
#include "Node.hpp"
#include "RequestHeader.hpp"
#include "ResponseHeader.hpp"
#include "../Client/Client.hpp"
#include "../Utils/Logger.hpp"
#include "../Parsing/ParsingUtils.hpp"
#include <sstream>
#include <cmath>
#include <string>

//==========================================================================//
// REGULAR METHODS==========================================================//
//==========================================================================//

int	Message::getState() const
{
	return (_state);
}

AHeader*	Message::getHeader() const
{
	return (_header);
}

// const std::list<Node>&	Message::getChain() const
std::list<Node>&	Message::getChain()
{
	return (_chain);
}

const std::list<Node>::iterator& 	Message::getIterator()
{
	return (_it);
}

void	Message::setState(int s)
{
	_state = s;
}

const size_t&				Message::getBytesSent() const
{
	return (_bytesSent);
}

void						Message::setBytesSent(size_t num)
{
	_bytesSent = num;
}

void	Message::printChain()
{
	std::string type = "";
	
	for(std::list<Node>::iterator it = _chain.begin(); it != _chain.end(); it++)
	{
		if (it->getType() == HEADER)
			type =  "Node Type: HEADER, string: ";
		if (it->getType() == BODY)
			type =  "Node Type: BODY, string: ";
		if (it->getType() == CHUNK)
			type =  "Node Type: CHUNK, string: ";
		if (it->getType() == LCHUNK)
			type =  "Node Type: LCHUNK, string: ";
		if (it->getType() == TRAILER)
			type =  "Node Type: TRAILER, string: ";
		if (it->getType() == CHUNK || it->getType() == LCHUNK)
			Logger::info(type + "\n", it->getStringChunked());
		else
			Logger::info(type + "\n", it->getStringUnchunked());
	}
}

// const std::string	Message::getBodyString() 
// {
// 	std::list<Node>::iterator it;
//
// 	// if (_chunked)
// 	// {
// 	// 	_chunksToBody();
// 	// 	_chunked = false;
// 	// 	_trailer = false;
// 	// }
// 	_findBody(it);
// 	if (it == _chain.begin())
// 		return ("");
// 	return (it->getStringUnchunked());
// }

const std::string	Message::getBodyString() 
{
	std::list<Node>::iterator it;

	_findBody(it);
	if (it == _chain.begin())
		return ("");
	std::string body;
	for (; it != _chain.end() && it->getType() != TRAILER; ++it)
		body += it->getStringUnchunked();
	return (body);
}

int&	Message::getErrorCode() const
{
	return (_errorCode);
}

void	Message::_findBody(std::list<Node>::iterator& it)
{
	it = _chain.begin();
	for (; it != _chain.end(); it++)
	{
		if(it->getType() == BODY || it->getType() == CHUNK) // BF: it only checked if BODY. Now it checks also if CHUNK.
			return;
	}
	it = _chain.begin();
}

size_t	Message::_calcOptimalChunkSize(const std::string& body)
{
	if (MAX_CHUNKSIZE <= 0)
	{
		Logger::error("Can not calculate maximum chunk size!", "");
		// std::cout << "Can not calculate maximum chunk size!" << std::endl;
		return (0);
	}
	int ceiling = std::ceil(static_cast<double>(body.size()) / static_cast<double>(MAX_CHUNKSIZE));
	int result = std::ceil(static_cast<double>(body.size()) / ceiling);
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

void	Message::_bodyToChunks(const std::string& body)
{

	std::list<Node>::iterator currentNode;
	size_t	optimalSize = _calcOptimalChunkSize(body);
	size_t	remainSize = body.size();
	size_t	sizeInNode = 0;
	size_t	strPos = 0;
	
	currentNode = _chain.begin();

	// _chain.pop_back();
	while (remainSize)
	{
		if (remainSize > optimalSize)
			sizeInNode = optimalSize;
		else
			sizeInNode = remainSize;
		remainSize -= sizeInNode;
		_chain.push_back((_newChunkNode(sizeInNode)));
		currentNode++;
		currentNode->setStringChunked(body.substr(strPos, sizeInNode));	
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
		tmp = it;	
		if (it->getType() != TRAILER)
			str += it->getStringUnchunked();
		it++;
		_chain.erase(tmp);
	}
	_chain.push_back(Node(str, BODY, _request));
	_chain.back().setBodySize(str.size());
	_chain.back().setState(COMPLETE);
}

std::string	Message::_createCgiHeaderDel()
{
	const std::string& str = _chain.begin()->getStringUnchunked();
	if (!_chain.begin()->getHasCgiDel())
		return ("\n\n");
	size_t found = str.rfind("\n");
	if (found == str.size() - 1)
		found = str.rfind("\n", str.size() - 2);
	if (found != std::string::npos)
	{
		// std::cout << "created this CGI Header Delimiter: " << std::endl;
		// Logger::chars(str.substr(found, str.size()), true);
		return (str.substr(found, str.size()));
	}
	return ("\n\n");
}

void	Message::createHeader()
{
	if(_header != NULL)
		return;

	// throw std::bad_alloc(); 
	//
	if (_request)
		_header = new RequestHeader(_chain.begin()->getStringUnchunked(), _errorCode);
	else
		_header = ResponseHeader::createCgiResponseHeader(_chain.begin()->getStringUnchunked(), _errorCode, "\n", _createCgiHeaderDel());

	if(_header && _header->getHttpStatusCode() != 0)
	{
		Logger::warning("Found Error in Client Header, error code: ", _header->getHttpStatusCode());
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
	// set whether we are expecting a TRAILER or not
	if (_header->getHeaderFieldMap().find("Trailer") != _header->getHeaderFieldMap().end())
		_trailer = true;
	// set whether we are expecting CHUNKS or not
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

void	Message::_setNodeComplete()
{
	// is HEADER of TRAILER complete?
	if (_it->getType() == HEADER || _it->getType() == TRAILER)
	{
		if (_request || _it->getType() == TRAILER)
		{
			if (_it->getStringUnchunked().find("\r\n\r\n") != std::string::npos)	
				_it->setState(COMPLETE);
		}
		// Logger::chars(_it->getStringUnchunked(), true);
		// else if (_it->getHasCgiDel())
		// 	_it->setState(COMPLETE);
	}

	// is CHUNK HEADER complete?
	if (_it->getType() == CHUNK && !_it->getChunkHeader())
	{
		if (_it->getStringChunked().find("\r\n") != std::string::npos)
			_it->setChunkHeader(true);
	}

	// is CHUNK or LCHUNK complete when no trailer expected?
	if ((_it->getType() == CHUNK || (_it->getType() == LCHUNK && !_trailer)) && _it->getChunkHeader() )
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
		_bodySize = _it->getBodySize(); // MR: Update body size.
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
		// MR: Accumulate chunk body size.
		_bodySize += _calcChunkSize(_it->getStringChunked());

		if (_it->getChunkSize() == 0)
		{
			_it->setType(LCHUNK);
			if (_trailer)
				_it->setState(COMPLETE);
		}
	}
	
	if (_it->getState() != COMPLETE)
		return ;

	if ((_it->getType() == LCHUNK && !_trailer)
		|| _it->getType() == BODY || _it->getType() == TRAILER)
	{
		_state = COMPLETE;
	}

	// if header, create new RequestHeader
	if (_it->getType() == HEADER)
	{
		// Logger::info("Header Complete:", "\n" + _it->getStringUnchunked());
		createHeader();
		_headerInfoToNode();

		// if body size is 0 and message not chunked and message is a request then Message is complete
		if (!_chunked && _it->getBodySize() == 0 && _request)
			_state = COMPLETE;
	}

	if (_it->getType() == TRAILER)
	{
		// THIS IS WHERE WE WOULD ADD THE TRAILER FIELDS TO THE HEADER
		// std::string withoutDel = _it->getStringUnchunked().substr(0, _it->getStringUnchunked().size() - 2);
		// _header->_fillHeaderFieldMap(_header->_getHeaderFields(withoutDel));
		
		// DELETING TRAILER NODE
		_chain.pop_back();
		_trailer = false;

		// Logger::warning("Request Header:", true);
		// std::cout << *_header << std::endl;
	}
}

void	Message::bufferToNodes(char* buffer, size_t num)
{
	size_t	bufferPos = 0;
	// std::cout << "buffer: ";
	// Logger::chars(buffer, true);
	while (bufferPos < num && _state == INCOMPLETE)
	{
		if (_it->getState() == COMPLETE && bufferPos < num && _state == INCOMPLETE)
			_addNewNode();
		_it->concatString(buffer, bufferPos, num);
		_setNodeComplete();
		_parseNode();
	}
}

void	Message::stringsToChain(ResponseHeader* header, const std::string& body)
{
	delete _header;
	_header = header;

	if (body.size() < MAX_BODY_SIZE)
	{
		// Logger::error("------ TADA!! ERROR FOUND ------","");
		header->setOneHeaderField("Content-Length", ParsingUtils::toString(body.size()));
		_chain.begin()->setString(header->turnResponseHeaderToString() + "\r\n");
		// Logger::chars(header->turnResponseHeaderToString(), true);
		_chain.push_back(Node("", BODY, _request));
		std::list<Node>::iterator it = _chain.begin();
		it++;
		it->setString(body);
	}
	else
	{
		// Logger::error("------ VOILA!! ERROR NOT FOUND ------","");
		header->setOneHeaderField("Transfer-Encoding", "chunked");
		_chain.begin()->setString(header->turnResponseHeaderToString() + "\r\n");
		_bodyToChunks(body);
		// printChain();
	}
}
void	Message::resetIterator()
{
	_it = _chain.begin();
}
void	Message::advanceIterator()
{
	if (_it != _chain.end())
		_it++;
}


//==========================================================================//
// Constructor, Destructor and OCF Parts ===================================//
//==========================================================================//

Message::Message (bool request, int& errorCode) : _errorCode(errorCode)
{
	if (_errorCode)
		Logger::error("F@ck message constructor WITH error code", _errorCode);
	// std::cout << "Message default constructor called" << std::endl;
	_request = request;
	_chain.push_back(Node("", HEADER, _request));
	_it = _chain.begin();
	_chunked = false;
	_trailer = false;
	_state = INCOMPLETE;
	_header = NULL;
	_bytesSent = 0;
	_bodySize = 0;
}

Message::~Message (void)
{
	delete _header;
	// std::cout << "Message destructor called" << std::endl;
}

Message::Message(Message const & src) : _errorCode(src._errorCode)
{
	//std::cout << "Message copy constructor called" << std::endl;
	*this = src;
}

Message &	Message::operator=(Message const & rhs)
{
	//std::cout << "Message Copy assignment operator called" << std::endl;
	if (this != &rhs)
	{
		// _bytesSent = rhs._bytesSent;
	}
	return (*this);
}

// MR: Method to retrieve message's total body size. Doesnt matter if chunked or normal.
// because its an accumulator.
const size_t&		Message::getBodySize() const
{
	return (_bodySize);
}
