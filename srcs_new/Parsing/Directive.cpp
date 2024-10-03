#include "Directive.hpp"
#include "../Server/DefaultSettings.hpp"
#include "ParsingUtils.hpp"
#include "Token.hpp"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "../Utils/Logger.hpp"
#include "../Utils/Data.hpp"
#include "../Utils/FileUtils.hpp"
#include "Configuration.hpp"

const std::string Directive::_validHttpDirectives[] = {"client_max_body_size", "autoindex", "index", "error_page", "root", "upload_folder"};

const std::string Directive::_validServerDirectives[] ={"listen", "host", "server_name",
"error_page", "client_max_body_size", "return", "autoindex", "index", "root", "upload_folder"};

const std::string Directive:: _validLocationDirectives[] = {"error_page", "client_max_body_size", "return",
"autoindex", "index", "limit_except", "root", "extension", "upload_folder"};

//directive that can be present only once
const std::string Directive::_uniqueDirectives[] = {"client_max_body_size", "autoindex", "root", "limit_except", "extension"}; //TODO: add upload_folder here but be carefull with checking Uploads cuzz this is default

const std::string Directive:: _validHttpMethods[] = {"GET", "POST", "DELETE"};

std::vector<Directive> Directive::getAllServerDirectives(const std::vector<Token>& allServerTokens)
{
	std::vector<Directive> serverDirectives;
	for(size_t i = 0; i < allServerTokens.size(); i++)
	{
		if(allServerTokens[i].getTokenType() == Token::DIRECTIVE)
		{
			Directive dire(allServerTokens[i]);
			serverDirectives.push_back(dire);
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

	firstIt = std::find_if(directives.begin(), directives.end(), functor);
	if(firstIt == directives.end())
		return false;
	secondIt = std::find_if(++firstIt, directives.end(), functor);
	if(secondIt == directives.end())
		return false;
	duplicateDir = &(*secondIt);
	return true;
}

size_t Directive::getDirectivePathSize(void) const 
{
	return (_dirPath.size());
}


void Directive::printAllDirectives(const std::vector<Directive> &allDirectives)
{
	for(size_t i = 0; i < allDirectives.size(); i++)
	{
		std::cout << allDirectives[i] << std::endl;
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
	else if(_directiveName == "upload_folder")
		_applyUploadFolder(settings);
	else 
	{
		Logger::warning("Applying directive: ", _directiveName);;
		Logger::warning("is not implemeted yet", "");
	}
}
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

bool Directive::_isNameValid(const std::string& name, const std::string validList[], Token::ContextType context) const
{
	size_t size = 0;
	switch (context)
	{
		case Token::HTTP :
		{
			size = sizeof(_validHttpDirectives)/sizeof(std::string);
			break;
		}
		case Token::SERVER :
		{
			size = sizeof(_validServerDirectives)/sizeof(std::string);
			break;
		}
		case Token::LOCATION:
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
		std::ostringstream oss ("Invalid directive [");
		oss << token.getTokenInfo() << "] " << "in line " << token.getTokenLineNumber();
		Logger::error(oss.str(), "");
		throw InvalidDirectiveException();
	}
	name = name.substr(0, spacePos);
	std::vector<Token> path (token.getTokenPath());
	if(path.empty() == true)
	{
		std::cout << "Directive is not inside any context " << _directiveName << " in line " << token.getTokenLineNumber() << std::endl;
		throw InvalidDirectiveException();
	}
	Token parent = path.back();
	bool validName = false;
	switch (parent.getCurrentTokenContextType())
	{
		case Token::HTTP :
		{
			validName = _isNameValid(name, _validHttpDirectives, Token::HTTP);
			break;
		};
		case Token::LOCATION:
		{
			validName = _isNameValid(name, _validLocationDirectives, Token::LOCATION);
			break;
		}
		case Token::SERVER:
		{
			validName = _isNameValid(name, _validServerDirectives, Token::SERVER);
			break;
		}
		default:
		{
			std::ostringstream oss;
			oss<<"Default case Directive " << name <<" in line " << token.getTokenLineNumber() << "is not inside any context";
			Logger::error(oss.str(), ""); 
			throw InvalidDirectiveException();
		}
	}
	if(validName == false)
	{
		std::ostringstream oss;
		oss<<"Unknown directive \"" << name << "\" inside Context type : " << parent.getCurrentTokenContextType() << " in line " << token.getTokenLineNumber();
		Logger::error(oss.str(), "");
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
	size_t posSemi = value.find(';');
	value = value.substr(0,posSemi);
	return value;
}



void Directive::_apllyRoot(DefaultSettings& settings)
{
	if(_directiveValue.empty() == true)
	{
		std::ostringstream oss;
		oss<< "Root value is empty in line " << _dirLineNumber;
		Logger::error(oss.str(), "");
		throw InvalidDirectiveException();
	}
	_directiveValue = ROOT_FOLDER + _directiveValue; // MR_NOTE: Here we append the root folder (WWW)
	// MR_NOTE: Here we check for path traversal sequences (../ ~/) in the name.
	if (FileUtils::isValidName(_directiveValue) == false)
	{
		Logger::warning("Root name contains path traversal sequences: ", _directiveValue);
		Logger::warning("Line :", _dirLineNumber);
		throw InvalidDirectiveException(); // Maybe rem,ove this or not_
	}
	settings.setRoot(_directiveValue);
}

void Directive::_applyIndex(DefaultSettings& settings)
{
	std::vector<std::string> indexes = ParsingUtils::splitString(_directiveValue, ' ');
	if(indexes.size() < 1 || indexes[0].empty() == true)
	{
		std::ostringstream oss;
		oss << "Index value is empty  in line " << _dirLineNumber;
		Logger::error(oss.str(), "");
		throw InvalidDirectiveException();
	}
	// MR_NOTE: Here we check for path traversal sequences (../ ~/) in the name.
	std::vector<std::string>::const_iterator it = indexes.begin();
	for (; it != indexes.end(); ++it)
	{
		if (FileUtils::isValidName(*it) == false)
		{
			Logger::warning("Index name contains path traversal sequences: ", *it);
			Logger::warning("Line :", _dirLineNumber);
			throw InvalidDirectiveException(); // Maybe rem,ove this or not_
		}
	}
	settings.setIndexes(indexes);
}

void Directive::_applyReturn(DefaultSettings& settings)
{
	std::vector<std::string> values = ParsingUtils::splitString(_directiveValue, ' ');
	if(values.size() != 2)
	{
		std::ostringstream oss;
		oss<< "Invalid return directive value " << _directiveValue << " in line " << _dirLineNumber << " for return value need to be status code and url seperated by space";
		Logger::error(oss.str(), "");
		throw InvalidDirectiveException();
	}
	int status = _stringToInt(values[0]);
	if(status < 100 || status > 599)
	{
		std::ostringstream oss;
		oss << "Http return code " << _directiveValue << " in line " << _dirLineNumber << " is out of range";
		Logger::error(oss.str(), "");
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
		std::ostringstream oss;
		oss << "Invalid auto index directive value " << _directiveValue << " in line " << _dirLineNumber << " value can be either on or off for autoindex";
		Logger::error(oss.str(), "");
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
		std::ostringstream oss;
		oss << "Invalid error page directive value " << _directiveValue << " in line " << _dirLineNumber;
		Logger::error(oss.str(), "");
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
		Logger::error(oss.str(), "");
		throw InvalidDirectiveException();
	}
	erorrPagePath = ROOT_FOLDER + erorrPagePath; // MR_NOTE: Here we append the root folder (WWW)
	// MR_NOTE: Here we check for path traversal sequences (../ ~/) in the name.
	if (FileUtils::isValidName(erorrPagePath) == false)
	{
		Logger::warning("Root name contains path traversal sequences: ", erorrPagePath);
		Logger::warning("Line :", _dirLineNumber);
		throw InvalidDirectiveException(); // Maybe rem,ove this or not_
	}
	settings.setErrorPage(codeNumber, erorrPagePath);
}



void Directive::_applyServerName(DefaultSettings& settings)
{
	if(settings.getFirstNameApply() == true)
	{
		settings.setNameFlagFalse();
		settings.removeDefaultName();
	}
	if(settings.isContainingName(_directiveValue) == true)
	{
		std::ostringstream oss;
		oss << "[" << getDirectiveName() << "]" << " conflicting server name [" << _directiveValue << "] " << FileUtils::getConfigFilePath() << ":";
		oss << getDirectiveLineNum() << " ignored";
		Logger::warning(oss.str(), true);
		return;
	}
	settings.addServerName(_directiveValue);
}

void Directive::_applyListenFirstTime(DefaultSettings& settings)
{
	if(settings.getFirstListenApplyFlag() == false)
		return;
	settings.setListenFlagFalse();
	settings.removeDefaultListen();
}

//set settings listen port
void Directive::_applyListen(DefaultSettings& settings)
{
	_applyListenFirstTime(settings);
	int portNumber = _stringToInt(_directiveValue);
	if(portNumber < 0 || portNumber > 65535)
	{
		std::cerr << "Invalid port Number in line " << _dirLineNumber << std::endl;
		throw InvalidDirectiveException();
	}
	if(settings.isListeningToPort(portNumber) == true)
	{
		std::ostringstream oss;
		oss << "[" << getDirectiveName() << "]" << " is duplicate in " << FileUtils::getConfigFilePath() << ":";
		oss << getDirectiveLineNum();
		Logger::error(oss.str(), true);
		throw Configuration::InvalidConfigFileException();
	}
	settings.addListenPort(portNumber);
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
			std::ostringstream oss;
			oss << "Unknown http method [" << methodName << "] in line " << _dirLineNumber;
			Logger::error(oss.str(), "");
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
		if(clientMaxBodySize > DEFAULT_MAX_BODY_SIZE)
		{
			Logger::warning("Trying to set client max body size bigger than we support. Config value:",clientMaxBodySize);
			Logger::warning("Too big client max body size in " + FileUtils::getConfigFilePath() + ":", _dirLineNumber);
			Logger::info("It is lowered to MACRO MAX which is :", DEFAULT_MAX_BODY_SIZE);
			clientMaxBodySize = DEFAULT_MAX_BODY_SIZE;
		}
	}
	catch(ParsingUtils::InvalidConversion &e)
	{
		std::ostringstream oss;
		oss << "Failed conversion from string [" << _directiveValue << "] to size_t in line " << _dirLineNumber;
		Logger::error(oss.str(), "");
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
			Logger::error("There is no interpreter for :", extensions[i]);
			Logger::error("In line", _dirLineNumber);
			throw InvalidDirectiveException();
		}
	}
	settings.setCgiExtensions(extensions);
}

void Directive::_applyUploadFolder(DefaultSettings& settings)
{
	_directiveValue = ROOT_FOLDER + _directiveValue; // MR_NOTE: Here we append the root folder (WWW)
	// MR_NOTE: Here we check for path traversal sequences (../ ~/) in the name.
	if (FileUtils::isValidName(_directiveValue) == false)
	{
		Logger::warning("Folder name contains path traversal sequences: ", _directiveValue);
		Logger::warning("Line :", _dirLineNumber);
		throw InvalidDirectiveException(); // Maybe rem,ove this or not_
	}
	int errorCode = 0;
	bool exist =  FileUtils::isDirectoryValid(_directiveValue, errorCode);
	if(exist == false)
	{
		Logger::warning("Folder does not exist ", _directiveValue);
		Logger::warning("Line :", _dirLineNumber);
		throw InvalidDirectiveException(); // Maybe rem,ove this or not_
	}
	settings.setUploadFolder(_directiveValue);
}


const char* Directive::InvalidDirectiveException::what() const throw()
{
	return ("Exception: Invalid directive");
}

std::ostream& operator<<(std::ostream& os, const Directive& directive)
{
	std::string fancyTitle = Logger::createFancyTitle("DIRECTIVE INFO");
	os << fancyTitle << std::endl;
	os << "Directive name is :[" << directive._directiveName <<"]" <<std::endl;
	os << "Directive value is :[" << directive._directiveValue <<"]" <<std::endl;
	os << Logger::getCharSequence(fancyTitle.size()) << std::endl;
	return os;
}
