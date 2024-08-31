
#include "Message.hpp"
#include "Node.hpp"

/******************************************************************************/
/*                               Constructors                                 */
/******************************************************************************/

Message::Message (void)
{

	std::cout << "Message default constructor called" << std::endl;
	_chain.push_back(Node("", HEADER, _bufferPos));
	_it = _chain.begin();
	_bufferPos = 0;
}

/******************************************************************************/
/*                                Destructor                                  */
/******************************************************************************/

Message::~Message (void)
{
	std::cout << "Message destructor called" << std::endl;
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

void	Message::_checkNodeComplete()
{
	// is HEADER complete?
	if (_it->getType() == HEADER)
	{
		if (_it->getStringUnchunked().find("\r\n\r\n") != std::string::npos)	
			_it->setState(COMPLETE);
	}

	// is CHUNK complete?
	if (_it->getType() == CHUNK)
	{
		size_t left = 0;
		size_t right = 0;

		left = _it->getStringUnchunked().find("\r\n");
		right = _it->getStringUnchunked().rfind("\r\n");

		if (left != std::string::npos && right != std::string::npos && left != right)	
			_it->setState(COMPLETE);
	}

	// is BODY complete

}

void	Message::printChain()
{
	for(std::list<Node>::iterator it = _chain.begin(); it != _chain.end(); it++)
	{
		if (it->getType() == HEADER)
			std::cout << "Node Type: HEADER, string: " << std::endl;
		if (it->getType() == BODY)
			std::cout << "Node Type: BODY, string: " << std::endl;
 		std::cout << it->getStringUnchunked() << std::endl;
	}
}

void	Message::bufferToNodes(char* buffer, size_t num)
{
	size_t	bufferPos = 0;
	//add buffer to Node String
	while (bufferPos < num)
	{
		_it->concatString(buffer, bufferPos, num);
		_checkNodeComplete();
		if (_it->getState() == COMPLETE)
		{
			if (_it->getType() == HEADER)
				_chain.push_back(Node("", BODY, _bufferPos));
			_it++;
		}
	}
}

