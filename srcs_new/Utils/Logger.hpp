#ifndef LOGGER_HPP
# define LOGGER_HPP

#include <cstddef>
#include <ostream>
#include <sstream>
#include <vector>
# ifndef PRINT 
#  define PRINT 1
# endif

#include <string>
#include <iostream>
#include <map>

/**
 * @brief static class for printing info, warning and error.
 * _printCurrentTime that this function does is in c and should change that to c++ way.
 * MAYBE YOU SHOULD DELETE THIS BEFORE SUBMITING BECASE OF WRITING TO ANY FILE DESCRIPTOR
 * 
 */

class Logger 
{
	public: 
		enum e_color
		{
			GREEN,
			YELLOW,
			RED,
			RESET,
			BLUE,
			PURPLE
		};
	public :
		static std::ostringstream& log(std::string title, std::string type = "INFO", e_color color = GREEN);

		template <typename T>
		static std::ostringstream& log(std::string title, T value = "", std::string type = "INFO", e_color color = GREEN)
		{
			static std::ostringstream oss;
			oss.str("");
			oss.clear();
			oss << _colorMap[color];
			oss << _printCurrentTime().str();
			oss << type << " " << title << " " << _colorMap[RESET];
			oss << value << std::endl;
			return oss;
		}

		template <typename T>
		static void warning(std::string title, T value = "")
		{
			if(_isPrintingAllowed() == false)
				return;
			std::cout << log(title, value, "WARNING", YELLOW).str();
		}

		template <typename T>
		static void error(std::string title, T value = "")
		{
			if(_isPrintingAllowed() == false)
				return;
			std::cout << log(title, value, "ERROR", RED).str();
		}

		template <typename T>
		static void info(std::string title, T value = "")
		{
			if(_isPrintingAllowed() == false)
				return;
			std::cout << log(title, value, "INFO", GREEN).str();
		}
		
		template <typename T>
		static void testCase(std::string title, T value = "")
		{
			if(_isPrintingAllowed() == false)
				return;
			std::cout << log(title, value, "TESTCASE", BLUE).str();
		}


		template<typename T>
		static void printVector(std::vector<T> customVector, std::string vectorTitle = "Vector Print")
		{
			if(_isPrintingAllowed() == false)
				return;
			std::cout << logVector(customVector, vectorTitle).str();
		}

		template <typename T>
		static std::ostringstream& logVector(std::vector<T> customVector, std::string vectorTitle)
		{
			std::string fancyTitle = createFancyTitle(vectorTitle);
			static std::ostringstream oss;
			oss.str("");
			oss.clear();
			oss << fancyTitle << std::endl;
			if(customVector.size() == 0)
				oss << "Vector is empty" << std::endl;
			else
			{
				for(size_t i = 0; i < customVector.size(); i++)
					oss << customVector[i] << std::endl;
			}
			oss << getCharSequence(fancyTitle.size()) << std::endl;
			return oss;
		}

		template<typename T, typename  U>
		static void printMap(std::map<T, U> customMap, std::string vectorTitle = "Map print")
		{
			if(_isPrintingAllowed() == false)
				return;
			std::cout << logMap(customMap, vectorTitle).str();
		}
		
		template <typename T, typename U>
		static std::ostringstream& logMap(std::map<T, U> customMap, std::string mapTitle)
		{
			std::string fancyTitle = createFancyTitle(mapTitle, '=', 10);
			static std::ostringstream oss;
			oss.str("");
			oss.clear();
			oss << fancyTitle << std::endl;
			if(customMap.size() == 0)
				oss << "map is empty" << std::endl;
			else
			{
				typename std::map<T, U>::const_iterator it = customMap.begin();
				typename std::map<T, U>::const_iterator end = customMap.end();
				for(; it != end; ++it)
				{
					oss << getCharSequence(10, '_') << std::endl;
					oss << "Map key: " << it->first << std::endl;
					oss << "Map Value: " << it->second << std::endl;
					oss << getCharSequence(10, '-') << std::endl;
				}
			}
			oss << std::endl << getCharSequence(fancyTitle.size(), '=') << std::endl;
			return oss;
		}
		static void chars(std::string message, bool newline);
		static std::string 						createFancyTitle(const std::string title, char c = '-', size_t numberOfChars = 10);
		static std::string 						getCharSequence(const size_t numberOfChars = 10, char c = '-');

	private:
		static std::ostringstream&				_printCurrentTime();
		static bool								_isPrintingAllowed();
		static std::map<e_color, std::string>	_colorMap;
};


#endif
