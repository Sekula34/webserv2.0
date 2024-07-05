#include "HttpStatusCode.hpp"
#include <iostream>
#include <map>
#include "Logger.hpp"

std::map<int, std::string> HttpStatusCode::_statusCode = _getFilledMap();

std::map<int, std::string> HttpStatusCode::_getFilledMap()
{
	std::map<int, std::string> filledMap;
    filledMap[100] = "Continue";
    filledMap[101] = "Switching Protocols";
    
	filledMap[200] = "OK";
    filledMap[201] = "Created";
    filledMap[202] = "Accepted";
    filledMap[203] = "Non-Authoritative Information";
    filledMap[204] = "No Content";
    filledMap[205] = "Reset Content";
    filledMap[206] = "Partial Content";
    
	filledMap[300] = "Multiple Choices";
    filledMap[301] = "Moved Permanently";
    filledMap[302] = "Found";
    filledMap[303] = "See Other";
    filledMap[304] = "Not Modified";
    filledMap[305] = "Use Proxy";
    filledMap[307] = "Temporary Redirect";
    
	filledMap[400] = "Bad Request";
    filledMap[401] = "Unauthorized";
    filledMap[402] = "Payment Required";
    filledMap[403] = "Forbidden";
    filledMap[404] = "Not Found";
    filledMap[405] = "Method Not Allowed";
    filledMap[406] = "Not Acceptable";
    filledMap[407] = "Proxy Authentication Required";
    filledMap[408] = "Request Timeout";
    filledMap[409] = "Conflict";
    filledMap[410] = "Gone";
    filledMap[411] = "Length Required";
    filledMap[412] = "Precondition Failed";
    filledMap[413] = "Payload Too Large";
    filledMap[414] = "URI Too Long";
    filledMap[415] = "Unsupported Media Type";
    filledMap[416] = "Range Not Satisfiable";
    
	filledMap[417] = "Expectation Failed";
    filledMap[500] = "Internal Server Error";
    filledMap[501] = "Not Implemented";
    filledMap[502] = "Bad Gateway";
    filledMap[503] = "Service Unavailable";
    filledMap[504] = "Gateway Timeout";
    filledMap[505] = "HTTP Version Not Supported";

	return filledMap;
}

std::string HttpStatusCode::getReasonPhrase(int code)
{
	std::map<int, std::string>::iterator it;
	it = _statusCode.find(code);
	if(it == _statusCode.end())
	{
		Logger::error("Cannot find ReasonPhrase with code: "); std::cerr << code << std::endl;
		throw UnkownCodeException();
	}
	return it->second;
}

const char* HttpStatusCode::UnkownCodeException::what() const throw()
{
	return ("Unkown Reason phrase");
}
