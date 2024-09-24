#include "DummyMessage.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

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

// void			DummyMessage::printMessage()
// {
// 	std::cout << std::endl;
// 	std::cout << "MESSAGE CONTENT" << std::endl;
// 	std::map<std::string, std::string>::const_iterator it = _header.begin();
// 	std::cout << "--------------------HEADER--------------------" << std::endl;
// 	for (; it != _header.end(); ++it)
// 		std::cout << it->first << "=" << it->second << std::endl;
// 	std::cout << "--------------------BODY--------------------" << std::endl;
// 	std::cout << _body << std::endl;
// }

//==========================================================================//
// Constructor, Destructor and OCF Parts ===================================//
//==========================================================================//

// Custom Constructor
DummyMessage::DummyMessage(std::string type, int errorCode) : _errorCode(errorCode)
{
	if (type != "test")
		return ;
	std::ifstream file(MESSAGE_HEADER);
	if (!file.is_open())
	{
		std::cerr << "Unable to open" << MESSAGE_HEADER << std::endl;
		return ;
	}
	std::string line;
	std::map<std::string, std::string> header;
	while (std::getline(file, line))
	{
		std::string key;
		std::string value;
		std::istringstream lineStream(line);
		if (std::getline(lineStream, key, '=') && std::getline(lineStream, value))
			header[key] = value;
	}
	file.close();

	file.open(MESSAGE_BODY);
	if (!file.is_open())
	{
		std::cerr << "Unable to open" << MESSAGE_HEADER << std::endl;
		return ;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string body = buffer.str();
	file.close();

	_header = header;
	_body = body;
}

// Default Constructor
DummyMessage::DummyMessage() : _errorCode(0) {}

// Destructor
DummyMessage::~DummyMessage() {}

// Overloaded Insertion Operator (For testing).
std::ostream&	operator<<(std::ostream& out, const DummyMessage& message)
{
	out << "MESSAGE CONTENT" << std::endl;
	out << "--------------------HEADER--------------------" << std::endl;
	std::map<std::string, std::string>::const_iterator it = message._header.begin();
	for (; it != message._header.end(); ++it)
		out << it->first << "=" << it->second << std::endl;
	out << "--------------------BODY--------------------" << std::endl;
	out << message._body;
	return (out);
}

// std::cout << "-----------------DEBUGGING-----------------" << std::endl;
