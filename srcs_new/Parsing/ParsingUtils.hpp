#ifndef PARSINGUTILS_HPP
#define PARSINGUTILS_HPP
#include <cstddef>
#include <exception>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

#define VALID_URI_CHARS "-_.~"

class ParsingUtils 
{
	public :
		template<typename Type>
		static std::string toString(Type number)
		{
			std::stringstream ss;
			std::string stringValue;
			ss << number;
			if(ss.fail())
			{
				std::cerr << "StringStreamFailed" << std::endl;
				throw std::runtime_error("StringStreamfailed");
			}
			stringValue = ss.str();
			if(ss.fail())
			{
				std::cerr << "StringStreamFailed2" << std::endl;
				throw std::runtime_error("StringStreamfailed2");
			}
			return stringValue;
		}

		static std::vector<std::string>	splitString(std::string fullString, char delimiter);
		static std::vector<std::string>	splitString(std::string fullString, std::string delimiter);
		static std::string				getStringOutOfVector(const std::vector<std::string> stringVector);		
		static bool						isStringValid(std::string toCheck, const std::string ValidValues[], size_t numberOfValids);
		static bool						isCharAscii(const char ch);
		static bool 					isStringAscii(const std::string& stringToCheck);
		static bool						isStringNumber(std::string toCheck);
		static std::string				getBaseName(const std::string& fullUrl, const std::string& ServerLocationString);
		static size_t					stringToSizeT(std::string string);
		static bool 					isStringEnd(const std::string toCheck, const std::string expectedEnd);
		static std::string				uriEncode(bool encodeSpaceAsPlus, const std::string& input);
		static std::string				uriDecode(bool decodePlusAsSpace, const std::string& input);
		/**
		 * @brief return substring from 0 till delimiter is found example for header till /r/n/r/n is found
		 * if delimiter is not found return empty string. 
		 * @param fullString 
		 * @param delimiter 
		 * @return std::string Delimiter is included
		 */
		static std::string				extractUntilDelim(const std::string fullString, const std::string delimiter, bool includeDelim = true);
		static std::string				extractAfterDelim(const std::string fullString, const std::string delimiter);
		/**
		 * @brief Get the Dir Name dir name. Dir name of /hej/filip2/donot.txt
		 *  is /hej/filip2/
		 * Dir name of hej/hej2/hej3/ is hej/hej2/
		 * @param fullPath 
		 * @return std::string dir/name
		 */
		static std::string				getDirName(std::string fullPath);
		
		/**
		 * @brief Get the Http Plain Value object
		 * 
		 * 
		 * @param filedValue value that may have one leading and/or one trailing space
		 * @return std::string  fieldValue without leadin and trailing space;
		 */
		static std::string				getHttpPlainValue(std::string fieldValue);

		static std::string				replaceAllCharsInString(std::string fullString, const std::string oldChar, const std::string neuChar);

		/**
		 * @brief Get the File Name From Urlsuffix, 
		 	example hej/i/am/filip.py/anotherfolder/file/
		 * 
		 * @param urlSuffixPath like hej/i/am/filip.py/anotherfolder/file/
		 * @param locationUri  like hej/i/
		 * @return std::string  am/filip.py/anotherfolder/file or "" if there is no file either location is full path or not match
		 */
		static std::string				getFileNameFromUrl(std::string urlSuffixPath, std::string locationUri);

		class InvalidConversion : public std::exception
		{
			const char * what() const throw();
		};
};

#endif
