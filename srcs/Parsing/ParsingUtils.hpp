#ifndef PARSINGUTILS_HPP
#define PARSINGUTILS_HPP
#include <cstddef>
#include <exception>
#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

class ParsingUtils 
{
	public :
		template<typename KeyType, typename ValueType>
		static void printMap(std::map<KeyType, ValueType> map, std::string MapTitle ="map")
		{
			std::cout << "Map " << MapTitle << " values are: " << std::endl;
			typename std::map<KeyType, ValueType>::const_iterator it;
			for(it = map.begin(); it != map.end(); it++)
			{
				std::cout <<"Key :[" << it->first <<"], Value: [" << it->second <<"]" << std::endl;
			}
			std::cout <<"_____endMap___" << std::endl;
		}

		template<typename Type>
		static void printVector(std::vector<Type> vec, std::string VectorTitle = "vector")
		{
			std::cout << "-----Vector name : " << VectorTitle << " values are: " << std::endl;
			typename std::vector<Type>::const_iterator it;
			for(it = vec.begin(); it != vec.end(); it++)
			{
				std::cout << *it;
				if(it != vec.end() - 1)
					std::cout << ", ";
			}
			std::cout <<std::endl <<"________end of vector ____" << std::endl;
		}

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

		static std::vector<std::string> splitString(std::string fullString, char delimiter);
		static std::string getStringOutOfVector(const std::vector<std::string> stringVector);		
		static bool isStringValid(std::string toCheck, const std::string ValidValues[], size_t numberOfValids);
		static bool isStringNumber(std::string toCheck);
		static std::string getBaseName(const std::string& fullUrl, const std::string& ServerLocationString);
		static size_t stringToSizeT(std::string string);
		/**
		 * @brief Get the Dir Name dir name. Dir name of /hej/filip2/donot.txt
		 *  is /hej/filip2/
		 * Dir name of hej/hej2/hej3/ is hej/hej2/
		 * @param fullPath 
		 * @return std::string dir/name
		 */
		static std::string getDirName(std::string fullPath);
		
		/**
		 * @brief Get the Http Plain Value object
		 * 
		 * 
		 * @param filedValue value that may have one leading and/or one trailing space
		 * @return std::string  fieldValue without leadin and trailing space;
		 */
		static std::string getHttpPlainValue(std::string fieldValue);

		class InvalidConversion : public std::exception
		{
			const char * what() const throw();
		};
};

#endif
