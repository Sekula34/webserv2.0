#include "ClientRequestHeader.hpp"
#include "../Utils/Logger.hpp"
#include <vector>
#include "../Parsing/ParsingUtils.hpp"

ClientRequestHeader::ClientRequestHeader(const std::string message)
:_message(message)
{
	_initAllVars();
	_setCHVarivables();
}

ClientRequestHeader::ClientRequestHeader(const ClientRequestHeader& source)
:_message(source._message)
{
	
}

ClientRequestHeader& ClientRequestHeader::operator=(const ClientRequestHeader& source)
{
	(void)source;
	Logger::warning("Give me really good reason why you want this");
	return (*this);
}

ClientRequestHeader::~ClientRequestHeader()
{

}


void ClientRequestHeader::_initAllVars(void)
{
	_errorCode = 0;
	_requestLine= "";

	_requestLineElements.requestMethod = "";
	_requestLineElements.requestTarget= "";
	_requestLineElements.protocolVersion = "";
	
	_host.name = "";
	_host.port = 0;
}

bool ClientRequestHeader::_setCHVarivables()
{
	if(_setRequestLine() == false)
		return false;
	if(_fillRequestStruct() == false)
		return false;
	if(_setHost() == false)
		return false;
	if(_checkRequestStruct() == false)
		return false;
	return true;
}

const int& ClientRequestHeader::getHostPort(void) const 
{
	return(_host.port);
}

const std::string& ClientRequestHeader::getHostName(void) const 
{
	return(_host.name);
}

const std::string& ClientRequestHeader::getFullMessage(void) const
{
	return (_message);
}


const RequestLine& ClientRequestHeader::getRequestLine() const 
{
	return (_requestLineElements);
}


const int& ClientRequestHeader::getErrorCode(void) const 
{
	return _errorCode;
}

const std::string& ClientRequestHeader::getRequestedUrl(void) const 
{
	const RequestLine& line = getRequestLine();
	return line.requestTarget;
}

bool ClientRequestHeader::_setRequestLine(void) 
{
	std::string firstLine;
	size_t firstLineEnd =_message.find("\r\n");
	if(firstLineEnd == std::string::npos)
	{
		Logger::warning("Cannot find CRLF in clientHeader", true);
		_errorCode = 400;
		return false;
	}
	firstLine = _message.substr(0,firstLineEnd);
	_requestLine = firstLine;
	return true;
}


bool ClientRequestHeader::_fillRequestStruct(void)
{
	std::vector<std::string> firstLineStrings = ParsingUtils::splitString(_requestLine, ' ');
	if(firstLineStrings.size() != 3)
	{
		Logger::warning("there is no 3 elements in _requestLine", true);
		_errorCode = 400;
		return false;
	}
	_requestLineElements.requestMethod = firstLineStrings[0];
	_requestLineElements.requestTarget = firstLineStrings[1];
	_requestLineElements.protocolVersion = firstLineStrings[2];
	return true;
}

bool ClientRequestHeader::_checkRequestStruct(void)
{
	const std::string validMethods[] = {"GET", "POST", "DELETE"};
	bool valid = ParsingUtils::isStringValid(_requestLineElements.requestMethod, validMethods, 3);
	if(valid == false)
	{
		Logger::warning("Not valid method found: ", false); std::cerr << _requestLineElements.requestMethod << std::endl;
		_errorCode = 405;
		return false;
	}
	if(_requestLineElements.protocolVersion != "HTTP/1.1")
	{
		Logger::warning("Not valid protocol", true);
		_errorCode = 505;
		return false;
	}
	return true;
}

bool ClientRequestHeader::_setHost(void)
{
	std::string HostLine;
	size_t hosPos = _message.find("Host:");
	if(hosPos == std::string::npos)
	{
		Logger::warning("Invalid header", true);
		_errorCode = 400;
		return false;
	}
	size_t endHos = _message.find("\r\n", hosPos);
	if(endHos == std::string::npos)
	{
		Logger::warning("Invalid header end of host", true);
		_errorCode = 400;
		return false;
	}
	HostLine = _message.substr(hosPos, endHos - hosPos);
	std::string plainHost = ParsingUtils::getHttpPlainValue(HostLine.substr(HostLine.find(':') + 1));
	std::vector<std::string> strings  = ParsingUtils::splitString(plainHost, ':');
	_host.name = strings[0];
	if(strings.size() == 1)
	{
		_host.port = 80;
	}
	else if(strings.size() == 2)
	{
		_host.port = ParsingUtils::stringToSizeT(strings[1]);
	}
	else 
	{
		Logger::warning("Found more than one port", true);
		_errorCode = 400;
		return false;
	}
	return true;
}
