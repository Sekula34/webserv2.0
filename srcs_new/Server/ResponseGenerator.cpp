#include "ResponseGenerator.hpp"
#include <sstream>
#include <string>
#include "../Parsing/ParsingUtils.hpp"
#include "../Utils/HttpStatusCode.hpp"
#include "../Server/VirtualServer.hpp"
#include "../Utils/Logger.hpp"
#include "../Utils/FileUtils.hpp"
#include "../Message/ResponseHeader.hpp"
#include "../Utils/Logger.hpp"
#include "../Message/Message.hpp"
#include "../Message/Node.hpp"
#include "../Message/RequestHeader.hpp"
#include "../Utils/Logger.hpp"

void ResponseGenerator::generateClientResponse(Client &client)
{
    if ((client.getMsg(Client::RESP_MSG)->getState() == COMPLETE && client.getCgiFlag() == false)
		|| (client.getMsg(Client::RESP_MSG)->getState() == INCOMPLETE && client.getCgiFlag() == true))
	{
        return;
	}
    Message* message = client.getMsg(Client::RESP_MSG);
    message->resetIterator();
    if (client.getCgiFlag() == false || client.getErrorCode() != 0 || client.checkTimeout() == false)
    // if (client.getCgiFlag() == false || client.getErrorCode() != 0)
    {   
        // if (client.getCgiFlag() == true)
        // {
        //     client.setCgiFlag(false);
        //     delete client.getMsg(Client::RESP_MSG);
        //     client.getMsg(Client::RESP_MSG); // remember this is a dirty getter and allocs a new instance of Messaage Class
        // }
        ResponseGenerator oneResponse(client);
        //std::cout << oneResponse.getResponse() << std::endl;
        Logger::warning("One response http status code is",oneResponse.getResponseHttpStatus());
        // std::cout << oneResponse.getResponseHttpStatus() << std::endl;
        ResponseHeader* header =  ResponseHeader::createRgResponseHeader(oneResponse);
        header->setOneHeaderField("Connection", "close");
        header->setOneHeaderField("Content-Type", "text/html; charset=utf-8");
        //    header->setOneHeaderField("Content-Length", ParsingUtils::toString(oneResponse.getResponse().size()));
        std::cout << header->turnResponseHeaderToString() << std::endl;
        std::cout << oneResponse.getResponse() << std::endl;
        message->stringsToChain(header, oneResponse.getResponse());
    }
    else // THIS MEANS CGI RAN SUCCESSFULLY
    {
        ResponseHeader* header = static_cast<ResponseHeader*>(message->getHeader());
        client.setCgiFlag(false);
		Logger::warning("string in response Header", header->turnResponseHeaderToString());
		message->getChain().begin()->setString(header->turnResponseHeaderToString() + "\r\n");
        // correct the Header instance in Message from CGI Response state to final state
        // call turnResponseHEaderToString at put it into Header node
  
    }
    client.getMsg(Client::RESP_MSG)->setState(COMPLETE);

   //if(client.g)

    // client.setErrorCode(0);
    // Message responseMsg(false, client.getErrorCode());
    // Logger::warning("Message object is generated", "");
    // std::string cRes = oneResponse.getResponse().c_str();
    // char * nonConst = const_cast<char*>(cRes.c_str()); 
    // responseMsg.bufferToNodes(nonConst, oneResponse.getResponse().size());
    //std::cout << "[" <<responseMsg.getBodyString() << "]" << std::endl;
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
:_client(client), _httpStatus(client.getErrorCode())
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
        RequestHeader& header = *static_cast<RequestHeader*>(_client.getMsg(Client::REQ_MSG)->getHeader());
        std::string requestMethod = header.getRequestLine().requestMethod;
        

        
        if(requestMethod == "GET")
        {
            //getHandler
        }
        else if (requestMethod == "POST")
        {
            //postHandler
        }
        else if(requestMethod == "DELETE")
        {
            //deleteMethod
        }
        else
        {
            Logger::warning("Method not implemneted", requestMethod);
            _response = _renderServerErrorPage(405);
        }
    
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
    Logger::warning("Http status code ", _httpStatus);
	return errorHtml;
}

void ResponseGenerator::_setFileType(fileType type)
{
	_fileType = type;
}
