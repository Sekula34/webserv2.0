#include "ResponseBody.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "../Utils/HttpStatusCode.hpp"
#include "../Parsing/ParsingUtils.hpp"

ResponseBody::ResponseBody(const ClientHeader& clientHeader, const ServerSettings* server)
:_clientHeader(clientHeader), _server(server), _httpStatusCode(0)
{
    if(clientHeader.isFullyRead() == false)
    {
        throw std::runtime_error("Trying to create Resonse body without fully read header");
    }
    std::cout << "Created Response body:" << std::endl;
	std::cout << clientHeader << std::endl;
	if(_server != NULL)
		std::cout << _server << std::endl;
	else 
		std::cout << "Server is NULL" << std::endl;
	if(clientHeader.getErrorCode() != 0)
	{
        _response = _generateErrorPage(clientHeader.getErrorCode());
        _httpStatusCode = clientHeader.getErrorCode();
		//std::cout << _generateErrorPage(clientHeader.getErrorCode());
	}
	else   
	{
		//generateServerResponse
	}
}

ResponseBody::ResponseBody(const ResponseBody& source)
:_clientHeader(source._clientHeader), _server(source._server), _response(source._response)
{

}
ResponseBody& ResponseBody::operator=(const ResponseBody& source)
{
	(void) source;
	return (*this);
}

ResponseBody::~ResponseBody()
{

}

std::string ResponseBody::_generateErrorPage(const int httpErrorCode)
{
	std::ostringstream errorPage;
	errorPage << 
	"<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "    <title>" << ParsingUtils::toString(httpErrorCode) + " " + HttpStatusCode::getReasonPhrase(httpErrorCode) << "</title>\n"
    "    <style>\n"
    "        body {\n"
    "            text-align: center;\n"
    "            padding: 20px;\n"
    "        }\n"
    "        h1 {\n"
    "            font-size: 50px;\n"
    "        }\n"
    "        body {\n"
    "            font: 20px Helvetica, sans-serif;\n"
    "            color: #333;\n"
    "        }\n"
    "    </style>\n"
    "</head>\n"
    "<body>\n"
    "    <h1>" <<ParsingUtils::toString(httpErrorCode) << "</h1>\n"
    "    <p>" << HttpStatusCode::getReasonPhrase(httpErrorCode) << "</p>\n"
    "</body>\n"
    "</html>\n";
	return  errorPage.str();
}

const std::string& ResponseBody::getResponse(void) const 
{
    return (_response);
}


const int& ResponseBody::getHttpStatusCode(void) const 
{
    return (_httpStatusCode);
}
