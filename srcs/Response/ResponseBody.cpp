#include "ResponseBody.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "../Utils/HttpStatusCode.hpp"
#include "../Parsing/ParsingUtils.hpp"
#include "../Utils/Logger.hpp"

ResponseBody::ResponseBody(const ClientHeader& clientHeader, const ServerSettings& server)
:_clientHeader(clientHeader), _server(server), _httpStatusCode(0)
{
    if(clientHeader.isFullyRead() == false)
    {
        throw std::runtime_error("Trying to create Resonse body without fully read header");
    }
    Logger::info("Called response body constructor: ");
	std::cout << clientHeader << std::endl;
	if(clientHeader.getErrorCode() != 0)
	{
        _response = _generateErrorPage(clientHeader.getErrorCode());
        Logger::info("Generated Error page with code :"); std::cout << _httpStatusCode << std::endl;
		//std::cout << _generateErrorPage(clientHeader.getErrorCode());
	}
	else   
	{
        Logger::error("GENERATING SERVER RESPONSE NOT IMPLEMENTED YET", true);
		_response =_generateServerResponse();
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
    _httpStatusCode = httpErrorCode;
	return  errorPage.str();
}

void ResponseBody::_renderServerErrorPage(int errorCode)
{
    std::string errorPagePath = _server.getErrorPagePath(errorCode);
    if(errorPagePath != "")
    {
        Logger::info("Server have that page and path to it is "); std::cout << errorPagePath << std::endl;
    }
    else 
    {
        Logger::info("Server dont have page for error code "); std::cout << errorCode << std::endl;
    }
    /*
        check if server have that error page 
        if yes 
            put that file in string 
            if that fails 
                generate errorPage(500) 
        if not 
            generate that ServerErrorPage
    */
}

int ResponseBody::_handlerGetMethod()
{
    Logger::info("Handling GET, ServerLocation", true);
    std::string clientRequestUri = _clientHeader.getRequestLine().requestTarget;
    Logger::info("Requsted location is "); std::cout << clientRequestUri << std::endl;
    bool found = true;
    Logger::info("Server that is responding is: "); std::cout << std::endl;
    _server.printServerSettings();
    std::vector<LocationSettings>::const_iterator it = _server.fetchLocationWithUri(clientRequestUri, found);
    if(found == true)
    {
        Logger::info("Location found");
        it->printAllSettings();
    }
    else
    {
        _renderServerErrorPage(404);
        //Generate Server Error(code)
        Logger::warning("Location not found");
    }
    return 200;
}

std::string ResponseBody::_generateServerResponse()
{
    if(_clientHeader.getRequestLine().protocolVersion != "HTTP/1.1")
        return _generateErrorPage(505);
    std::string requstedMethod = _clientHeader.getRequestLine().requestMethod;
    if(requstedMethod == "GET")
    {
        std::cout << "Calling Get handler" << std::endl;
        _handlerGetMethod();
    }
    else if(requstedMethod == "GET")
    {
        Logger::error("Not implemeted method yet :"); std::cout << requstedMethod << std::endl;
    }
    else if(requstedMethod == "GET")
    {
        Logger::error("Not implemeted method yet :"); std::cout << requstedMethod << std::endl;
    }
    return "";
}

const std::string& ResponseBody::getResponse(void) const 
{
    return (_response);
}


const int& ResponseBody::getHttpStatusCode(void) const 
{
    return (_httpStatusCode);
}
