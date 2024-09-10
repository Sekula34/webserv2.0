#include "ResponseHeader.hpp"
#include "../Utils/HttpStatusCode.hpp"
#include "../Parsing/ParsingUtils.hpp"
#include <cstddef>
#include <ostream>
#include <sstream>
#include <vector>


ResponseHeader::ResponseHeader(std::string headerSection, const int httpCode)
:AHeader(headerSection),
_httpCode(httpCode)
{
	_fillStatusLineElements();
}

ResponseHeader::ResponseHeader(const int& httpCode, size_t contentLength)
:_httpCode(httpCode)
{
	_fillStatusLineElements();
	m_headerFields["Connection"] = "close";
	if(contentLength != 0)
	{
		m_headerFields["Content-Length"] = ParsingUtils::toString(contentLength);
		m_headerFields["Content-Language"] = "en";
	}
}

ResponseHeader::ResponseHeader(const ResponseHeader& source)
:AHeader(),
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

ResponseHeader* ResponseHeader::createCgiResponseHeader(std::string cgiResponse, const std::string cgiHeaderDelimiter)
{
	std::string toReplace = cgiHeaderDelimiter;
	std::string httpDelimiter = "\r\n";
	std::string aHeaderString = ParsingUtils::replaceAllCharsInString(cgiResponse, toReplace, httpDelimiter);
	std::string firstLine = ParsingUtils::extractUntilDelim(aHeaderString, httpDelimiter);
	int statusCode;  
	if(_cgiStatusLine(firstLine) == true)
	{
		aHeaderString.erase(0, firstLine.size());
		StatusLineElements elem;
		if(_setStatusLine(elem, firstLine) == true)
			statusCode = elem.statusCode;
		else
		 	statusCode = 500;
	}
	else
		statusCode = 200;
	ResponseHeader* toReturn = new ResponseHeader(aHeaderString, statusCode);
	return toReturn;
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

bool ResponseHeader::_cgiStatusLine(std::string firstLine)
{
	if(firstLine.find(":") == std::string::npos)
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
