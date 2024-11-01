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
#include <cstdio>


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

	// WE DO GENERATE OUR RESPONSE IF
	// WE DID NOT RUN CGI OR THERE WAS AN ERR ALREADY OR THE CLIENT IS PAST THE TIMEOUT
	if (client.getCgiFlag() == false || client.getErrorCode() != 0 || client.checkTimeout() == false)
	{
		client.setCgiFlag(false);
		ResponseGenerator oneResponse(client);
		// Logger::warning("One response http status code is",oneResponse.getResponseHttpStatus());
		ResponseHeader* header =  ResponseHeader::createRgResponseHeader(oneResponse);
		// Logger::info("Response header:\n", header->turnResponseHeaderToString());
		// Logger::info("Response body (generated):\n", oneResponse.getResponse());

		if (client.getErrorCode() != 0)
		{
			message->getChain().clear();
			message->getChain().push_back(Node("", HEADER, false));
			message->resetIterator();
		}


		// CREATES RESPONSE MESSAGE AND CHUNKS THE BODY IF NECESSARY
		message->stringsToChain(header, oneResponse.getResponse());
		// START TESTING
		// if (message->getErrorCode() == 502)
		// {
		// 	Logger::error("Printing whole message for client with error code 502", "");
		// 	message->printChain();
		// }
		// END TESTING
	}

	else // THIS MEANS CGI RAN SUCCESSFULLY
	{
		ResponseHeader* header = static_cast<ResponseHeader*>(message->getHeader());
		client.setCgiFlag(false);
		// Logger::info("string in response Header:\n", header->turnResponseHeaderToString());
		message->getChain().begin()->setString(header->turnResponseHeaderToString() + "\r\n");
	}

	client.getMsg(Client::RESP_MSG)->setState(COMPLETE);
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
	_response = _renderLocationErrorPage(location,_httpStatus);
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
	Logger::info("Responsible location is", location.getLocationUri());
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
		std::string fileName = file;
		if(fileName[fileName.size() - 1] != '/')
			fileName += "/";
		fileName += (location.getIndexes()[i]);
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
			_response = _renderLocationErrorPage(location,_httpStatus); 
			return;
		}
		_httpStatus = 200;
	}
	else 
		_response = _renderLocationErrorPage(location, 404);
}

void ResponseGenerator::_generateHtml(const LocationSettings& location)
{
	const UrlSuffix& suffix = * (static_cast<RequestHeader*>(_client.getMsg(Client::REQ_MSG)->getHeader())->urlSuffix);
	std::string relativePath = _pathRelativeToExecutable(location, suffix);
	int result = FileUtils::isPathFileOrFolder(relativePath, _httpStatus);
	if(result == -1)
	{
		_response = _renderLocationErrorPage(location,_httpStatus);
		return;
	}
	if(result == 1)
	{
		if (_fileHtml(relativePath, _response) == true)
			_httpStatus = 200;
		else 
			_response = _renderLocationErrorPage(location,404);
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
static std::string	generateFilename(const std::string& queryString, const std::string& uploadFolder)
{
	const time_t now = time(NULL);
	std::string fileName = "file_" + ParsingUtils::toString(now);
	std::string fileExtension(".bin");
	if(queryString.empty() == false)
	{
		fileExtension = FileUtils::getFileExtension(queryString);
		if(fileExtension == "")
			fileExtension = ".bin";
		fileName = FileUtils::getFileName(queryString);
		fileName = ParsingUtils::uriDecode(true, fileName);
	}
	std::ostringstream file;
	file << fileName << fileExtension;
	std::string fullPath = uploadFolder + "/" + file.str();
	struct stat buffer;
	bool exist = (stat(fullPath.c_str(), &buffer) == 0);
	size_t counter = 1;
	while (exist)
	{
		file.clear();
		file.str("");
		file << fileName << "_" << counter << fileExtension;
		fullPath = uploadFolder + "/" + file.str();
		exist = (stat(fullPath.c_str(), &buffer) == 0);
		++counter;
	}
	return (file.str());
}

void		ResponseGenerator::_postHandler(const LocationSettings& location)
{
	Logger::info("POST method executed", "");
	Message& message = *(_client.getMsg(Client::REQ_MSG));
	const RequestHeader& header = *static_cast<RequestHeader*>(message.getHeader());
	std::string filename = generateFilename(header.urlSuffix->getQueryParameters(), location.getUploadFolder());
	std::string folderName = location.getUploadFolder();
	filename  = folderName + "/" + filename;
	std::ofstream outputFile(filename.c_str(), std::ios::binary);
	if (!outputFile.is_open())
	{
		Logger::error("Unable to create POST file!: ", filename);
		_httpStatus = 500;
		_response = _renderLocationErrorPage(location,_httpStatus);
		return ;
	}
	outputFile << message.getBodyString();
	outputFile.close();
	_httpStatus = 201;
	_response = _renderLocationErrorPage(location,_httpStatus);
}

void		ResponseGenerator::_deleteHandler(const LocationSettings& location)
{
	Logger::info("DELETE method executed", "");
	const UrlSuffix& suffix = * (static_cast<RequestHeader*>(_client.getMsg(Client::REQ_MSG)->getHeader())->urlSuffix);
	std::string filename = _pathRelativeToExecutable(location, suffix);
	if (std::remove(filename.c_str()) != 0)
	{
		Logger::error("Unable to DELETE file!", filename);
		_httpStatus = 404;
		_response = _renderLocationErrorPage(location,_httpStatus);
		return ;
	}
	_httpStatus = 204;
	_response = "";
}

void ResponseGenerator::_responseMenu()
{
	if(_client.getErrorCode() != 0)
	{
		Logger::info("Generating error response: ", _client.getErrorCode());
		_response = _renderServerErrorPage(_client.getErrorCode());
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
			_response = _renderLocationErrorPage(requestLocation,_httpStatus);
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
	const VirtualServer& server = *(_client.getVirtualServer());
    const std::string &errorPagePath = server.getErrorPagePath(errorCode);
	return(_renderErrorPage(errorPagePath, errorCode));
}

std::string ResponseGenerator::_renderLocationErrorPage(const LocationSettings& location, const int& errorCode)
{
	std::string locationErrorHtml;
	const std::string& errorPagePath = location.getErrorPagePath(errorCode);
	return (_renderErrorPage(errorPagePath, errorCode));
}

std::string ResponseGenerator::_renderErrorPage(const std::string& errorPagePath, const int& errorCode)
{
	std::string html;
	if(errorPagePath != "")
    {
        Logger::info("Server/Location have that error page and path to it is ", errorPagePath);
        bool success = FileUtils::putFileInString(errorPagePath, html);
        if(success == true)
            _httpStatus = errorCode;
        else
		{
            html = _generateErrorPage(errorCode);
			_httpStatus = errorCode;
		}
    }
	else
    {
        Logger::info("Server/location doesn't have page for error code ", errorCode);
        html = _generateErrorPage(errorCode);
    }
	return html;
}



void ResponseGenerator::_setFileType(fileType type)
{
	_fileType = type;
}
