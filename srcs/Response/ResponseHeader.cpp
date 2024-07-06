#include "ResponseHeader.hpp"
#include "../Utils/HttpStatusCode.hpp"
#include "../Parsing/ParsingUtils.hpp"
#include <cstddef>
#include <ostream>

ResponseHeader::ResponseHeader(int& httpCode, size_t contentLength)
:_httpCode(httpCode)
{
	_fillStatusLineElements();
	_headerFields["Connection"] = "close";
	_headerFields["Content-Length"] = contentLength;
	if(contentLength != 0)
		_headerFields["Content_Language"] = "en";
}

ResponseHeader::ResponseHeader(const ResponseHeader& source)
:_httpCode(source._httpCode), _statusLine(source._statusLine), _headerFields(source._headerFields)
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
	statusLine += ParsingUtils::intToString(_statusLine.statusCode);
	statusLine += " ";
	statusLine += _statusLine.ReasonPhrase;
	statusLine += "\r\n";
	return statusLine;
}

std::string ResponseHeader::_getOneHeaderFieldAsString(std::string key, std::string value) const 
{
	std::string oneHeaderField;
	oneHeaderField += key;
	oneHeaderField += ": ";
	oneHeaderField += value;
	oneHeaderField += "\r\n";
	return oneHeaderField;
}

// "Content-Type: text/html\r\n"
// "Content-Length: 48\r\n"
// "Connection: close\r\n"
std::string ResponseHeader::_getAllHeaderFieldsAsString() const
{
	std::string headerFields;
	std::map<std::string, std::string>::const_iterator it;
	for(it = _headerFields.begin(); it != _headerFields.end(); it++)
	{
		headerFields += _getOneHeaderFieldAsString(it->first, it->second);
	}
	return headerFields;
}



std::string ResponseHeader::turnResponseHeaderToString(void) const
{
	std::string fullHeader;
	fullHeader += _getStatusLineAsString();
	fullHeader += _getAllHeaderFieldsAsString();
	return fullHeader;
}

std::ostream& operator<<(std::ostream& os, const ResponseHeader& obj)
{
	os << "Response header is " << std::endl;
	os << obj.turnResponseHeaderToString();
	return os;
}
