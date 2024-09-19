#include "ResponseBody.hpp"
#include <cstddef>
#include <iostream>
#include <sstream>
#include <vector>
#include "../Utils/HttpStatusCode.hpp"
#include "../Parsing/ParsingUtils.hpp"
#include "../Utils/Logger.hpp"
#include "../Utils/FileUtils.hpp"
#include "../Client/Client.hpp"
#include "../Client/Message.hpp"
#include "Autoindex.hpp"

class Client;

ResponseBody::ResponseBody(const Client& client, const ServerSettings* server)
:_client(client), _server(server), _httpStatusCode(0)
{
    if(client.getErrorCode() == 400 || server == NULL)
    {
        _generateErrorPage(400);
        return;
    }

	if(client.getErrorCode() != 0)
	{
        _renderServerErrorPage(client.getErrorCode());
        Logger::info("Generated Error page with code :"); std::cout << _httpStatusCode << std::endl;
    }else if(client.getCgi())
    {
        _httpStatusCode = 200;
        _response = client.getServerMsg()->getUnchunkedBodyString();
    }else   
	{
        Logger::warning("GENERATING SERVER RESPONSE NOT fully IMPLEMENTED YET", true);
		_generateServerResponse();
	}
}

ResponseBody::ResponseBody(const ResponseBody& source)
:_client(source._client), _server(source._server), _response(source._response)
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
    std::string errorPagePath = "";
    if(_server != NULL)
        errorPagePath = _server->getErrorPagePath(errorCode);
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
    std::string path = static_cast<RequestHeader*>(_client.getClientMsg()->getHeader())->urlSuffix->getPath();
    Logger::info("Requsted url is "); std::cout << path << std::endl;
    std::string serverLocationUri = _server->getLocationURIfromPath(path);
    Logger::info("Server location resposible for reponse is " + serverLocationUri, true);
    bool found = true;
    std::vector<LocationSettings>::const_iterator it = _server->fetchLocationWithUri(serverLocationUri, found);
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
        Logger::warning("Location not found: "); std::cout << path << std::endl;
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
    {
        //_fetchServerPage(location);
        _generateHtml(location);
    }
    else
        _renderServerErrorPage(403);
    //access this site
}

void ResponseBody::_generateHtml(const LocationSettings& location)
{
    std::string serverFilePath = _convertToServerPath(location);
    int result = FileUtils::isPathFileOrFolder(serverFilePath, _httpStatusCode);
    if(result == -1)
    {
        _renderServerErrorPage(_httpStatusCode);
        return;
    }
    if(result == 1)
    {
        _fileHtml(serverFilePath);
        return;
    }
    else
    {
        _dirHtml(serverFilePath, location);
    }
    return;
}

bool ResponseBody::_fileHtml(const std::string& serverFilePath)
{
    bool success = FileUtils::putFileInString(serverFilePath, _response);
    if(success == true)
    {
        _httpStatusCode = 200;
        return true;
    }
    _renderServerErrorPage(404);
    return false;
}

void ResponseBody::_dirHtml(const std::string& serverFilePath, const LocationSettings& location)
{
    (void)serverFilePath;
    Logger::info("Dir html", true);
    if(_constructIndex(serverFilePath, location) == true)
    {
        _httpStatusCode = 200;
        return;
    }
    _autoindexHtml(serverFilePath,location);
}

void ResponseBody::_autoindexHtml(const std::string& serverFilePath, const LocationSettings& location)
{
    if(location.getAutoindexFlag() == true)
    {
        _httpStatusCode = 0;
        Autoindex autoindex(serverFilePath, _httpStatusCode, static_cast<RequestHeader*>(_client.getClientMsg()->getHeader())->getFullClientURL());
        _response = autoindex.getAutoIndexHtml();
        if(autoindex.getStatusCode() != 0 && autoindex.getStatusCode() != 200)
        {
            _renderServerErrorPage(autoindex.getStatusCode());
            return;
        }
        _httpStatusCode = 200;
    }
    else 
    {
        _renderServerErrorPage(404);
    }
}

bool ResponseBody::_constructIndex(const std::string& serverFilePath, const LocationSettings& location)
{
    for(size_t i = 0; i < location.getIndexes().size(); i++)
    {
        std::string fileName = serverFilePath + (location.getIndexes()[i]);
        if(_fileHtml(fileName) == true)
            return true;
    }
    return false;
}


std::string ResponseBody::_convertToServerPath(const LocationSettings& location) const
{
    std::string serverPath(static_cast<RequestHeader*>(_client.getClientMsg()->getHeader())->urlSuffix->getPath());
    size_t startingPos = 0;
    size_t replaceLen = location.getLocationUri().size();
    const std::string& replaceString(location.getRoot() + "/");
    serverPath.replace(startingPos, replaceLen, replaceString);
    return serverPath;
}

void ResponseBody::_generateServerResponse()
{
    if(static_cast<RequestHeader*>(_client.getClientMsg()->getHeader())->getRequestLine().protocolVersion != "HTTP/1.1")
    {
        _renderServerErrorPage(505);
    }
    std::string requstedMethod = static_cast<RequestHeader*>(_client.getClientMsg()->getHeader())->getRequestLine().requestMethod;
    if(requstedMethod == "GET")
    {
        _handlerGetMethod();
    }
    else if(requstedMethod == "POST")
    {
        Logger::error("Not implemeted method yet :"); std::cout << requstedMethod << std::endl;
        _renderServerErrorPage(501);
    }
    else if(requstedMethod == "DELETE")
    {
        Logger::error("Not implemeted method yet :"); std::cout << requstedMethod << std::endl;
        _renderServerErrorPage(501);
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
