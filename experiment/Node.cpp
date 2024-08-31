#include "Node.hpp"
#include <sstream>




/******************************************************************************/
/*                               Constructors                                 */
/******************************************************************************/

Node::Node (void):
_state(INCOMPLETE),
_type(0),
_str(""),
_bufferPos(0),
_btr(0)
{
	std::cout << "Node default constructor called" << std::endl;
}

Node::Node (const std::string & str, int type, size_t bufferPos):
_state(INCOMPLETE),
_type(type),
_str(str),
_bufferPos(bufferPos),
_btr(0)
{
	std::cout << "Node default constructor called" << std::endl;
}

/******************************************************************************/
/*                                Destructor                                  */
/******************************************************************************/

Node::~Node (void)
{
	std::cout << "Node destructor called" << std::endl;
}

/******************************************************************************/
/*                             Copy Constructor                               */
/******************************************************************************/

Node::Node(Node const & src):
_state(src._state),
_type(src._type),
_str(src._str)
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

void	Node::setState(int state)
{
	_state = state;
}

std::string	Node::getStringChunked()
{
	if (_type == CHUNK)
		return (_str);
	return (_chunk());
}

std::string	Node::getStringUnchunked()
{
	if (_type == CHUNK)
		return (_unChunk());
	return (_str);
}

const int &		Node::getType() const
{
	return (_type);
}

const int &		Node::getState() const
{
	return (_state);
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
	std::string uc_str;
	std::string del = {'\r', '\n'};
	std::size_t found = 0;

	found = _str.find(del);
	if (found != std::string::npos)
		found += 2;
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

size_t	ft_strstr(char *buffer, std::string remainDel, size_t & bufferPos, size_t num)
{
	size_t	i = bufferPos;
	size_t	j = 0;

	if (!buffer || !remainDel.c_str())
		return (0);
	if (remainDel.c_str()[j] == '\0')
		return (0);
	while (i < num)
	{
		while (buffer[i + j] == remainDel.c_str()[j])
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

bool	Node::_checkRemainDelIsBufStart(std::string remainDel, char* buffer)
{
	for (size_t i = 0; i < remainDel.size(); i++)
	{
		if (remainDel.c_str()[i] != buffer[i + _bufferPos])	
			return (false);
	}
	return (true);
}

void	Node::_calcBtr(char* buffer, std::string del, size_t & bufferPos, size_t num)
{
	std::string remainDel = _getRemainDel(del);
	if (remainDel.size() < del.size() && _bufferPos == 0 && _checkRemainDelIsBufStart(remainDel, buffer))
		_btr = remainDel.size();
	else
	{
		size_t	tmp = ft_strstr(buffer, remainDel, bufferPos, num);
		if (tmp == 0)
		{
			_btr = (num);
			return ;
		}
		_btr = tmp + remainDel.size();
	}
}

void	Node::concatString(char* buffer, size_t & bufferPos, size_t num)
{
	std::string	del;

	if (_type == HEADER)
	{
		del = {'\r','\n','\r','\n'};
		_calcBtr(buffer, del, bufferPos, num);
	}
	if (_type == CHUNK)
	{
		del = {'\r','\n'};
		_calcBtr(buffer, del, bufferPos, num);
	}
	if (_type == BODY)
	{
		_btr = 26 - _str.size();
	}
	std::string tmp(_str.size() + _btr, '0');
	
	for (size_t i = 0; i < _btr; i++)
		_str += buffer[bufferPos + i];
	bufferPos += _btr;
	// std::cout << std::endl  << "string in node: " << std::endl << std::endl << _str << std::endl << std::endl;
}
