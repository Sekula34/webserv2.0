#include "Token.hpp"
#include <cstddef>
#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>
#include "../Utils/Logger.hpp"

const std::string Token::_validContexts[] = {"http", "server", "location"};
//Create Token out of info and set default value of type to be UNKOWN, and lineNumber to 0
//check everyContext in AllTokens
//throw InvalidTokenException if there is Unkown context
//set _contextType while checking
void Token::checkAllTokensContext(std::vector<Token>& allTokens)
{
	for(size_t i = 0; i < allTokens.size(); i++)
	{
		Token& oneToken(allTokens[i]);
		oneToken._checkOneToken();
	}
}

//return vector of Tokens that are part of server matching serverId, not including braces
std::vector<Token> Token::getAllServerTokens(int serverId, std::vector<Token>& allTokens)
{
	std::vector<Token> serverTokens;
	//Token::printAllTokensInfo(allTokens);
	for(size_t i = 0; i < allTokens.size(); i++)
	{
		std::vector<Token> tokenPath = allTokens[i].getTokenPath();
		if(tokenPath.size() < 2)
			continue;
		if(tokenPath[1].getTokenContextType() == SERVER && tokenPath[1].getTokenServerId() == serverId)
		{
			if(allTokens[i].getTokenType() == OPENING_BRACE || allTokens[i].getTokenType() == CLOSING_BRACE)
				continue;
			serverTokens.push_back(allTokens[i]);
		}
	}
	return serverTokens;
}

const Token::TokenType& Token::getTokenType(void) const
{
	return (_type);
}

const Token::ContextType& Token::getTokenContextType(void) const 
{
	return(_contextType);
}

const int& Token::getTokenServerId(void) const
{
	return (_serverId);
}

const size_t& Token::getTokenLineNumber() const
{
	return (_lineNumber);
}

const std::string& Token::getTokenInfo() const
{
	return (_info);
}

const Token::ContextType& Token::getCurrentTokenContextType() const
{
	return _contextType;
}

const std::vector<Token>& Token::getTokenPath(void) const
{
	return (_path);
}

void Token::setTokenPath(std::vector<Token> path)
{
	_path = path;
}

//set token type of enum TokenTYpe
void Token::setTokenType(TokenType newType)
{
	_type = newType;
}

Token::Token()
{
	_info = "";
	_type = UNKOWN;
	_lineNumber = 0;
	_serverId = 0;
	_contextType = NOT_CONTEXT;
}
// TYPE will be then auto seted if info is opening or closing bracket 
// if string ends with ; it is directive, otherwise it is Context
// use setTokenType function to give type to be context or Directive
Token::Token(const std::string& info, size_t lineNumber, int serverId)
:_info(info), _type(UNKOWN), _lineNumber(lineNumber), _serverId(serverId), _contextType(NOT_CONTEXT)
{
	if(_info == "{")
		_type = OPENING_BRACE;
	else if(_info == "}")
		_type = CLOSING_BRACE;
	else if(_info[_info.size() - 1] == ';')
		_type = DIRECTIVE;
	else 
	{
		_type = CONTEXT;
		_contextChecker();
	}
}

Token& Token::operator=(const Token& source)
{
	_info = source._info;
	_type = source._type;
	_lineNumber = source._lineNumber;
	_serverId = source._serverId;
	_contextType = source._contextType;
	_path = source._path;
	return (*this);
}

Token::Token(const Token& source)
{
	*this = source;
}

Token::~Token()
{

}


void Token::_checkOneToken(void)
{
	if(_type == CONTEXT)
		_contextChecker();
	//directives are checked while applying
}

//check if context is valid 
//set Context type if context is valid
//throw InvalidTokenException if context is not listed in const validContexts variable
void Token::_contextChecker(void)
{
	//std::cout << "Checking " << _info << std::endl;
	const std::string location("location");
	for(size_t i = 0; i < sizeof(_validContexts) / sizeof(std::string); i++)
	{
		if(_info == _validContexts[i])
		{
			if(_validContexts[i] == "http")
				_contextType = HTTP;
			if(_validContexts[i] == "server")
				_contextType = SERVER;
			return;
		}
		if(_info.compare(0,location.size(),location) == 0)
		{
			_contextType = LOCATION;
			//std::cout << _info << " is " << location << std::endl;
			return;
		}
	}
	std::ostringstream oss("Unkown context [");
	oss << _info << "] in line ";
	Logger::error(oss.str(), _lineNumber);
	throw InvalidTokenException();
}

const char * Token::InvalidTokenException::what() const throw()
{
	return("Exception Invalid token");
}

std::ostream& operator<<(std::ostream& os, const Token& token)
{
	std::string facnyTitle = Logger::createFancyTitle("TOKEN INFO");
	os << facnyTitle<<std::endl;
	os << "Token content: " << token._info << std::endl;
	os << "Server id :" << token._serverId << std::endl;
	os << "Token type: "<< token._type << std::endl;
	os << "Context type is " << token._contextType << std::endl;
	os << "Token line number: " << token._lineNumber << std::endl;
	os << "Token path is :";
	for(size_t i = 0; i < token._path.size(); i++)
	{
		os << token._path[i]._info;
		if(i != token._path.size() - 1)
			os <<" ";
	}
	os << std::endl;
	os << "Token context type is " << token._contextType << std::endl;
	os << Logger::getCharSequence(facnyTitle.size())<< std::endl;
	return os;
}
