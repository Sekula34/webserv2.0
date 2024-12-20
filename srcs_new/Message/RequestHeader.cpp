#include "RequestHeader.hpp"
#include <cstddef>
#include <string>
#include <iostream>
#include "../Parsing/ParsingUtils.hpp"
#include "../Utils/Logger.hpp"
#include "AHeader.hpp"
#include "../Utils/FileUtils.hpp"

std::string RequestHeader::getFullRequest(const std::string& message)
{
	std::string fullRequest = ParsingUtils::extractUntilDelim(message, "\r\n\r\n");
	return fullRequest;
}

std::string RequestHeader::getHeaderSectionString(const std::string& message)
{
	std::string fullRequest = getFullRequest(message);
	std::string headerSection = fullRequest;
	std::string delimiter = "\r\n";
	std::string secondDelim = "\r\n\r\n";
	size_t posFirstNewLine = fullRequest.find(delimiter);
	if(posFirstNewLine != std::string::npos)
	{
		headerSection = fullRequest.substr(posFirstNewLine + delimiter.size());
		size_t posOfSecondDelim = headerSection.find(secondDelim);
		headerSection = headerSection.substr(0, posOfSecondDelim + 2);
	}
	return headerSection;
}

static bool	isValidRequestLine(const std::string& str)
{
	std::vector<std::string> values = ParsingUtils::splitString(str, " ");
	if (values.size() < 3)
		return (false);
	if (values[0] != "GET" && values[0] != "POST" && values[0] != "DELETE")
		return (false);
	if (values[2] != "HTTP/1.1")
		return (false);
	return (true);
}

bool	RequestHeader::checkHeaderByLine(const std::string str)
{
	std::vector<std::string> lines = ParsingUtils::splitString(str, "\r\n");
	lines.pop_back();
	std::vector<std::string>::const_iterator it = lines.begin();
	if (isValidRequestLine(*it) == false)
		return (false);
	it++;
	while(it != lines.end())
	{
		std::string key = ParsingUtils::extractUntilDelim(*it, ":");
		if(key != "")
			key.erase(key.end() - 1);
		std::string plainKey = ParsingUtils::getHttpPlainValue(key);
		std::string value = ParsingUtils::extractAfterDelim(*it, ":");
		value = ParsingUtils::getHttpPlainValue(value);
		if(key == "" || value == "")
			return (false);
		it++;
	}
	return (true);
}

RequestHeader::RequestHeader(const std::string message, int& errorCode)
:AHeader(getHeaderSectionString(message), errorCode),
_fullRequest(getFullRequest(message))
{
	_constructFunction();
}

RequestHeader::RequestHeader(const RequestHeader& source)
:AHeader(getHeaderSectionString(source.m_headerSection), source.m_errorCode),
_fullRequest(getFullRequest(source.getFullMessage()))
{

}


RequestHeader& RequestHeader::operator=(const RequestHeader& source)
{
	(void) source;
	Logger::warning("Give me good reason for this RequestHeader operator =", "");
	return  *this;
}

RequestHeader::~RequestHeader()
{
	delete urlSuffix;
}

std::string RequestHeader::getStartLine() const 
{
	return _requestLine;
}

const int& RequestHeader::getHostPort(void) const 
{
	return(_host.port);
}

const std::string& RequestHeader::getHostName(void) const 
{
	return(_host.name);
}

const std::string& RequestHeader::getFullMessage(void) const
{
	return (_fullRequest);
}


const RequestLine& RequestHeader::getRequestLine() const 
{
	return (_requestLineElements);
}

const std::string& RequestHeader::getURLSuffix(void) const 
{
	const RequestLine& line = getRequestLine();
	return line.requestTarget;
}

std::string RequestHeader::getFullClientURL() const
{
	std::ostringstream oss;
	oss << "http://";
	oss << getHostName() <<":"<< getHostPort();
	oss << getURLSuffix();
	return oss.str();
}

void RequestHeader::_constructFunction()
{
	if(_fullRequest == "")
		Logger::warning("Tried to create ClientRequest header with string that does not contain CRLFCRLF",true);
	_initRequestVars();
	if(_setReqVariables() == true)
		urlSuffix = new UrlSuffix(getURLSuffix());
	else
	{
		p_setHttpStatusCode(400);
		return;
	}
	_checkRequestStruct();
}

void RequestHeader::_initRequestVars(void)
{
	urlSuffix = NULL;
	_requestLine= "";
	_requestLineElements.requestMethod = "";
	_requestLineElements.requestTarget= "";
	_requestLineElements.protocolVersion = "";
}

bool RequestHeader::_setReqVariables()
{
	if(_setRequestLine() == false)
		return false;
	if(_fillRequestStruct() == false)
		return false;
	if(_setHost() == false)
		return false;
	return true;
}

bool RequestHeader::_setRequestLine(void) 
{
	std::string firstLine;
	size_t firstLineEnd =_fullRequest.find("\r\n");
	if(firstLineEnd == std::string::npos)
	{
		Logger::warning("Cannot find CRLF in clientHeader", true);
		return false;
	}
	firstLine = _fullRequest.substr(0,firstLineEnd);
	_requestLine = firstLine;
	return true;
}

bool RequestHeader::_fillRequestStruct(void)
{
	std::vector<std::string> firstLineStrings = ParsingUtils::splitString(_requestLine, ' ');
	if(firstLineStrings.size() != 3)
	{
		Logger::warning("there is no 3 elements in _requestLine", true);
		return false;
	}
	_requestLineElements.requestMethod = firstLineStrings[0];
	_requestLineElements.requestTarget = firstLineStrings[1];
	_requestLineElements.protocolVersion = firstLineStrings[2];
	return true;
}
bool RequestHeader::_setHost(void)
{
	std::map<std::string, std::string>::const_iterator it = m_headerFields.find("Host");
	if(it == m_headerFields.end())
		return false;
	std::string hostLine = it->second;
	std::string plainHost = ParsingUtils::getHttpPlainValue(hostLine);
	std::vector<std::string> strings  = ParsingUtils::splitString(plainHost, ':');
	_host.name = strings[0];
	if(strings.size() == 1)
		_host.port = 80;
	else if(strings.size() == 2)
		_host.port = ParsingUtils::stringToSizeT(strings[1]);
	else 
	{
		Logger::warning("Found more than one port", true);
		return false;
	}
	return true;
}

bool RequestHeader::_checkRequestStruct(void)
{
	const std::string validMethods[] = {"GET", "POST", "DELETE"};
	bool valid = ParsingUtils::isStringValid(_requestLineElements.requestMethod, validMethods, 3);
	if(valid == false)
	{
		Logger::warning("Not valid method found: ", _requestLineElements.requestMethod);
		p_setHttpStatusCode(405);
		return false;
	}
	if(_requestLineElements.protocolVersion != "HTTP/1.1")
	{
		Logger::warning("Not valid protocol", true);
		p_setHttpStatusCode(505);
		return false;
	}
	if(FileUtils::isValidName(_requestLineElements.requestTarget) == false)
	{
		Logger::error("Requested location contain path traversal or is empty ", _requestLineElements.requestTarget);
		p_setHttpStatusCode(400);
		return false;
	}
	return true;
}

std::ostream& operator<<(std::ostream& os, const RequestHeader& obj)
{
	os << obj.getStartLine() << std::endl;
	const AHeader& base = obj;
	os << base;
	// os << dynamic_cast<AHeader>(obj);
	// std::map<std::string, std::string>::const_iterator it = obj.m_headerFields.begin();
	// for(;it != obj.m_headerFields.end(); it++)
	// {
	// 	os << it->first << ": " << it->second << std::endl;
	// }
	return os;
}
