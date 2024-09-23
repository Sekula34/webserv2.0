#include "DummyMessage.hpp"
#include <iostream>

//==========================================================================//
// REGULAR METHODS==========================================================//
//==========================================================================//

std::map<std::string, std::string>&		DummyMessage::getHeader()
{
	return (_header);
}

std::string&	DummyMessage::getBody()
{
	return (_body);
}

void			DummyMessage::setHeader(std::map<std::string, std::string>& header)
{
	_header = header;
}

void			DummyMessage::setBody(std::string& body)
{
	_body = body;
}

int&			DummyMessage::getErrorCode()
{
	return (_errorCode);
}

void			DummyMessage::printMessage()
{
	std::map<std::string, std::string>::const_iterator it = _header.begin();
	std::cout << "--------------------HEADER--------------------" << std::endl;
	for (; it != _header.end(); ++it)
		std::cout << it->first << "=" << it->second << std::endl;
	std::cout << "--------------------BODY--------------------" << std::endl;
	std::cout << _body << std::endl;
}

//==========================================================================//
// Constructor, Destructor and OCF Parts ===================================//
//==========================================================================//

// Constructor
DummyMessage::DummyMessage() : _errorCode(0) {}

// Destructor
DummyMessage::~DummyMessage() {}

// std::cout << "-----------------DEBUGGING-----------------" << std::endl;