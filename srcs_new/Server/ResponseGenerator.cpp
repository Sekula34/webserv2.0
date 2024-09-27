#include "ResponseGenerator.hpp"
#include <sstream>
#include <string>
#include "../Parsing/ParsingUtils.hpp"
#include "../Utils/HttpStatusCode.hpp"
#include "../Server/VirtualServer.hpp"
#include "../Utils/Logger.hpp"
#include "../Utils/FileUtils.hpp"
#include "../Message/Message.hpp"

void ResponseGenerator::generateClientResponse(Client &client)
{
	ResponseGenerator oneResponse(client);
    client.setErrorCode(0);
    Message responseMsg(false, client.getErrorCode());
    Logger::warning("Message object is generated", "");
    std::string cRes = oneResponse.getResponse().c_str();
    char * nonConst = const_cast<char*>(cRes.c_str()); 
    responseMsg.bufferToNodes(nonConst, oneResponse.getResponse().size());
    std::cout << "[" <<responseMsg.getBodyString() << "]" << std::endl;
	//Response message 
	//generate message
	//store message in client
}

const std::string& ResponseGenerator::getResponse() const
{
	return _response;
}

const int& ResponseGenerator::getResponseHttpStatus() const 
{
	return _httpStatus;
}

ResponseGenerator::ResponseGenerator(Client& client)
:_client(client)
{
	_fileType = FileUtils::HTML;
	_responseMenu();
}
ResponseGenerator::~ResponseGenerator()
{

}

void ResponseGenerator::_responseMenu()
{
	if(_client.getErrorCode() != 0)
	{
        Logger::info("Generating error response", _client.getErrorCode());
		_response = _renderServerErrorPage(_client.getErrorCode());
		//Generate Error
	}
	else
	{
        Logger::warning("Here should be implemented normal or cgi response", "");
		//if Normal 
			//generate normal 
		//else 
			//store Cgi?
	}
}

std::string ResponseGenerator::_generateErrorPage(const int httpErrorCode)
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
	_setFileType(FileUtils::HTML);
    _httpStatus = httpErrorCode;
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
std::string ResponseGenerator::_renderServerErrorPage(int errorCode)
{
	std::string errorHtml;
	const VirtualServer& server = *(_client.getVirtualServer());
    std::string errorPagePath = "";
    errorPagePath = server.getErrorPagePath(errorCode); //TODO: check extenstion of page path and set it accordingly .css CSS, .html HTML
    if(errorPagePath != "")
    {
        Logger::info("Server have that page and path to it is ", errorPagePath);
        bool success = FileUtils::putFileInString(errorPagePath, errorHtml);
        if(success == true)
            _httpStatus = errorCode;
        else
            errorHtml = _generateErrorPage(500);
    }
    else 
    {
        Logger::info("Server dont have page for error code ", errorCode);
        errorHtml = _generateErrorPage(errorCode);
        Logger::info("Response body is generated and it is", errorHtml);
    }
	return errorHtml;
}

void ResponseGenerator::_setFileType(fileType type)
{
	_fileType = type;
}
