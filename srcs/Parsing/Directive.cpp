#include "Directive.hpp"
#include "DefaultSettings.hpp"
#include "ParsingUtils.hpp"
#include "Token.hpp"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "../Utils/Logger.hpp"
#include "../Utils/Data.hpp"
#include "../Utils/FileUtils.hpp"

const std::string Directive::_validHttpDirectives[] = {"client_max_body_size", "autoindex", "index", "error_page", "root"};

const std::string Directive::_validServerDirectives[] ={"listen", "host", "server_name",
"error_page", "client_max_body_size", "return", "autoindex", "index", "root"};

const std::string Directive:: _validLocationDirectives[] = {"error_page", "client_max_body_size", "return",
"autoindex", "index", "limit_except", "root", "extension"};

//directive that can be present only once
const std::string Directive::_uniqueDirectives[] = {"client_max_body_size", "autoindex", "root", "limit_except", "extension"};

const std::string Directive:: _validHttpMethods[] = {"GET", "POST", "DELETE"};

Directive::Directive()
{

}
Directive::Directive(std::string dirName, std::string dirValue)
{
	_directiveName = dirName;
	_directiveValue = dirValue;
}

Directive::Directive(const Token& token)
{
	
	_directiveName = _getNameFromToken(token);
	_dirLineNumber = token.getTokenLineNumber();
	_directiveValue = _getValueFromToken(token); 
	_dirPath = token.getTokenPath();
}

Directive& Directive::operator=(const Directive& source)
{
	_dirPath = source._dirPath;
	_directiveName = source._directiveName;
	_directiveValue = source._directiveValue;
	_dirLineNumber = source._dirLineNumber;
	return (*this);
}

Directive::Directive(const Directive& source)
{
	(*this) = source;
}

Directive::~Directive()
{

}




bool Directive::_isNameValid(const std::string& name, const std::string validList[], ContextType context) const
{
	size_t size = 0;
	switch (context)
	{
		case HTTP :
		{
			size = sizeof(_validHttpDirectives)/sizeof(std::string);
			break;
		}
		case SERVER :
		{
			size = sizeof(_validServerDirectives)/sizeof(std::string);
			break;
		}
		case LOCATION:
		{
			size = sizeof(_validLocationDirectives)/sizeof(std::string);
			break;
		}
		default:
		{
			std::cerr<<"_isNameValid function should not be printed ever " << std::endl;
			throw std::runtime_error("Unexpected to be here");
		}
	}
	for(size_t i = 0; i < size; i++)
	{
		//std::cout << "Comparing " << name << " to" << validList[i] << std::endl;
		if(name == validList[i])
			return true;
	}
	return false;
}

//convert std::string to int value
//throw runtime error if isstringstream failed
int Directive::_stringToInt(std::string stringValue) const 
{
	std::istringstream iss(stringValue);
	int value;
	if(!(iss >> value))
	{
		std::cerr <<"In line " << _dirLineNumber << " \"" << stringValue<< "\"" <<std::endl;
		throw std::runtime_error("isstringstream to int failed");
	}
	return (value);
}

//extract token name from token info
//throw InvalidDirectiveException if token is Unkown or directive is not inside expexcted context
std::string Directive::_getNameFromToken(const Token& token) const
{
	std::string name;
	name = token.getTokenInfo();
	size_t spacePos = name.find(' ');
	if(name.empty() || spacePos == std::string::npos)
	{
		std::cerr<<yellow << "Invalid directive [" << token.getTokenInfo() <<"]" <<
		"in line " << token.getTokenLineNumber() << resetText <<std::endl;
		throw InvalidDirectiveException();
	}
	name = name.substr(0, spacePos);
	std::vector<Token> path (token.getTokenPath());
	//std::cout << "path size is " << path.size() << std::endl;
	if(path.empty() == true)
	{
		std::cout << "Directive is not inside any context " << _directiveName << " in line " << token.getTokenLineNumber() << std::endl;
		throw InvalidDirectiveException();
	}
	Token parent = path.back();
	bool validName = false;
	switch (parent.getCurrentTokenContextType())
	{
		case HTTP :
		{
			validName = _isNameValid(name, _validHttpDirectives, HTTP);
			break;
		};
		case LOCATION:
		{
			validName = _isNameValid(name, _validLocationDirectives, LOCATION);
			break;
		}
		case SERVER:
		{
			validName = _isNameValid(name, _validServerDirectives, SERVER);
			break;
		}
		default:
		{
			std::cerr<<yellow<<"Default case Directive " << name <<" in line " << token.getTokenLineNumber() << "is not inside any context" << resetText<< std::endl; 
			throw InvalidDirectiveException();
		}
	}
	if(validName == false)
	{
		std::cerr<<yellow<<"Unknown directive \"" << name << "\" inside Context type : " << parent.getCurrentTokenContextType() << " in line " << token.getTokenLineNumber()<< resetText << std::endl;
		throw InvalidDirectiveException();
	}
	return name;
}


std::string Directive:: _getValueFromToken(const Token& token) const
{
	std::string value = token.getTokenInfo();
	size_t namePos = value.find(_directiveName) + _directiveName.size();
	value = value.substr(namePos);
	for(std::string::iterator i = value.begin(); i < value.end() ; i++)
	{
		if(*i == ' ' || *i == '\t')
		{
			value.erase(i);
			i--;
		}
		else
			break;
	}
	//std::cout <<"Value is :"<<value<<std::endl;
	size_t posSemi = value.find(';');
	value = value.substr(0,posSemi);
	return value;
}


void Directive::printDirectiveInfor(void) const
{
	std::cout << "---------------Directive info--------------" << std::endl;
	std::cout << "Directive name is [" << _directiveName <<"]" <<std::endl;
	std::cout << "Directive value is :[" << _directiveValue <<"]" <<std::endl;
	//std::cout <<"Directive path is : " << std::endl;
	//Token::printAllTokensInfo(_dirPath);
	std::cout <<"_____________________________________________" << std::endl;
}

void Directive::printAllDirectives(const std::vector<Directive> &allDirectives)
{
	for(size_t i = 0; i < allDirectives.size(); i++)
	{
		allDirectives[i].printDirectiveInfor();
	}
}


const std::string& Directive::getDirectiveName(void) const 
{
	return _directiveName;
}

const size_t& Directive::getDirectiveLineNum(void) const 
{
	return _dirLineNumber;
}
void Directive::apply(DefaultSettings& settings)
{
	if(_directiveName == "listen")
		_applyListen(settings);
	else if(_directiveName =="error_page")
		_applyErrorPage(settings);
	else if(_directiveName == "limit_except")
		_applyLimitExcept(settings);
	else if(_directiveName == "client_max_body_size")
		_applyClientMaxBodySize(settings);
	else if(_directiveName == "autoindex")
		_apllyAutoIndex(settings);
	else if(_directiveName == "return")
		_applyReturn(settings);
	else if(_directiveName == "index")
		_applyIndex(settings);
	else if(_directiveName == "root")
		_apllyRoot(settings);
	else if (_directiveName == "server_name")
		_applyServerName(settings);
	else if(_directiveName == "extension")
		_applyCgiExtension(settings);
	else 
	{
		Logger::warning("Applying directive: "); std::cout << _directiveName; Logger::warning("is not implemeted yet", true);
	}
}


void Directive::_applyServerName(DefaultSettings& settings)
{
	//std::cout << "Server name applying" << std::endl;
	//std::cout <<"directive value is " <<_directiveValue << std::endl;
	settings.setServerName(_directiveValue);
}

void Directive::_apllyRoot(DefaultSettings& settings)
{
	if(_directiveValue.empty() == true)
	{
		std::cerr<< yellow << "Root value is empty in line " << _dirLineNumber <<resetText <<std::endl;
		throw InvalidDirectiveException();
	}
	settings.setRoot(_directiveValue);

}

void Directive::_applyIndex(DefaultSettings& settings)
{
	std::vector<std::string> indexes = ParsingUtils::splitString(_directiveValue, ' ');
	if(indexes.size() < 1 || indexes[0].empty() == true)
	{
		std::cerr<< yellow << "Index value is empty  in line " << _dirLineNumber <<resetText <<std::endl;
		throw InvalidDirectiveException();
	}
	settings.setIndexes(indexes);
}

void Directive::_applyReturn(DefaultSettings& settings)
{
	std::vector<std::string> values = ParsingUtils::splitString(_directiveValue, ' ');
	if(values.size() != 2)
	{
		std::cerr<< yellow << "Invalid return directive value " << _directiveValue << " in line " << _dirLineNumber << " for return value need to be status code and url seperated by space"<<resetText <<std::endl;
		throw InvalidDirectiveException();
	}
	int status = _stringToInt(values[0]);
	if(status < 100 || status > 599)
	{
		std::cerr<< yellow << "Http return code " << _directiveValue << " in line " << _dirLineNumber << " is out of range"<<resetText <<std::endl;
		throw InvalidDirectiveException();
	}
	std::string url = values[1];
	settings.setNginxReturn(status, url);
}

void Directive::_apllyAutoIndex(DefaultSettings& settings)
{
	bool state = false;
	if(_directiveValue == "on")
		state = true;
	else if(_directiveValue == "off")
		state = false;
	else 
	{
		std::cerr<< yellow << "Invalid auto index directive value " << _directiveValue << " in line " << _dirLineNumber << " value can be either on or off for autoindex"<<resetText <<std::endl;
		throw InvalidDirectiveException();
	}
	settings.setAutoIndex(state);
}

//add error page to settings with int key and std::string path to error page 
void Directive::_applyErrorPage(DefaultSettings& settings)
{
	size_t posOfSpace = _directiveValue.find(' ');
	if(posOfSpace == std::string::npos)
	{
		std::cerr<< yellow << "Invalid error page directive value " << _directiveValue << " in line " << _dirLineNumber <<resetText <<std::endl;
		throw InvalidDirectiveException();
	}
	std::string codeString = _directiveValue.substr(0, posOfSpace);
	std::string erorrPagePath = _directiveValue.substr(posOfSpace + 1);
	int codeNumber = _stringToInt(codeString);
	if(codeNumber < 300 || codeNumber > 599)
	{
		std::ostringstream oss;
		oss << "Value [" << codeNumber <<"] must be beetween 300 and 599 in " << FileUtils::getConfigFilePath() << ":";
		oss << _dirLineNumber << "' for direcetive error_page";
		Logger::error(oss.str(), true);
		std::cerr << yellow << "Code number : "<<codeNumber <<" of error page directive is out of range. Line " << _dirLineNumber << resetText << std::endl;
		throw InvalidDirectiveException();
	}
	settings.setErrorPage(codeNumber, erorrPagePath);
}

//set settings listen port
void Directive::_applyListen(DefaultSettings& settings)
{
	int portNumber = _stringToInt(_directiveValue);
	if(portNumber < 0 || portNumber > 65535)
	{
		std::cerr << "Invalid port Number in line " << _dirLineNumber << std::endl;
		throw InvalidDirectiveException();
	}
	settings.setListenPort(portNumber);
}

//set all accepted http methods to false 
//then splits directive value by spaces and apply all method to true
//checking if it is actually known
void Directive::_applyLimitExcept(DefaultSettings& settings)
{
	settings.setAllAcceptedMethodsToFalse();
	std::vector<std::string> methodsString = ParsingUtils::splitString(_directiveValue, ' ');
	for(size_t i = 0; i < methodsString.size() ; i++)
	{
		std::string methodName = methodsString[i];
		size_t numberOfValids = sizeof(_validHttpMethods)/sizeof(std::string);
		if(ParsingUtils::isStringValid(methodName,_validHttpMethods, numberOfValids))
			settings.setAcceptedMethodToTrue(methodName);
		else
		{
			std::cerr<<yellow << "Unknown http method [" << methodName << "] in line " << _dirLineNumber << resetText << std::endl;
			throw InvalidDirectiveException();
		}
	}
}

void Directive::_applyClientMaxBodySize(DefaultSettings& settings)
{
	size_t clientMaxBodySize;
	try
	{
		clientMaxBodySize = ParsingUtils::stringToSizeT(_directiveValue);
	}
	catch(ParsingUtils::InvalidConversion &e)
	{
		std::cerr<<yellow << "Failed conversion from string [" << _directiveValue << "] to size_t in line " << _dirLineNumber << resetText << std::endl;
		throw InvalidDirectiveException();
	}
	settings.setClientMaxBodySize(clientMaxBodySize);
}

void Directive::_applyCgiExtension(DefaultSettings& settings)
{
	std::vector<std::string> extensions = ParsingUtils::splitString(_directiveValue, ' ');
	for(size_t i = 0; i < extensions.size(); i++)
	{
		if(Data::isCgiExtensionValid(extensions[i]) == false)
		{
			Logger::error("There is no interpreter for :"); std::cout << extensions[i] << " ";
			std::cout << "In line " << _dirLineNumber << std::endl;
			throw InvalidDirectiveException();
		}
	}
	settings.setCgiExtensions(extensions);
}

std::vector<Directive> Directive::getAllServerDirectives(const std::vector<Token>& allServerTokens)
{
	std::vector<Directive> serverDirectives;	
		//Token::printAllTokensInfo(serverToken);
	for(size_t i = 0; i < allServerTokens.size(); i++)
	{
		//allServerTokens[i].printTokenInfo();
		if(allServerTokens[i].getTokenType() == DIRECTIVE)
		{
			//allServerTokens[i].printTokenInfo();
			Directive dire(allServerTokens[i]);
			serverDirectives.push_back(dire);
			//dire.apply(server);
			//server.printServerSettings();
			////settings.printAllSettings();
			//dire.printDirectiveInfor();
		}
	}
	return serverDirectives;
}
//apply allDirectives on settings
void Directive::applyAllDirectives(std::vector<Directive>& allDirectives, DefaultSettings& settings)
{
	for(size_t i = 0; i < allDirectives.size(); i ++)
	{
		allDirectives[i].apply(settings);
	}
}


bool Directive::isDuplicateDirectivePresent(const std::vector<Directive> &directives, const Directive* &duplicateDir)
{
	size_t uniqueSize = sizeof(_uniqueDirectives) / sizeof(std::string);
	duplicateDir = NULL;
	for(size_t i = 0; i < uniqueSize; i++)
	{
		if(isDuplicateDirectiveNamePresent(directives, duplicateDir, _uniqueDirectives[i]) == true)
			return true;
	}
	return false;
}

bool Directive::isDuplicateDirectiveNamePresent(const std::vector<Directive> &directives, const Directive *&duplicateDir, const std::string &nameToCheck)
{
	std::vector<Directive>::const_iterator firstIt;
	std::vector<Directive>::const_iterator secondIt;
	FindbyDirectiveName functor(nameToCheck);
	duplicateDir = NULL;

	firstIt = std::find_if(directives.cbegin(), directives.cend(), functor);
	if(firstIt == directives.cend())
		return false;
	secondIt = std::find_if(std::next(firstIt), directives.cend(), functor);
	if(secondIt == directives.cend())
		return false;
	duplicateDir = &(*secondIt);
	return true;
}

size_t Directive::getDirectivePathSize(void) const 
{
	return (_dirPath.size());
}

const char* Directive::InvalidDirectiveException::what() const throw()
{
	return ("Exception: Invalid directive");
}
