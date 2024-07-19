#include "ResponseBody.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "../Utils/HttpStatusCode.hpp"
#include "../Parsing/ParsingUtils.hpp"
#include "../Utils/Logger.hpp"
#include "../Utils/FileUtils.hpp"

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
        //_response = _generateErrorPage(clientHeader.getErrorCode());
        _renderServerErrorPage(clientHeader.getErrorCode());
        Logger::info("Generated Error page with code :"); std::cout << _httpStatusCode << std::endl;
		//std::cout << _generateErrorPage(clientHeader.getErrorCode());
	}
	else   
	{
        Logger::error("GENERATING SERVER RESPONSE NOT IMPLEMENTED YET", true);
		_generateServerResponse();
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

    /*
        check if server have that error page 
        if yes 
            put that file in string 
            if that fails 
                generate errorPage(500) 
        if not 
            generate that ServerErrorPage
    */
void ResponseBody::_renderServerErrorPage(int errorCode)
{
    std::string errorPagePath = _server.getErrorPagePath(errorCode);
    if(errorPagePath != "")
    {
        Logger::info("Server have that page and path to it is "); std::cout << errorPagePath << std::endl;
        bool success = FileUtils::putFileInString(errorPagePath, _response);
        if(success == true)
            _httpStatusCode = errorCode;
        else
        {
            _response = _generateErrorPage(500);
        }
    }
    else 
    {
        Logger::info("Server dont have page for error code "); std::cout << errorCode << std::endl;
        _response = _generateErrorPage(errorCode);
        Logger::info("Response body is generated and it is", true);
        std::cout << _response << std::endl;
    }
}

void ResponseBody::_handlerGetMethod()
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
        const LocationSettings& location = *it;
        Logger::info("Location found", true);
       // it->printAllSettings();
       // Logger::warning("MORE TO IMPELEMT HERE", true);
       _processRequestedLocation(location);
    }
    else
    {
        _renderServerErrorPage(404);
        Logger::warning("Location not found");
    }
}


void ResponseBody::_handleRedirect(const NginnxReturn& redirect)
{
    Logger::info("Handling redirect: ");
    redirect.printNginxReturnInfo();
    bool success = FileUtils::putFileInString(redirect.getRedirectPath(), _response);
    if(success == true)
    {
        Logger::info("Succesfully redirected", true);
        _httpStatusCode = redirect.getStatus();
    }
    else
    {
        Logger::warning("Redirection is not succesfull, not sure which code so 410 is generated", true);
        _renderServerErrorPage(410);
    }
}
/*
HANDLER GET METHOD 
    1. FIND WHICH LOCATION CLIENT WANTS 
    2. CHECK IF IT IS REDIRECTED
        3.HANDLE REDIRECT 
            return NNGINX RETURN
    4. TRY TO ACCESS SITE(CONSTRUCR root/index.html)
    5. IF SUCCESS 
        return 200
    6. GENERTE SERVER ERROR PAGE
*/
void ResponseBody::_processRequestedLocation(const LocationSettings& location)
{
    Logger::info("I am processing location: ");
    location.printLocationSettings();
    const NginnxReturn& redirect = location.getNginxReturn();
    if(redirect.getFlag() == true)
    {
        _handleRedirect(redirect);
        //handle redirect
    }
    //access this site
}

void ResponseBody::_generateServerResponse()
{
    if(_clientHeader.getRequestLine().protocolVersion != "HTTP/1.1")
    {
        _renderServerErrorPage(505);
    }
    std::string requstedMethod = _clientHeader.getRequestLine().requestMethod;
    if(requstedMethod == "GET")
    {
        std::cout << "Calling Get handler" << std::endl;
        _handlerGetMethod();
    }
    else if(requstedMethod == "POST")
    {
        Logger::error("Not implemeted method yet :"); std::cout << requstedMethod << std::endl;
    }
    else if(requstedMethod == "DELETE")
    {
        Logger::error("Not implemeted method yet :"); std::cout << requstedMethod << std::endl;
    }
}

const std::string& ResponseBody::getResponse(void) const 
{
    return (_response);
}


const int& ResponseBody::getHttpStatusCode(void) const 
{
    return (_httpStatusCode);
}
