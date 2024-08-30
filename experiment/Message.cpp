
#include "Message.hpp"
#include "Node.hpp"

/******************************************************************************/
/*                               Constructors                                 */
/******************************************************************************/

Message::Message (void)
{

	_chain.push_back(Node("", HEADER));
	_it = _chain.begin();
	std::cout << "Message default constructor called" << std::endl;
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

void	Message::checkNodeComplete()
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

void	Message::addStr(const std::string & buffer)
{

	//add buffer to Node String
	_it->concatString(buffer);

}
