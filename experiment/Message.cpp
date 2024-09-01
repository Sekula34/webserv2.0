
#include "Message.hpp"
#include "Node.hpp"

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

void	Message::_isNodeComplete()
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
		// if (it->getType() == HEADER)
		// 	std::cout << "Node Type: HEADER, string: " << std::endl;
		// if (it->getType() == BODY)
		// 	std::cout << "Node Type: BODY, string: " << std::endl;
 		std::cout << it->getStringUnchunked();
		// if (it->getType() == BODY)
		// 	std::cout << std::endl;
	}
}

void	Message::_addNewNode()
{
	// create REGULAR BODY NODE if message unchunked and header is complete
	if (_it->getType() == HEADER && !_chunked)
		_chain.push_back(Node("", BODY));

	// create CHUNKED BODY NODE if message is chunked and body is complete
	if (_it->getType() != LCHUNK && _chunked)
		_chain.push_back(Node("", CHUNK));

	// create TRAILER NODE if message is chunked and has trailer and last chunk is complete
	if (_it->getType() == LCHUNK && _trailer)
		_chain.push_back(Node("", TRAILER));
	_it++;
	if (_it->getType() == BODY)
		_it->setBodySize(27);
}

void	Message::_parseNode()
{
	// if chunk read chunk header and set _btr
	// if chunk header is size 0 set Type to LCHUNK
	
	if (_it->getState() != COMPLETE)
		return ;

	// if header, create new ClientHeader with Filips code
	
	// if Trailer, complete the header with info from trailer
}

void	Message::bufferToNodes(char* buffer, size_t num)
{
	size_t	bufferPos = 0;
	while (bufferPos < num)
	{
		_it->concatString(buffer, bufferPos, num);
		_isNodeComplete();
		_parseNode();
		if (_it->getState() == COMPLETE)
			_addNewNode();
	}
}
