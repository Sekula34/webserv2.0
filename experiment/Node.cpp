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

Node::Node (const std::string & str, int type):
_state(INCOMPLETE),
_type(type),
_str(str),
_bufferPos(0),
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

size_t	ft_strstr(char *str, std::string remainDel)
{
	size_t	i = 0;
	size_t	j = 0;

	if (!str || !remainDel.c_str())
		return (0);
	if (remainDel.c_str()[j] == '\0')
		return (0);
	while (i < MAXLINE)
	{
		while (str[i + j] == remainDel.c_str()[j])
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

bool	Node::checkRemainDelIsCharStart(std::string remainDel, char* buffer)
{
	for (size_t i = 0; i < remainDel.size(); i++)
	{
		if (remainDel.c_str()[i] != buffer[i + _bufferPos])	
			return (false);
	}
	return (true);
}

void	Node::calcBtr(char* buffer, std::string del)
{
	std::string remainDel = _getRemainDel(del);
	if (remainDel.size() < del.size() && checkRemainDelIsCharStart(remainDel, buffer))
		_btr = remainDel.size();
	else
	{
		size_t	tmp = ft_strstr(buffer, remainDel);
		if (tmp == 0)
		{
			_btr = (MAXLINE - 1);
			return ;
		}
		_btr = tmp + remainDel.size();
	}
}

void	Node::concatString(char* buffer)
{
	std::string	del;

	if (_type == HEADER)
	{
		del = {'\r','\n','\r','\n'};
		calcBtr(buffer, del);
	}
	if (_type == CHUNK)
	{
		del = {'\r','\n'};
		calcBtr(buffer, del);

	}
	std::string tmp(_str.size() + _btr, '0');
	
	for (size_t i = 0; i < _btr; i++)
		_str += buffer[_bufferPos + i];
}
