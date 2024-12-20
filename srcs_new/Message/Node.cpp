#include "Node.hpp"
#include "../Utils/Logger.hpp"
// #include "../Client/Client.hpp"
#include "../Io/Io.hpp"
#include <sstream>
#include <iostream>

/******************************************************************************/
/*                               Constructors                                 */
/******************************************************************************/

Node::Node (void):
_state(INCOMPLETE),
_type(0),
_str(""),
_btr(0),
_chunkSize(0),
_chunkHeaderSize(0),
_bodySize(0),
_chunkHeader(false),
_request(true),
_hasNewline(false),
_hasCgiDel(false)
{
	// std::cout << "Node default constructor called" << std::endl;
}

Node::Node (const std::string & str, int type, bool request):
_state(INCOMPLETE),
_type(type),
_str(str),
_btr(0),
_chunkSize(0),
_chunkHeaderSize(0),
_bodySize(0),
_chunkHeader(false),
_request(request),
_hasNewline(false),
_hasCgiDel(false)
{
	// std::cout << "Node default constructor called" << std::endl;
}

Node::Node (const std::string & str, int type, size_t size, bool request):
_state(INCOMPLETE),
_type(type),
_str(str),
_btr(0),
_chunkSize(0),
_chunkHeaderSize(0),
_bodySize(size),
_chunkHeader(false),
_request(request),
_hasNewline(false),
_hasCgiDel(false)
{
	// std::cout << "Node default constructor called" << std::endl;
}

/******************************************************************************/
/*                                Destructor                                  */
/******************************************************************************/

Node::~Node (void)
{
	// std::cout << "Node destructor called" << std::endl;
}

/******************************************************************************/
/*                             Copy Constructor                               */
/******************************************************************************/

Node::Node(const Node& src):
_state(src._state),
_type(src._type),
_str(src._str),
_btr(src._btr),
_chunkSize(src._chunkSize),
_chunkHeaderSize(src._chunkHeaderSize),
_bodySize(src._bodySize),
_chunkHeader(src._chunkHeader),
_request(src._request),
_hasNewline(src._hasNewline),
_hasCgiDel(src._hasCgiDel)
{
	//std::cout << "Node copy constructor called" << std::endl;
	*this = src;
}

/******************************************************************************/
/*                      Copy Assignment Operator Overload                     */
/******************************************************************************/

Node &	Node::operator=(const Node & rhs)
{
	//std::cout << "Node Copy assignment operator called" << std::endl;
	if (this != &rhs)
	{
	}
	return (*this);
}

bool	Node::getHasCgiDel()
{
	return (_hasCgiDel);
}

void	Node::setString(const std::string & s)
{
	_str = s;
}

void	Node::setStringChunked(const std::string & s)
{
	_str = s;
	_chunk();
}

void	Node::setState(int state)
{
	_state = state;
}

std::string	Node::getStringChunked()
{
	if (_type == CHUNK || _type == LCHUNK)
		return (_str);
	return (_chunk());
}

std::string	Node::getStringUnchunked()
{
	if (_type == CHUNK || _type == LCHUNK)
		return (_unChunk());
	return (_str);
}

const int &	Node::getType() const
{
	return (_type);
}

const int &	Node::getState() const
{
	return (_state);
}

bool	Node::getChunkHeader() const
{
	return (_chunkHeader);
}

size_t	Node::getChunkSize() const
{
	return (_chunkSize);
}

size_t	Node::getChunkHeaderSize()
{
	return (_chunkHeaderSize);
}
const std::string&			Node::getString() const
{
	return (_str);
}

void	Node::setChunkHeader(bool b)
{
	_chunkHeader = b;
}


void	Node::setChunkSize(const size_t size)
{
	_chunkSize = size;
}

void	Node::setBodySize(const size_t size)
{
	_bodySize = size;
}
void	Node::setChunkHeaderSize(const size_t size)
{
	_chunkHeaderSize = size;
}

void	Node::setType(int type)
{
	_type = type;	
}

std::string	Node::_chunk()
{	
	std::stringstream ss;
	std::string result;
	std::string del = ("\r\n");
	ss << std::hex << _str.size();
	_str = ss.str() + del + _str + del;
	// result = ss.str() + del + _str + del;
	return (result);
}

std::string	Node::_unChunk()
{
	if (_str.size() == 0)
	{
		Logger::warning("can't unchunk empty string", "");
		return ("");
	}
	std::string uc_str;
	std::string del = ("\r\n");
	std::size_t found = 0;

	found = _str.find(del);
	if (found != std::string::npos)
		found += 2;
	else
		return (_str);
	uc_str = _str.substr(found, _str.size() - found - 2);
	return (uc_str);
}

std::string	Node::_getRemainDel(const std::string & del)
{
	std::string	tmp = "";
	size_t		found = 0;

	for (size_t i = 0; i < del.size(); i++)
	{
		tmp = del.substr(0, del.size() - i);
		found = _str.rfind(tmp);
		if (found == std::string::npos)
			continue;
		if (found == _str.size() - (tmp.size()))
			return (del.substr(tmp.size(), del.size()));
	}
	return (del);
}

size_t	Node::getBodySize() const
{
	return (_bodySize);
}

size_t	ft_strstr(char *buffer, std::string remainDel, size_t & bufferPos, size_t num)
{
	size_t	i = 0;
	size_t	j = 0;

	if (!buffer || remainDel.empty())
		return (0);
	if (remainDel.c_str()[j] == '\0')
		return (0);
	// while (i + j + bufferPos < num)
	while (i + bufferPos < num)
	{
		j = 0;
		while (i + j + bufferPos < num && buffer[i + j + bufferPos] == remainDel.c_str()[j])
		{
			j++;
			if (remainDel.c_str()[j] == '\0')
				return (i);
		}
		i++;
	}
	return (0);
}

bool	Node::_checkRemainDelIsBufStart(std::string remainDel, char* buffer, size_t bufferPos)
{
	if (remainDel.size() + bufferPos > MAXLINE)
		return (false);
	for (size_t i = 0; i < remainDel.size(); i++)
	{
		if (remainDel.c_str()[i] != buffer[i + bufferPos])	
			return (false);
	}
	return (true);
}

size_t	Node::_calcBtr(char* buffer, std::string del, size_t & bufferPos, size_t num)
{
	std::string remainDel = _getRemainDel(del);
	if (bufferPos == 0 && _checkRemainDelIsBufStart(remainDel, buffer, bufferPos))
		return (remainDel.size());
	else
	{
		size_t	tmp = ft_strstr(buffer, remainDel, bufferPos, num);
		if (tmp == 0)
			return (num);
		return (tmp + remainDel.size());
	}
}

size_t	Node::_calcBtrCgi(char* buffer, size_t & bufferPos, size_t num)
{
	size_t i = bufferPos;

	for (;i < num; i++)
	{
		if (buffer[i] == '\r' && _hasNewline)
		{
			_hasCgiDel = true;
			continue ;
		}
		if (buffer[i] == '\n')
		{
			if (_hasNewline)
			{
				_state = COMPLETE;
				return (++i);
			}
			else
				_hasNewline = true;
		}
		if (buffer[i] != '\r' && buffer[i] != '\n')
		{
			if (_hasCgiDel)
			{
				_state = COMPLETE;
				return (i);
			}
			_hasNewline = false;
		}
	}
	return (i);
}

void	Node::_setBtr(char* buffer, size_t & bufferPos, size_t num)
{
	std::string	del;
	if (_type == HEADER || _type == TRAILER)
	{
		if (_request)
			_btr = _calcBtr(buffer, "\r\n\r\n", bufferPos, num);
		else
			_btr = _calcBtrCgi(buffer, bufferPos, num);
	}

	if ((_type == CHUNK || _type == LCHUNK) && !_chunkHeader)
	{
		del = "\r\n";
		_btr = _calcBtr(buffer, del, bufferPos, num);
	}
	if (_type == BODY)
	{
		if (_request || _bodySize)
			_btr = _bodySize - _str.size();
		else
			_btr = num - bufferPos;
	}
	if ((_type == CHUNK || LCHUNK) && _chunkHeader)
		_btr =  _chunkHeaderSize + _chunkSize + 2 - _str.size() ;
	
}

void	Node::concatString(char* buffer, size_t & bufferPos, size_t num)
{
	size_t i = 0;

	
	// sets variable _btr, which is the bytes to read from buffer
	// _btr is based on finding the delimiter of Header and Chunk in buffer
	// or it is based on bodysize and chunksize if applicable
	_setBtr(buffer, bufferPos, num);

	// increase the size of string so it can hold the new bytes (_btr)
	_str.reserve(_str.size() + _btr);

	// fill the string with chars from buffer up to _btr or end of filled part of buffer
	for (; i < _btr && i < num - bufferPos; i++)
		_str += buffer[bufferPos + i];

	// set the position inf buffer where reading has stopped
	bufferPos += i;

	// std::cout << std::endl  << "string in node: " << std::endl << std::endl << _str << std::endl << std::endl;
}
