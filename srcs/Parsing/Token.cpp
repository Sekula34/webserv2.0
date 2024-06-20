#include "Token.hpp"
#include <cstddef>
#include <iostream>
#include <vector>




Token::Token()
{

}
//Create Token out of info and set default value of type to be UNKOWN, and lineNumber to 0
// TYPE will be then auto seted if info is opening or closing bracket 
// if string ends with ; it is directive, otherwise it is Context
// use setTokenType function to give type to be context or Directive
Token::Token(const std::string& info, size_t lineNumber, int serverId)
:_info(info), _type(UNKOWN), _lineNumber(lineNumber), _serverId(serverId), _contextType(NOT_CONTEXT)
{
	if(_info == "{")
	{
		_type = OPENING_BRACE;
	}
	else if(_info == "}")
	{
		_type = CLOSING_BRACE;
	}
	else if(_info[_info.size() - 1] == ';')
	{
		_type = DIRECTIVE;
	}
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
	{
		_contextChecker();
	}
	else if(_type == DIRECTIVE)
	{

	}
}

//check if context is valid 
//set Context type if context is valid
//throw InvalidTokenException if context is not listed in const validContexts variable
void Token::_contextChecker(void)
{
	//std::cout << "Checking " << _info << std::endl;
	const std::string location("location");
	for(size_t i = 0; i < sizeof(validContexts) / sizeof(std::string); i++)
	{
		if(_info == validContexts[i])
		{
			if(validContexts[i] == "http")
				_contextType = HTTP;
			if(validContexts[i] == "server")
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
	std::cerr << yellow<< "Unkown context \"" << _info << "\" in line " << _lineNumber << resetText <<std::endl;
	throw InvalidTokenException();
}

//check everyContext in AllTokens
//throw InvalidTokenException if there is Unkown context
//set _contextType while checking
void Token::checkAllTokensContext(std::vector<Token>& allTokens)
{
	for(size_t i = 0; i < allTokens.size(); i++)
	{
		Token& oneToken(allTokens[i]);
		oneToken._checkOneToken();
		//std::cout <<"Checking printer" << std::endl;
		//oneToken.printTokenInfo();
	}
}

//print all infromation of every token in allTokens
void Token::printAllTokensInfo(const std::vector<Token>& allTokens)
{
	for(size_t i = 0; i < allTokens.size(); i++)
	{
		allTokens[i].printTokenInfo();
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

TokenType Token::getTokenType(void) const
{
	return (_type);
}

ContextType Token::getTokenContextType(void) const 
{
	return(_contextType);
}

int Token::getTokenServerId(void) const
{
	return (_serverId);
}

size_t Token::getTokenLineNumber() const
{
	return (_lineNumber);
}

std::string Token::getTokenInfo() const
{
	return (_info);
}

ContextType Token::getCurrentTokenContextType() const
{
	return _contextType;
}

std::vector<Token> Token::getTokenPath(void) const
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

void Token::printTokenInfo(void) const
{
	std::cout << "---------------TOKEN INFO---------------"<<std::endl;
	std::cout << "Token content: " << _info << std::endl;
	std::cout << "Server id :" << _serverId << std::endl;
	std::cout << "Token type: "<<_type << std::endl;
	std::cout << "Context type is " << _contextType << std::endl;
	std::cout << "Token line number: " << _lineNumber << std::endl;
	std::cout << "Token path is :";
	for(size_t i = 0; i < _path.size(); i++)
	{
		std::cout << _path[i]._info;
		if(i != _path.size() - 1)
			std::cout <<" ";
	}
	std::cout <<std::endl;
	std::cout << "Token context type is " << _contextType << std::endl;
	std::cout << "________________________________________" << std::endl;
}


const char * Token::InvalidTokenException::what() const throw()
{
	return("Exception Invalid token");
}
