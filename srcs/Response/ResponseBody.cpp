#include "ResponseBody.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include "../Utils/HttpStatusCode.hpp"
#include "../Parsing/ParsingUtils.hpp"
#include "../Utils/Logger.hpp"
#include "../Utils/FileUtils.hpp"

ResponseBody::ResponseBody(const ClientHeader& clientHeader, const ServerSettings& server)
:_clientHeader(clientHeader), _server(server), _httpStatusCode(0)
{
    // if(clientHeader.isFullyRead() == false)
    // {
    //     throw std::runtime_error("Trying to create Resonse body without fully read header");
    // }
    Logger::info("Called response body constructor: ");
	std::cout << clientHeader << std::endl;
	if(clientHeader.getErrorCode() != 0)
	{
        _renderServerErrorPage(clientHeader.getErrorCode());
        Logger::info("Generated Error page with code :"); std::cout << _httpStatusCode << std::endl;
	}
	else   
	{
        Logger::warning("GENERATING SERVER RESPONSE NOT fully IMPLEMENTED YET", true);
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
    std::string clientRequestUrl = _clientHeader.getRequestedUrl();
    Logger::info("Requsted url is "); std::cout << clientRequestUrl << std::endl;
    std::string serverLocation = _server.getLocationPartOfUrl(clientRequestUrl);
    Logger::info("Server location resposible for reponse is " + serverLocation, true);
    bool found = true;
 //   Logger::info("Server that is responding is: "); std::cout << std::endl;
    //_server.printServerSettings();
    std::vector<LocationSettings>::const_iterator it = _server.fetchLocationWithUri(serverLocation, found);
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
        Logger::warning("Location not found: "); std::cout << clientRequestUrl << std::endl;
        _renderServerErrorPage(404);
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
    Logger::info("I am processing location: "); std::cout << location.getLocationUri() << std::endl;
   // location.printLocationSettings();
    const NginnxReturn& redirect = location.getNginxReturn();
    if(redirect.getFlag() == true)
    {
        _handleRedirect(redirect);
        return;
        //handle redirect
    }
    if(location.isMethodAllowed("GET"))
        _fetchServerPage(location);
    else
        _renderServerErrorPage(403);
    //access this site
}

bool ResponseBody::_setFilePath(std::string& path, const LocationSettings& location) const
{
    path.erase();
    std::string requestedUrl = _clientHeader.getRequestedUrl();
    std::cout << "Requested url is " << requestedUrl << std::endl;
    std::cout << "Location uri is" << location.getLocationUri();
    std::string baseName = ParsingUtils::getBaseName(requestedUrl, location.getLocationUri());
    Logger::info("base name is " + baseName, true);
    if(baseName == "")
        return location.setIndexPagePath(path);
    std::string filePath = location.getRoot() + "/" + baseName; 
    path = filePath;
    std::cout << "File Path is [" << path << "]" << std::endl;
    return FileUtils::isPathValid(filePath);
}

void ResponseBody::_fetchServerPage(const LocationSettings& location)
{
    Logger::info("Called fetching page", true);
   // location.printLocationSettings();
    std::string path;
    bool found = _setFilePath(path, location);
    if(found == true)
    {
        bool success = FileUtils::putFileInString(path, _response);
        if(success == true)
            _httpStatusCode = 200;
        else
            _renderServerErrorPage(500);
    }
    else if(found == false)
    {
        _renderServerErrorPage(404);
    }
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
