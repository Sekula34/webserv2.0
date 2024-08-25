#include "ClientHeader.hpp"
#include "../Utils/Logger.hpp"
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>
#include "../Parsing/ParsingUtils.hpp"

ClientHeader::ClientHeader(const std::string message)
:_message(ParsingUtils::extractUntilDelim(message, "\r\n\r\n"))
{
	_constructFunction();
}

ClientHeader::ClientHeader(const ClientHeader& source)
:_message(source._message)
{
	_constructFunction();
}

ClientHeader& ClientHeader::operator=(const ClientHeader& source)
{
	(void)source;
	Logger::warning("Give me really good reason why you want this");
	return (*this);
}

ClientHeader::~ClientHeader()
{
	delete urlSuffix;
}


void ClientHeader::_initAllVars(void)
{
	urlSuffix = NULL;
	_errorCode = 0;
	_requestLine= "";

	_requestLineElements.requestMethod = "";
	_requestLineElements.requestTarget= "";
	_requestLineElements.protocolVersion = "";
	
	_host.name = "";
	_host.port = 0;
}

bool ClientHeader::_setCHVarivables()
{
	if(_setRequestLine() == false)
		return false;
	if(_fillRequestStruct() == false)
		return false;
	if(_setHost() == false)
		return false;
	if(_checkRequestStruct() == false)
		return false;
	if(_setHeaderFields() == false)
		return false;
	if(_checkHeaderFields() == false)
		return false;
	return true;
}

bool ClientHeader::_setHeaderFields()
{
	std::vector<std::string> plainHeaders = _getHeaderFields();
	for(size_t i = 0; i < plainHeaders.size(); i++)
	{
		_setOneHeader(plainHeaders[i]);
	}
	return true;
	
}

void ClientHeader::_setOneHeader(std::string keyAndValue)
{
	std::vector<std::string> connected = ParsingUtils::splitString(keyAndValue, ':');
	if(connected.size() < 2)
	{
		Logger::warning("Header filed syntax is in client Header is invalid ");
		_errorCode = 400;
	}
	std::string key = connected[0];
	std::string value = ParsingUtils::getHttpPlainValue(connected[1]);
	_headerFields[key] = value;
}

std::vector<std::string> ClientHeader::_getHeaderFields(void) const 
{
	std::vector<std::string> headerFields =  ParsingUtils::splitString(_message, "\r\n");
	std::vector<std::string>::iterator last = headerFields.end() - 1;
	headerFields.erase(last);
	last = headerFields.end() - 1;
	headerFields.erase(last);
	headerFields.erase(headerFields.begin());
	return headerFields;
} 

const int& ClientHeader::getHostPort(void) const 
{
	return(_host.port);
}

const std::string& ClientHeader::getHostName(void) const 
{
	return(_host.name);
}

const std::string& ClientHeader::getFullMessage(void) const
{
	return (_message);
}


const RequestLine& ClientHeader::getRequestLine() const 
{
	return (_requestLineElements);
}


const int& ClientHeader::getErrorCode(void) const 
{
	return _errorCode;
}

const std::string& ClientHeader::getURLSuffix(void) const 
{
	const RequestLine& line = getRequestLine();
	return line.requestTarget;
}

std::string ClientHeader::getFullClientURL() const
{
	std::ostringstream oss;
	oss << "http://";
	oss << getHostName() <<":"<< getHostPort();
	oss << getURLSuffix();
	return oss.str();
}

const std::map<std::string, std::string> & ClientHeader::getHeaderFields() const
{
	return (_headerFields);
}

bool ClientHeader::isBodyExpected() const
{
	if(_headerFields.find("Content-Length") != _headerFields.end())
		return true;
	return false;
}


void ClientHeader::_constructFunction()
{
	if(_message == "")
		Logger::warning("Tried to create ClientRequest header with string that does not contain CRLFCRLF",true);
	_initAllVars();
	if(_setCHVarivables() == true)
	{
		urlSuffix = new UrlSuffix(getURLSuffix());
	}
}

bool ClientHeader::_setRequestLine(void) 
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


bool ClientHeader::_fillRequestStruct(void)
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

bool ClientHeader::_checkRequestStruct(void)
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

bool ClientHeader::_checkHeaderFields(void)
{
	//check if there is authorization
	std::string authorizationKey = "Authorization";
	std::map<std::string, std::string>::iterator it = _headerFields.find((authorizationKey));
	if(it != _headerFields.end())
	{
		Logger::error("Authorization is not supported", true);
		_errorCode = 403;
		return false;
	}
	return true;
}


bool ClientHeader::_setHost(void)
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

std::ostream& operator<<(std::ostream& os, const ClientHeader& obj)
{
	os<< "Client message data" << std::endl;
	os<< "Request method is :" << obj._requestLineElements.requestMethod << std::endl;
	os<< "Request target is :" << obj._requestLineElements.requestTarget << std::endl;
	os<< "Request protocol Version is :" << obj._requestLineElements.protocolVersion << std::endl;
	os<< "Host name is :" << obj._host.name << std::endl;
	os<< "Host port is :" << obj._host.port << std::endl;
	//os<< "Message is :[" << obj._message << "]";
	return os;
}
