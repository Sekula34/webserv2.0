#include "ResponseHeader.hpp"
#include "../Utils/HttpStatusCode.hpp"
#include "../Parsing/ParsingUtils.hpp"
#include <cstddef>
#include <ostream>
#include <sstream>

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

std::string ResponseHeader::turnResponseHeaderToString(void) const
{
	std::string fullHeader;
	fullHeader += _getStatusLineAsString();
	fullHeader += p_getAllHeaderFieldsAsString();
	return fullHeader;
}

std::ostream& operator<<(std::ostream& os, const ResponseHeader& obj)
{
	os << "Response header is " << std::endl;
	os << obj.turnResponseHeaderToString();
	return os;
}
