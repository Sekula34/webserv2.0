#include "UrlSuffix.hpp"
#include "Logger.hpp"


UrlSuffix::UrlSuffix(const std::string urlSuffix)
:_urlSuffix(urlSuffix)
{
	_initVars();
	_parseAndSetPath();
}

UrlSuffix::UrlSuffix(const UrlSuffix& source) 
:_urlSuffix(source._urlSuffix)
{
	//parse stuff from suffix	
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


void UrlSuffix::_initVars()
{
	_path = "";
	_queryParameters = "";
	_fragment = "";
}

void UrlSuffix::_parseAndSetPath()
{
	size_t questionMarkPos = _urlSuffix.find_first_of("?#");
	if(questionMarkPos == std::string::npos)
		_path = _urlSuffix;
	_path = _urlSuffix.substr(0, questionMarkPos);
	
}
