
#include "Node.hpp"
#include <sstream>


/******************************************************************************/
/*                               Constructors                                 */
/******************************************************************************/

Node::Node (void):
_state(0),
_type(0),
_str("")
{
	std::cout << "Node default constructor called" << std::endl;
}

Node::Node (const std::string & str, int type):
_state(0),
_type(type),
_str(str)
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

void	Node::concatString(const std::string & s)
{
	_str += s;
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
