#include "ParsingUtils.hpp"
#include <cctype>
#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

std::vector<std::string> ParsingUtils::splitString(std::string fullString, char delimiter)
{
	std::vector<std::string> result;
	size_t posDelimiter;
	posDelimiter = fullString.find(delimiter);
	while (posDelimiter != std::string::npos)
	{
		std::string oneValue = fullString.substr(0, posDelimiter);
		//std::cout << "One value is [" << oneValue << "]" << std::endl;
		result.push_back(oneValue);
		fullString.erase(0, posDelimiter + 1);
		//std::cout << "New full string is [" << fullString << "] " << std::endl;
		posDelimiter = fullString.find(delimiter);
	}
	result.push_back(fullString);
	return result;
}

std::string ParsingUtils::getStringOutOfVector(const std::vector<std::string> stringVector)
{
	std::string fullString;
	for(size_t i = 0; i < stringVector.size(); i++)
	{
		std::string locationInBracket = "[" + stringVector[i] + "] ";
		fullString += locationInBracket;
	}
	return fullString;
}

//check if all chars in toCheck are digits
//return true if string is number
//return false otherwise
bool ParsingUtils::isStringNumber(std::string toCheck)
{
	for(size_t i = 0; i < toCheck.size(); i++)
	{
		if(std::isdigit(toCheck[i]) == 0)
		{
			return false;
		}
	}
	return true;
}
std::string ParsingUtils::getBaseName(const std::string& fullUrl, const std::string& serverLocationString)
{
	std::string fileName = fullUrl.substr(serverLocationString.size());
	return fileName;
}

bool ParsingUtils::isStringValid(std::string toCheck, const std::string ValidValues[], size_t numberOfValids)
{
	for(size_t i = 0; i < numberOfValids; i++)
	{
		if(toCheck == ValidValues[i])
			return true;
	}
	return false;
}
//return sizeT value of string
//throw InvalidConverion() exception if string is not number
//std::runtime exception if iss to sizeT failed
size_t ParsingUtils::stringToSizeT(std::string string)
{
	if(isStringNumber(string) == false)
	{
		std::cerr << "Cannot convert string: " << string << " to number" << std::endl; 
		throw InvalidConversion();
	}
	std::istringstream iss(string);
	size_t sizeTValue;
	if(!(iss >> sizeTValue))
	{
		std::cerr << "Cannot convert string: " << string << " to number" << std::endl; 
		throw std::runtime_error("istringstrream to size_t failed. Number does not fit in size_t probably");
	}
	return sizeTValue;
}

std::string ParsingUtils::getDirName(std::string fullPath)
{
	if(fullPath.empty() || fullPath.size() == 1)
		throw std::runtime_error("getDirName full path is empty or consist of only one char");
	std::string::iterator lastChar = fullPath.end() - 1;
	if(*lastChar == '/')
	{	
		fullPath.erase(lastChar);
	}
	std::size_t posOfSlash = fullPath.find_last_of('/');
	return fullPath.substr(0, posOfSlash + 1);
}


bool ParsingUtils::isStringEnd(const std::string toCheck,const std::string expectedEnd)
{
	if(toCheck.size() < expectedEnd.size())
		return false;
	size_t posToStart = toCheck.size() - expectedEnd.size();
	std::string end = toCheck.substr(posToStart);
	std::cout << "End is [" << end.size() << "]" <<std::endl;
	std::cout << "Expected end is [" << expectedEnd.size() << "]" << std::endl;
	if(end != expectedEnd)
		return false;
	return true;
}

std::string ParsingUtils::extractUntilDelim(const std::string fullString, const std::string delimiter)
{
	std::string::size_type pos = fullString.find(delimiter);
	if(pos == std::string::npos)
		return "";
	pos += delimiter.length();
	return fullString.substr(0, pos);
}


std::string ParsingUtils::getHttpPlainValue(std::string fieldValue)
{
	std::string PlainValue(fieldValue);
	if(PlainValue[0] == ' ' || PlainValue[0] == '\t')
	{
		PlainValue.erase(0, 1);
	}
	if(*PlainValue.rbegin() == ' ' || *PlainValue.rbegin() == '\t')
	{
		PlainValue.erase(PlainValue.size() - 1);
	}
	return PlainValue;
}

const char * ParsingUtils::InvalidConversion::what() const throw()
{
	return("Exception: Invalid Converion");
}
