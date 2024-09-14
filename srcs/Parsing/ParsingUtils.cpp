#include "ParsingUtils.hpp"
#include <cctype>
#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "../Utils/Logger.hpp"
#include <iomanip>

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

std::vector<std::string> ParsingUtils::splitString(std::string fullString, std::string delimiter)
{
	std::vector<std::string> result;
	size_t posDelimiter;
	posDelimiter = fullString.find(delimiter);
	while (posDelimiter != std::string::npos)
	{
		std::string oneValue = fullString.substr(0, posDelimiter);
		//std::cout << "One value is [" << oneValue << "]" << std::endl;
		result.push_back(oneValue);
		fullString.erase(0, posDelimiter + delimiter.size());
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

std::string ParsingUtils::extractUntilDelim(const std::string fullString, const std::string delimiter, bool includeDelim)
{
	std::string::size_type pos = fullString.find(delimiter);
	if(pos == std::string::npos)
		return "";
	if(includeDelim)
		pos += delimiter.length();
	return fullString.substr(0, pos);
}

std::string ParsingUtils::extractAfterDelim(const std::string fullString, const std::string delimiter)
{
	std::string::size_type pos = fullString.find(delimiter);
	if(pos == std::string::npos)
		return "";
	pos += delimiter.length();
	return fullString.substr(pos);
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

std::string ParsingUtils::replaceAllCharsInString(std::string fullString, const std::string oldChar, const std::string neuChar)
{
	std::string neuString(fullString);
	size_t pos = neuString.find(oldChar);
	while(pos != std::string::npos)
	{
		neuString.replace(pos, oldChar.size(), neuChar);
		pos += neuChar.size();
		pos = neuString.find(oldChar, pos);
	}
	return neuString;
}

std::string ParsingUtils::getFileNameFromUrl(std::string urlSuffixPath, std::string locationUri)
{
	std::string fileName;
	size_t pos = urlSuffixPath.find(locationUri);
	if(pos == std::string::npos)
	{
		Logger::error("Cannot find locatoin uri in suffix path, this should not happend ever", true);
		return "";
	}
	fileName = urlSuffixPath.substr(pos + locationUri.size());
	return fileName;
}

const char * ParsingUtils::InvalidConversion::what() const throw()
{
	return("Exception: Invalid Converion");
}

std::string	ParsingUtils::uriDecode(bool decodePlusAsSpace, const std::string& input)
{
	std::ostringstream decoded;
	for (size_t i = 0; i < input.length(); ++i)
	{

		// 1st check mode and + for faster performance.
		if (decodePlusAsSpace && input[i] == '+')
			decoded << ' ';
		else if (input[i] == '%' && i + 2 < input.length())
		{
			// Convert hex to char
			std::istringstream hexStream(input.substr(i + 1, 2));
			int hexValue;
			hexStream >> std::hex >> hexValue;
			decoded << static_cast<char>(hexValue);
			i += 2; // Skip over the hex digits
		}
		else
			decoded << input[i];
	}
	return (decoded.str());
}

//============================================================================
//FIXME:================FUNTIONS FOR TESTING OR NOT BEING USED================
//============================================================================
/* 

static bool	ft_isValidURIChar(unsigned char& c)
{
	static std::string validURIChars(VALID_URI_CHARS);
	return (isalnum(c) || validURIChars.find(c) != std::string::npos);
}

//MR_NOTE: This function is not being used.
std::string	ParsingUtils::uriEncode(bool encodeSpaceAsPlus, const std::string& input)
{
	std::ostringstream encoded;
	for (size_t i = 0; i < input.length(); ++i)
	{
		unsigned char c = input[i];
		// 1st check mode and space for faster performance.
		if (encodeSpaceAsPlus && c == ' ')
			encoded << '+';
		// Check if the character is safe (if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~'))
		else if (ft_isValidURIChar(c))
			encoded << c;
		else
		{
			// Convert character to its hex representation
			encoded << '%' << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << int(c);
		}
	}
	return (encoded.str());
}
 */