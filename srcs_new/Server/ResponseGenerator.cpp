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
#include "LocationSettings.hpp"
#include "../Utils/Autoindex.hpp"
#include <fstream>
#include <sys/stat.h>
// #include <iostream>

// Static function
void ResponseGenerator::generateClientResponse(Client &client)
{
	// WE DON'T GENERATE A RESPONSE TO CLIENT IF:
	// THE RESPONSE MESSAGE IS NOT COMPLETE AND WE ARE NOT RUNNING CGI
	// OR WE ARE RUNNING CGI BUT THE CHILD PROCESS HAS NOT RETURNED YET
	if ((client.getMsg(Client::RESP_MSG)->getState() == COMPLETE && client.getCgiFlag() == false)
		|| (client.getCgiFlag() == true && client.getWaitReturn() == 0))
	{
		return;
	}

	// SELECT THE CORRECT MESSAGE IN CLIENT AND RESET THE ITERATOR TO THE HEADER IN MESSAGE
    Message* message = client.getMsg(Client::RESP_MSG);
    message->resetIterator();

	// WE ARE GENERATING OUT RESPONSE IF
	// WE DID NOT RUN CGI OR THERE WAS AN ERR ALREADY OR THE CLIENT IS PAST THE TIMEOUT
    if (client.getCgiFlag() == false || client.getErrorCode() != 0 || client.checkTimeout() == false)
    {   
        client.setCgiFlag(false);
        ResponseGenerator oneResponse(client);
        // Logger::warning("One response http status code is",oneResponse.getResponseHttpStatus());
        ResponseHeader* header =  ResponseHeader::createRgResponseHeader(oneResponse);
		Logger::info("Response header:\n", header->turnResponseHeaderToString());
		Logger::info("Response body (generated):\n", oneResponse.getResponse());

		// CREATES RESPONSE MESSAGE AND CHUNKS THE BODY IF NECESSARY
        message->stringsToChain(header, oneResponse.getResponse());
    }

    else // THIS MEANS CGI RAN SUCCESSFULLY
    {
        // TODO: correct the Header instance in Message from CGI Response state to final state
        ResponseHeader* header = static_cast<ResponseHeader*>(message->getHeader());
        client.setCgiFlag(false);
		Logger::info("string in response Header:\n", header->turnResponseHeaderToString());
		message->getChain().begin()->setString(header->turnResponseHeaderToString() + "\r\n");
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

static const LocationSettings& getClientRequestedLocation(Client& client) 
{
	const VirtualServer& server = * (client.getVirtualServer());
	const RequestHeader& header = *static_cast<RequestHeader*>(client.getMsg(Client::REQ_MSG)->getHeader());
	const std::string& clientUriPath = header.urlSuffix->getPath();
	const std::string& serverLocationUri = server.getLocationURIfromPath(clientUriPath);
	bool found = false;
	std::vector<LocationSettings>::const_iterator it = server.fetchLocationWithUri(serverLocationUri, found);
	if(found == true)
	{
		const LocationSettings& reponseLocation = *it;
		return reponseLocation;
	}
	Logger::error("THIS SHOULD NEVER HAPPEND, requested location is not found", serverLocationUri);
	return server.getServerLocations()[0];
} 

const NginnxReturn& ResponseGenerator::getRedirect() const
{
	return (_redirect);
}

void ResponseGenerator::_redirectHandler(const LocationSettings& location)
{
	_redirect = location.getNginxReturn();
	_httpStatus = _redirect.getStatus();
	//_response = _generateErrorPage(302);
	_response = _renderServerErrorPage(_httpStatus); //FIXME: check what if user provide you error page that does not exist in config file 
}

static std::string _pathRelativeToExecutable(const LocationSettings& location, const UrlSuffix& suffix)
{
	std::string serverPath = suffix.getPath();
// std::string serverPath(static_cast<RequestHeader*>(_client.getClientMsg()->getHeader())->urlSuffix->getPath());
	size_t startingPos = 0;
	const size_t& replaceLen = location.getLocationUri().size();
	const std::string& replaceString(location.getRoot() + "/");
	serverPath.replace(startingPos, replaceLen, replaceString);
	Logger::info("Relative path ", serverPath);
	return serverPath;
}

void ResponseGenerator::_getHandler(const LocationSettings& location)
{
	Logger::info("Resposible location is", location.getLocationUri());
	_generateHtml(location);
	//g
	// bool result = FileUtils::putFileInString("html/first.html", _response);
	// if(result == true)
	//     _httpStatus = 200;
	// else 
	// {
	//     _response = _renderServerErrorPage(500);
	// }
	//check if redirected 
}

static bool _fileHtml(const std::string& file, std::string& response)
{
	bool success = FileUtils::putFileInString(file, response);
	if(success == true)
		return true;
	return false;
}

static bool _constructIndex(const std::string& file, const LocationSettings& location, std::string& response)
{
	for(size_t i = 0; i < location.getIndexes().size(); i++)
	{
		std::string fileName = file + (location.getIndexes()[i]);
		if(_fileHtml(fileName, response) == true)
			return true;
	}
	return false;
}

static bool _dirHtml(const std::string& file, const LocationSettings& location, std::string& response)
{
	Logger::info("Dir html", true);
	if(_constructIndex(file, location, response) == true)
	{
		return true; //200set
	}
	return false;
	//_autoindexHtml(file,location);
}

void ResponseGenerator::_autoindexHtml(const std::string& relativePath, const LocationSettings& location)
{
	if(location.getAutoindexFlag() == true)
	{
		_httpStatus = 0;
		Logger::warning("getFullClientURL ", static_cast<RequestHeader*>(_client.getMsg(Client::REQ_MSG)->getHeader())->getFullClientURL());
		Autoindex autoindex(relativePath, _httpStatus, static_cast<RequestHeader*>(_client.getMsg(Client::REQ_MSG)->getHeader())->getFullClientURL());
		_response = autoindex.getAutoIndexHtml();
		if(_httpStatus != 0)
		{
			_response = _renderServerErrorPage(_httpStatus);
			return;
		}
		_httpStatus = 200;
	}
	else 
		_response = _renderServerErrorPage(404);
}

void ResponseGenerator::_generateHtml(const LocationSettings& location)
{
	const UrlSuffix& suffix = * (static_cast<RequestHeader*>(_client.getMsg(Client::REQ_MSG)->getHeader())->urlSuffix);
	std::string relativePath = _pathRelativeToExecutable(location, suffix);
	int result = FileUtils::isPathFileOrFolder(relativePath, _httpStatus);
	if(result == -1)
	{
		_response = _renderServerErrorPage(_httpStatus);
		return;
	}
	if(result == 1)
	{
		if (_fileHtml(relativePath, _response) == true)
			_httpStatus = 200;
		else 
			_response = _renderServerErrorPage(404);
		return;
	}
	else
	{
		if(_dirHtml(relativePath, location, _response) == true)
			_httpStatus = 200; 
		else 
			_autoindexHtml(relativePath, location);
	}
	return;
}

// Helper function for _postHandler (POST request)
// it generates a name based in: query_string, or time()
// and appends counter to avoid duplicate overwrite.
static std::string	generateFilename(const std::string& queryString)
{
	if (queryString.empty() == false)
		return (queryString);
	time_t now = time(NULL); // seconds since Epoch
	std::ostringstream filename;
	filename << "file_" << now << ".bin";
	struct stat buffer;
	bool exist = (stat(filename.str().c_str(), &buffer) == 0);
	size_t counter = 1;
	while (exist)
	{
		filename.clear();
		filename.str("");
		filename << "file_" << now << "_" << counter << ".bin";
		exist = (stat(filename.str().c_str(), &buffer) == 0);
		++counter;
	}
	return (filename.str());
}

void		ResponseGenerator::_postHandler(const LocationSettings& location)
{
	std::cout << "POST method executed" << std::endl;
	Message& message = *(_client.getMsg(Client::REQ_MSG));
	const RequestHeader& header = *static_cast<RequestHeader*>(message.getHeader());
	std::string filename = generateFilename(header.urlSuffix->getQueryParameters());
	std::string folderName = location.getUploadFolder();
	filename  = folderName + "/" + filename;
	std::ofstream outputFile(filename.c_str(), std::ios::binary);
	if (!outputFile.is_open())
	{
		std::cerr << "Unable to create POST file!" << std::endl;
		_httpStatus = 500;
		_response = _renderServerErrorPage(_httpStatus);
		return ;
	}
	outputFile << message.getBodyString();
	outputFile.close();
	_httpStatus = 201;
	_response = _renderServerErrorPage(_httpStatus);
}

void		ResponseGenerator::_deleteHandler(const LocationSettings& location)
{
	std::cout << "DELETE method executed" << std::endl;
	const UrlSuffix& suffix = * (static_cast<RequestHeader*>(_client.getMsg(Client::REQ_MSG)->getHeader())->urlSuffix);
	std::string filename = _pathRelativeToExecutable(location, suffix);
	if (remove(filename.c_str()) != 0)
	{
		Logger::error("Unable to DELETE file!", filename);
		_httpStatus = 404;
		_response = _renderServerErrorPage(_httpStatus);
		return ;
	}
	_httpStatus = 204;
	_response = "";
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
		const RequestHeader& header = *static_cast<RequestHeader*>(_client.getMsg(Client::REQ_MSG)->getHeader());
		const std::string& requestMethod = header.getRequestLine().requestMethod;
		const LocationSettings& requestLocation = getClientRequestedLocation(_client);
		
		if (requestLocation.getNginxReturn().getFlag() == true)
		{
			Logger::warning("You are about to be redirected", requestLocation.getNginxReturn().getRedirectPath());
			_redirectHandler(requestLocation);
		}
		else if(requestMethod == "GET")
		{
			Logger::info("Client requeste url is ", header.urlSuffix->getPath());
			_getHandler(requestLocation);
		}
		else if (requestMethod == "POST")
		{
		_postHandler(requestLocation);
		}
		else if(requestMethod == "DELETE")
		{
			_deleteHandler(requestLocation);
		}
		else
		{
			Logger::warning("Method not implemneted", requestMethod);
			_response = _renderServerErrorPage(405);
		}
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
        Logger::info("Server doesn't have page for error code ", errorCode);
        errorHtml = _generateErrorPage(errorCode);
    }
    // Logger::warning("Http status code ", _httpStatus);
	return errorHtml;
}

void ResponseGenerator::_setFileType(fileType type)
{
	_fileType = type;
}
