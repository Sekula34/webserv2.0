#include "Configuration.hpp"
#include "Token.hpp"
#include <cstddef>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/stat.h> //for checking if file is regular, not udevRandom
#include <vector>
#include "../Utils/Logger.hpp"

//either will stay private or will call Configuration(std::string path) where 
//path will be default config file
Configuration::Configuration()
{
	std::cout << "Configuration class created" << std::endl;
}

//Set filePath to be const std::string variable
Configuration::Configuration(const std::string& filePath)
:_serverId(0),_filePath(filePath)
{
	_checkFileAccessType(); // MR_NOTE: This are file checks (open, etc).
	_copyFileInVector(); // MR_NOTE: This is file parsing/saving.
	_fillTokensVector(); // MR_NOTE: This is tokinizing (extract context, rirectives, etc).
	_fillAllTokensPaths(); // MR_NOTE: This are token checks ({}).
}

Configuration::Configuration(const Configuration& source)
:_filePath(source._filePath)
{
	
}
//maybe should be private
Configuration& Configuration::operator=(const Configuration& source)
{
	(void) source;
	return (*this);
}

Configuration::~Configuration()
{

}

void Configuration::_checkDoubleHttp(const Token& oneToken)
{
	if(oneToken.getTokenInfo() == "http" && _tokensVector.size() != 0)
	{
		Logger::error("Second http context found ind line "); std::cout << oneToken.getTokenLineNumber() <<std::endl;
		throw InvalidConfigFileException();
	}
}

//function that tries to open file in _filePath 
//throw InvalidConfFileException if file cannot be opened, is empty (No reading permmision or file dont exist)
void Configuration::_checkFileAccessType() const
{
	if(_filePath.empty() == true)
	{
		std::cerr << "Provided path to file is empty.";
		throw InvalidConfigFileException();
	}
	std::ifstream file(_filePath.c_str());
	if(file.good() == false) // MR_SEARCH: is goot ok to check like this?
	{
		std::cerr << "Opening file [" << _filePath <<"] failed. Check if file exist (maybe typo) and if you have reading permmision." << std::endl;
		throw InvalidConfigFileException();
	}
	struct stat fileStat; // MR_SEARCH: is goot ok to check like this?
	if(stat(_filePath.c_str(), &fileStat) != 0)
	{
		throw std::runtime_error("Stat function failed");
	}
	if(S_ISREG(fileStat.st_mode) == 0)
	{
		std::cerr<<"File at location:[" << _filePath << "] is not regular file." << std::endl;
		throw InvalidConfigFileException();
	}
	Logger::info("Configuration file is regular file that can be opened", true);
}



//Read config file at location _file path line by line
//call function to cleanLine (line without comments and leading and trailing spaces and tabs)
//store line in vector _fileLine as pair of cleanLine and sizeT absolute line number
//ex http{, 1;  server{, 3
void Configuration::_copyFileInVector(void)
{
	std::ifstream file(_filePath.c_str());
	if(file.is_open() == false)
		throw std::runtime_error("Opening file in _copyFileInVector failed.");
	std::string oneLine;
	size_t lineNumber(1);
	while(std::getline(file, oneLine))
	{
		oneLine = _getCleanConfLine(oneLine);
		if(oneLine.empty() == false)
		{	
			//std::cout << "One clean string in line "<<lineNumber << " is [" << oneLine <<"]" << std::endl;
			std::pair<std::string, size_t> confLinePair;
			confLinePair.first = oneLine;
			confLinePair.second = lineNumber;
			_fileLine.push_back(confLinePair);
		}
		lineNumber++;
	}
}
//return true if char c is {, } or ;
//return false otherwise
bool Configuration::_isCharDelimiter(char c) const
{
	const char delimiters[] ={'{', '}', ';'};
	for(size_t i = 0; i < sizeof(delimiters)/sizeof(char); i++)
	{
		if(c == delimiters[i])
			return true;
	}
	return false;
}


//goes through all tokens and set paths to it 
//throw InvalidConfigexcpetion if opening brace comes after token that is not context
//or there is less than 3 tokens in config file 
//or closing bracket does not have partner
//or if not all opening bracket are closed
//goes through every token
//algo
// token nex = curent + 1;
// //set current Token path
// if(next == { )
// {
// 	if(current is not directive )
// 		error
// 	tokenPath += curent;
// 	continue;
// }
// if(next == })
// {
// 	if(tokenPath is 0)
// 		closing unopend bracket error;
// 	remove last from TokenPath;
// }
void Configuration::_fillAllTokensPaths(void)
{
	std::vector<Token> tokenPath;
	if(_tokensVector.size() < 3)
	{
		std::cerr<<yellow<<"Configuration file should have at least 3 tokkens http and {}." << resetText << std::endl;
		throw InvalidConfigFileException();
	}
	for(size_t i = 0; i < _tokensVector.size() - 1; i++)
	{
		Token &currentToken = _tokensVector[i];
		Token nextToken = _tokensVector[i + 1]; // MR_DOUBT: This doesnt trigger "invalid read"?
		currentToken.setTokenPath(tokenPath); // MR_DOUBT: What is tokenPath? It seems its the {}
		if(nextToken.getTokenType() == OPENING_BRACE)
		{
			if(currentToken.getTokenType() != CONTEXT)
			{
				std::cerr << yellow << "In line " << currentToken.getTokenLineNumber() << " Token :\"";
				std::cerr<< currentToken.getTokenInfo() <<"\" is not CONTEXT so '{' cannot come after it" << resetText <<std::endl;
				throw InvalidConfigFileException();
			}
			tokenPath.push_back(currentToken);
			continue;
		}
		if(nextToken.getTokenType() == CLOSING_BRACE)
		{
			if(tokenPath.size() == 0)
			{
				std::cerr<<yellow<< "'}' in line " << nextToken.getTokenLineNumber();
				std::cerr<< " does not have matching opening brace '{'" << resetText << std::endl;
				throw InvalidConfigFileException();
			}
			tokenPath.pop_back();
		}
	}
	if(tokenPath.size() > 0)
	{
		std::cerr << yellow<< tokenPath.size() << " * '}' closing bracket is missing" <<resetText << std::endl;
		throw InvalidConfigFileException();
	}
}


//funtion that creates one token from tokenInfo and pushed it back in _tokensVector
//if tokenInfo is "server" add id to id 
//if token is empty do nothing// not even exception maybe should change
void Configuration:: _createOneToken(std::string& tokenInfo, size_t lineNumber)
{
	if(tokenInfo.empty() == true)
	{
		return;
	}
	if(tokenInfo == "server")
	{
		_serverId ++;
	}
	Token oneToken(tokenInfo,lineNumber,_serverId);
	_checkDoubleHttp(oneToken);
	//check double http
	//oneToken.printTokenInfo();
	_tokensVector.push_back(oneToken);
}



//goes char by char in vector and creates Token out of each thing
//if token info is not empty at the end throw exception INVALID CONFIG FILE
//ifchar is ; add it to token info
	//create Token fromTokenInfo
	//clear tokenInfo
	//increase j by 1 since ; should not go in next iter and continue(this will increase j by 2)
// else (delim is { or })
	// create onetoken from tokenInfo
	//clear token info
	// create second token only from delimiter
	//continue
void Configuration::_fillTokensVector(void)
{
	std::string tokenInfo;
	for(size_t i = 0; i < _fileLine.size(); i++)
	{
		for(size_t j = 0; j<_fileLine[i].first.size(); j ++)
		{
			char currentChar = _fileLine[i].first[j];
			if(_isCharDelimiter(currentChar) == true)
			{
				if(currentChar == ';')
				{
					tokenInfo += currentChar;
					_createOneToken(tokenInfo, _fileLine[i].second);
					tokenInfo.clear();
					// IF SOMETHING WILL BREAK UNCOOMENT THISj++;
					continue;
				}
				else 
				{
					_createOneToken(tokenInfo, _fileLine[i].second);
					tokenInfo.clear();
					std::string bracket(1,currentChar);
					//std::cout << "Bracket is [" << bracket <<"]"<<std::endl;
					_createOneToken(bracket, _fileLine[i].second);
					continue;
				}
			}
			tokenInfo += currentChar;
		}
	}
	if(tokenInfo.empty() == false)
	{
		std::cerr <<yellow << "Token \""<< tokenInfo <<"\" is not delimited with anything." <<resetText <<std::endl;
		throw InvalidConfigFileException();
	}
}





//takes server id as integer ServerId ad return string that is always
//"server{ServerId} ex 1, will return server1"
//MR_NOTE: This function is not being used.
std::string Configuration::_generateServerIdString(int serverId)
{
	std::string indexedServer;
	std::stringstream ss;
	ss << serverId;
	std::string idAsString = ss.str();
	if(ss.fail())
	{
		throw std::runtime_error("in _generateServerIdString stringstream ss failed");
	}
	indexedServer = "server_" + idAsString;
	return (indexedServer);
}

//take potential dirtyLine and return clean one
// line like this   #will ignore everthing after # and remove leading and trailing spaces and tabs
std::string Configuration::_getCleanConfLine(const std::string& dirtyLine) 
{
	std::string cleanLine(dirtyLine);
	size_t posOfHash = cleanLine.find('#');
	if(posOfHash != std::string::npos)
	{
		cleanLine = cleanLine.substr(0, posOfHash);
		if (cleanLine.empty() == true)
			return cleanLine;
	}
	_removeSpacesAndTabs(cleanLine);
	return cleanLine;
}



//will remove newlineaswell;
void Configuration::_removeSpacesAndTabs(std::string& dirtyLine)
{
	size_t beginPos(0);
	size_t endPos = dirtyLine.size();
	for(size_t i = 0; i < dirtyLine.size(); i++)
	{
		if(dirtyLine[i] == ' ' || dirtyLine[i] == '\t')
		{
			beginPos++;
		}
		else break;
	}
	for(size_t i = endPos - 1; i > 0; i--)
	{
		if(dirtyLine[i] == ' ' || dirtyLine[i] == '\t' || dirtyLine[i] == '\n')
		{
			endPos--;
		}
		else break;
	}
	if(endPos <= beginPos)
	{
		dirtyLine.clear();
		return;
	}
	dirtyLine = dirtyLine.substr(beginPos, endPos - beginPos);
}

//return string to relative path of ConfFile inside project
//MR_NOTE: This function is not being used.
const std::string& Configuration::getFilePath() const
{
	return (_filePath);
}

//MR_NOTE: This function is not being used.
void Configuration::printFileMember(void)
{
	for(size_t i = 0; i < _fileLine.size(); i++)
	{
		std::cout << "Line " << _fileLine[i].second << " " << _fileLine[i].first << std::endl;
	} 
}

std::vector<Token> Configuration::getAllTokens() const 
{
	return (_tokensVector);
}

int Configuration::getNumberOfServers(void) const
{
	return (_serverId);
}

const char* Configuration::InvalidConfigFileException::what() const throw()
{
	return("Invalid Configuration File");
}
