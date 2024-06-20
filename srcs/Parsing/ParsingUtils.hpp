#ifndef PARSINGUTILS_HPP
#define PARSINGUTILS_HPP
#include <cstddef>
#include <exception>
#include <map>
#include <string>
#include <iostream>
#include <vector>

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

		static std::vector<std::string> splitString(std::string fullString, char delimiter);
		static bool isStringValid(std::string toCheck, const std::string ValidValues[], size_t numberOfValids);
		static bool isStringNumber(std::string toCheck);
		static size_t stringToSizeT(std::string string);

		class InvalidConversion : public std::exception
		{
			const char * what() const throw();
		};
};

#endif
