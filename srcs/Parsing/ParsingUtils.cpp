#include "ParsingUtils.hpp"
#include <cctype>
#include <cstddef>
#include <sstream>
#include <stdexcept>
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

const char * ParsingUtils::InvalidConversion::what() const throw()
{
	return("Exception: Invalid Converion");
}
