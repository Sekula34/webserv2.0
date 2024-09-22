#include "UrlSuffix.hpp"
#include "Data.hpp"
#include "Logger.hpp"
#include <cstddef>
#include "../Parsing/ParsingUtils.hpp"


UrlSuffix::UrlSuffix(const std::string urlSuffix)
:_urlSuffix(urlSuffix)
{
	_initVars();
	_setAllVars();
}

UrlSuffix::UrlSuffix(const UrlSuffix& source) 
:_urlSuffix(source._urlSuffix)
{
	_initVars();
	_setAllVars();
}

UrlSuffix& UrlSuffix::operator=(const UrlSuffix source)
{
	Logger::warning("Give me good reason why you called this on URL suffix", "");
	(void) source;
	return (*this);
}

UrlSuffix::~UrlSuffix()
{

}

const std::string& UrlSuffix::getPath() const
{
	return (_path);
}

const std::string& UrlSuffix::getQueryParameters() const 
{
	return (_queryParameters);
}

const std::string& UrlSuffix::getCgiScriptName() const 
{
	return (_cgiScriptName);
}

const std::string& UrlSuffix::getCgiScriptExtension() const
{
	return (_cgiScriptExtenstion);
}

const std::string& UrlSuffix::getCgiPathInfo() const 
{
	return (_cgiPathInfo);
}

bool UrlSuffix::setCgiScriptName(std::string scriptWithExtension)
{
	//check if script name have valid extension
	size_t dotPos = scriptWithExtension.rfind(".");
	if(dotPos == std::string::npos)
		return false;
	std::string extension = scriptWithExtension.substr(dotPos);
	if(Data::isCgiExtensionValid(extension) == true)
	{
		_cgiScriptName = scriptWithExtension;
		_setCgiScriptExtension(extension);
		return true;
	}
	return  false;
}


void UrlSuffix::setCgiPathInfo(std::string cgiPathInfo)
{
	_cgiPathInfo = cgiPathInfo;
}

void UrlSuffix::_setCgiScriptExtension(std::string extension)
{
	_cgiScriptExtenstion = extension;	
}

void UrlSuffix::_initVars()
{
	_path = "";
	_queryParameters = "";
	_cgiScriptExtenstion = "";
	_cgiScriptName = "";
}

void UrlSuffix::_setAllVars()
{
	_parseAndSetPath();
	_parseAndSetQuerryParameters();
}

void UrlSuffix::_parseAndSetPath()
{
	size_t questionMarkPos = _urlSuffix.find_first_of("?");
	if(questionMarkPos == std::string::npos)
		_path = _urlSuffix;
	_path = _urlSuffix.substr(0, questionMarkPos); // This _path is percent-encoded (%20).
	_path = ParsingUtils::uriDecode(false, _path); // Now this _path is decoded.
}

void UrlSuffix::_parseAndSetQuerryParameters()
{
	std::string rest = ParsingUtils::extractAfterDelim(_urlSuffix, "?");
	_queryParameters = ParsingUtils::extractUntilDelim(rest, "#");
	if(_queryParameters == "")
		_queryParameters = rest;
	else
		_queryParameters.erase(_queryParameters.end() - 1);
}
