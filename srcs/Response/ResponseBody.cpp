#include "ResponseBody.hpp"
#include <iostream>
#include "../Utils/HttpStatusCode.hpp"
#include "../Parsing/ParsingUtils.hpp"

ResponseBody::ResponseBody(const ClientHeader& clientHeader, const ServerSettings* server)
:_clientHeader(clientHeader), _server(server)
{
	std::cout << "Created Response body:" << std::endl;
	std::cout << clientHeader << std::endl;
	if(_server != NULL)
		std::cout << _server << std::endl;
	else 
		std::cout << "Server is NULL" << std::endl;
	if(clientHeader.getErrorCode() != 0)
	{
		//std::cout << _generateErrorPage(clientHeader.getErrorCode());
	}
	else   
	{
		//generateServerResponse
	}
}

ResponseBody::ResponseBody(const ResponseBody& source)
:_clientHeader(source._clientHeader), _server(source._server)
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
	std::string errorPage =
	"<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "    <title>";
	errorPage += ParsingUtils::intToString(httpErrorCode) + " " + HttpStatusCode::getReasonPhrase(httpErrorCode);
	errorPage += "</title>\n"
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
    "    <h1>";
	errorPage += ParsingUtils::intToString(httpErrorCode);
	errorPage += "</h1>\n"
    "    <p>";
	errorPage += HttpStatusCode::getReasonPhrase(httpErrorCode);
	errorPage +="</p>\n"
    "</body>\n"
    "</html>\n";
	return errorPage;
}
