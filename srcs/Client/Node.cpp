#include "Node.hpp"
#include "Client.hpp"
#include <sstream>

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
_request(true)
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
_request(request)
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
_request(request)
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

Node::Node(Node const & src):
_state(src._state),
_type(src._type),
_str(src._str),
_btr(src._btr),
_chunkSize(src._chunkSize),
_chunkHeaderSize(src._chunkHeaderSize),
_bodySize(src._bodySize),
_chunkHeader(src._chunkHeader),
_request(src._request)
{
	//std::cout << "Node copy constructor called" << std::endl;
	*this = src;
}

/******************************************************************************/
/*                      Copy Assignment Operator Overload                     */
/******************************************************************************/

Node &	Node::operator=(Node const & rhs)
{
	//std::cout << "Node Copy assignment operator called" << std::endl;
	if (this != &rhs)
	{
	}
	return (*this);
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
	std::string del = {'\r','\n'};

	ss << std::hex << _str.size();
	result = ss.str() + del + _str + del;
	return (result);
}

std::string	Node::_unChunk()
{
	if (_str.size() == 0)
		return (std::cout << "can't unchunk empty string", "");
	std::string uc_str;
	std::string del = {'\r', '\n'};
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

size_t	ft_strstr(unsigned char *buffer, std::string remainDel, size_t & bufferPos, size_t num)
{
	size_t	i = 0;
	size_t	j = 0;

	if (!buffer || !remainDel.c_str())
		return (0);
	if (remainDel.c_str()[j] == '\0')
		return (0);
	while (i + bufferPos < num)
	{
		while (buffer[i + j + bufferPos] == remainDel.c_str()[j])
		{
			j++;
			if (remainDel.c_str()[j] == '\0')
				return (i);
		}
		j = 0;
		i++;
	}
	return (0);
}

bool	Node::_checkRemainDelIsBufStart(std::string remainDel, unsigned char* buffer, size_t bufferPos)
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

void	Node::_calcBtr(unsigned char* buffer, std::string del, size_t & bufferPos, size_t num)
{
	std::string remainDel = _getRemainDel(del);
	// if (remainDel.size() < del.size() && _bufferPos == 0 && _checkRemainDelIsBufStart(remainDel, buffer))
	if (bufferPos == 0 && _checkRemainDelIsBufStart(remainDel, buffer, bufferPos))
		_btr = remainDel.size();
	else
	{
		size_t	tmp = ft_strstr(buffer, remainDel, bufferPos, num);
		if (tmp == 0)
		{
			_btr = num;
			return ;
		}
		_btr = tmp + remainDel.size();
	}
}

void	Node::_setBtr(unsigned char* buffer, size_t & bufferPos, size_t num)
{
	std::string	del;
	if (_type == HEADER || _type == TRAILER)
	{
		if (_request)
			del = {'\r','\n','\r','\n'};
		else
			del = {'\n','\n'};
		_calcBtr(buffer, del, bufferPos, num);
	}
	if ((_type == CHUNK || _type == LCHUNK) && !_chunkHeader)
	{
		if (_request)
			del = {'\r','\n'};
		_calcBtr(buffer, del, bufferPos, num);
	}
	if (_type == BODY)
	{
		if (_request)
			_btr = _bodySize - _str.size();
		else
			_btr = num - bufferPos;
	}
	if ((_type == CHUNK || LCHUNK) && _chunkHeader)
		_btr =  _chunkHeaderSize + _chunkSize + 2 - _str.size() ;
}

void	Node::concatString(unsigned char* buffer, size_t & bufferPos, size_t num)
{
	size_t i = 0;

	
	// sets variable _btr, which is the bytes to read from buffer
	// _btr is based on finding the delimiter of Header and Chunk in buffer
	// or on bodysize and chunksize if applicable
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
