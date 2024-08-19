#include "UrlSuffix.hpp"
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
	Logger::warning("Give me good reason why you called this on URL suffix");
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


void UrlSuffix::_initVars()
{
	_path = "";
	_queryParameters = "";
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
	_path = _urlSuffix.substr(0, questionMarkPos);
	
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
