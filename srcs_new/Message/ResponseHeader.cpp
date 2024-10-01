#include "ResponseHeader.hpp"
#include "../Utils/HttpStatusCode.hpp"
#include "../Utils/Logger.hpp"
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
	_httpCode = errorCode;
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

static ResponseHeader*	cgiDelimitersToHttpDelimiters(std::string& cgiHeaderStr, const std::string& cgiHeaderFieldDelimiter, const std::string& cgiHeaderDelimiter, int& clientError)
{
	size_t pos = cgiHeaderStr.find(cgiHeaderDelimiter);
	if (pos != std::string::npos)
	{
		cgiHeaderStr.replace(pos, cgiHeaderDelimiter.size(), cgiHeaderFieldDelimiter);
		std::string toReplace = cgiHeaderFieldDelimiter;
		std::string httpDelimiter = "\r\n";
		// Logger::error("cgiHeaderStr: ", cgiHeaderStr);
		// Logger::chars(toReplace, true);
		// Logger::chars(httpDelimiter, true);
		std::string aHeaderStr = ParsingUtils::replaceAllCharsInString(cgiHeaderStr, toReplace, httpDelimiter);
		return (new ResponseHeader(aHeaderStr, clientError));
		// Logger::chars(aHeaderStr, true);
	}
	return (NULL);
}

void	ResponseHeader::cgiToHttpConversion(ResponseHeader* toReturn)
{
	if(toReturn->_cgiStatusLine() == true)
	{
		std::map<std::string, std::string>::iterator it = toReturn->m_headerFields.find("Status");
		std::string cgiStatus = ParsingUtils::extractUntilDelim(it->second, " ", false);
		int newHttpCode = ParsingUtils::stringToSizeT(cgiStatus);
		//FIXME: newHttpCode is http code from CGI, WHAT SHOULD WE DOO if cgi for example provide 600? correct it ? 400 /500/502?
		toReturn->changeHttpCode(newHttpCode);
		toReturn->m_headerFields.erase(it);
	}
}

ResponseHeader* ResponseHeader::createCgiResponseHeader(std::string cgiHeaderStr, int& clientError, const std::string cgiHeaderFieldDelimiter, const std::string cgiHeaderDelimiter)
{	
	ResponseHeader* toReturn = NULL;
	// REPLACE CGI HEADER FIELD DELIMITERS AND CGI HEADER DELIMITERS WITH HTTP DELIMITERS
	toReturn = cgiDelimitersToHttpDelimiters(cgiHeaderStr, cgiHeaderFieldDelimiter, cgiHeaderDelimiter, clientError);

	// IF THE CGI HEADER HAS NO CGI DELIMITER, CREATE AN EMPTY HEADER AND SET THE ERR CODE
	if (!toReturn)
		toReturn = new ResponseHeader("\r\n\r\n", clientError);

	// IF ONE SERVER GETS INVALID RESPONSE FROM ANOTHER -> BAD GETAWAY 502
	if(toReturn->getHttpStatusCode() != 0)
		toReturn->p_setHttpStatusCode(502);

	// CONVERT CGI'S HEADER FIELDS INTO HTTP HEADER FIELDS
	cgiToHttpConversion(toReturn);
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
	//rgHeader->setOneHeaderField("Connection", "close");
	//rgHeader->setOneHeaderField("Content-Type", "text/html; charset=utf-8");//TODO: either left it out or decide based on file extension
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
	if(_httpCode == 0) //if _http code is 0 that means client had no error so far so we turn it into 200 ok
		_httpCode = 200;
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
