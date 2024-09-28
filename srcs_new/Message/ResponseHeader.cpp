#include "ResponseHeader.hpp"
#include "../Utils/HttpStatusCode.hpp"
#include "../Parsing/ParsingUtils.hpp"
#include "AHeader.hpp"
#include <cstddef>
#include <ostream>
#include <sstream>
#include <vector>
#include "../Server/ResponseGenerator.hpp"


ResponseHeader::ResponseHeader()
:AHeader(_httpCode = 0)
{
	
}

ResponseHeader::ResponseHeader(std::string headerSection, int& errorCode)
:AHeader(headerSection, errorCode)
{
	_fillStatusLineElements();
}

// ResponseHeader::ResponseHeader(int& httpCode, size_t contentLength)
// :_httpCode(httpCode)
// {
// 	_fillStatusLineElements();
// 	m_headerFields["Connection"] = "close";
// 	if(contentLength != 0)
// 	{
// 		m_headerFields["Content-Length"] = ParsingUtils::toString(contentLength);
// 		m_headerFields["Content-Language"] = "en";
// 	}
// }

ResponseHeader::ResponseHeader(const ResponseHeader& source)
:AHeader(source.m_errorCode),
_httpCode(source._httpCode),
_statusLine(source._statusLine)
{

}

ResponseHeader& ResponseHeader::operator=(const ResponseHeader& source)
{
	(void) source;
	return (*this);
}

ResponseHeader::~ResponseHeader()
{
}

std::string ResponseHeader::getStartLine() const 
{
	std::ostringstream oss;
	oss << _statusLine.HttpVersion << " " << _statusLine.statusCode << " " << _statusLine.ReasonPhrase;
	return oss.str();
}

std::string ResponseHeader::turnResponseHeaderToString(void) const
{
	std::string fullHeader;
	fullHeader += _getStatusLineAsString();
	fullHeader += p_getAllHeaderFieldsAsString();
	return fullHeader;
}

ResponseHeader* ResponseHeader::createCgiResponseHeader(std::string cgiResponse, int& clientError, const std::string cgiHeaderFieldDelimiter, const std::string cgiHeaderDelimiter)
{
	size_t pos = cgiResponse.find(cgiHeaderDelimiter);
	std::string aHeaderString = "\r\n\r\n";
	ResponseHeader* toReturn;
	if (pos != std::string::npos)
	{
		cgiResponse.replace(pos, cgiHeaderDelimiter.size(), cgiHeaderFieldDelimiter);
		std::string toReplace = cgiHeaderFieldDelimiter;
		std::string httpDelimiter = "\r\n";
		aHeaderString = ParsingUtils::replaceAllCharsInString(cgiResponse, toReplace, httpDelimiter);
		toReturn = new ResponseHeader(aHeaderString, clientError);
	}
	else
		toReturn = new ResponseHeader(aHeaderString, clientError);
	if(toReturn == NULL || toReturn->getHttpStatusCode() != 0)
	{
		// if one server gets invalid response for anothe -> bad getaway 502
		if (toReturn)
			toReturn->p_setHttpStatusCode(502);
		return toReturn;
	}
	if(toReturn->_cgiStatusLine() == true)
	{
		std::cout << "I have status line " << std::endl;
		std::map<std::string, std::string>::iterator it = toReturn->m_headerFields.find("Status");
		std::string cgiStatus = ParsingUtils::extractUntilDelim(it->second, " ", false);
		int newHttpCode = ParsingUtils::stringToSizeT(cgiStatus);
		toReturn->changeHttpCode(newHttpCode);
	}
	return toReturn;
}

ResponseHeader* ResponseHeader::createRgResponseHeader(const ResponseGenerator &rg)
{
	
	ResponseHeader* rgHeader = new ResponseHeader();
	rgHeader->changeHttpCode(rg.getResponseHttpStatus());
	if(rg.getRedirect().getFlag() == true)
	{
		rgHeader->setOneHeaderField("Location", rg.getRedirect().getRedirectPath());
	}
	rgHeader->setOneHeaderField("Connection", "close");
	rgHeader->setOneHeaderField("Content-Type", "text/html; charset=utf-8");
	//rgHeader->setOneHeaderField("Content-Type", std::string value)
	//Content-Type: text/html; charset=ISO-8859-1



	return rgHeader;
	//rgHeader->setOneHeaderField(std::string key, std::string value)
}

void ResponseHeader::changeHttpCode(int newHttpCode)
{
	_httpCode = newHttpCode;
	_fillStatusLineElements();
}

bool ResponseHeader::_setStatusLine(StatusLineElements& elem, std::string line)
{
	std::vector<std::string> elements = ParsingUtils::splitString(line, ' ');
	if(elements.size() != 3)
		return false;
	elem.HttpVersion = elements[0];
	elem.statusCode = ParsingUtils::stringToSizeT(elements[1]);
	elem.ReasonPhrase = HttpStatusCode::getReasonPhrase(elem.statusCode);
	return true;
}

bool ResponseHeader::_cgiStatusLine() const
{
	std::map<std::string, std::string>::const_iterator it = m_headerFields.find("Status");
	if(it != m_headerFields.end())
		return true;
	return false;
}

void ResponseHeader::_fillStatusLineElements()
{
	_statusLine.HttpVersion = "HTTP/1.1";
	_statusLine.statusCode = _httpCode;
	_statusLine.ReasonPhrase =  HttpStatusCode::getReasonPhrase(_httpCode);
}

//"HTTP/1.1 200 OK\r\n"
std::string ResponseHeader::_getStatusLineAsString() const
{
	std::string statusLine;
	statusLine += _statusLine.HttpVersion;
	statusLine += " ";
	statusLine += ParsingUtils::toString(_statusLine.statusCode);
	statusLine += " ";
	statusLine += _statusLine.ReasonPhrase;
	statusLine += "\r\n";
	return statusLine;
}


std::ostream& operator<<(std::ostream& os, const ResponseHeader& obj)
{
	os << "Response header is " << std::endl;
	os << obj.turnResponseHeaderToString();
	return os;
}
