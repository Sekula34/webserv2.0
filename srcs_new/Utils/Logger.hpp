#ifndef LOGGER_HPP
# define LOGGER_HPP

# ifndef PRINT 
#  define PRINT 1
# endif

#include <string>
#include <iostream>

/**
 * @brief static class for printing info, warning and error.
 * _printCurrentTime that this function does is in c and should change that to c++ way.
 * MAYBE YOU SHOULD DELETE THIS BEFORE SUBMITING BECASE OF WRITING TO ANY FILE DESCRIPTOR
 * 
 */

class Logger 
{

	private:
		static void _printCurrentTime();
		static bool _isPrintingAllowed();
		static const std::string returnColor();

	public :
		static const std::string GREEN;
		static const std::string YELLOW;
		static const std::string RED;
		static const std::string RESET;
		static const std::string BLUE;
		static const std::string PURPLE;

		static void log(std::string title, std::string type = "INFO", std::string color = GREEN);

		template <typename T>
		static void log(std::string title, T value = "", std::string type = "INFO", std::string color = GREEN)
		{
			if(_isPrintingAllowed() == false)
			{
				return;
			}
			std::cout << color;
			_printCurrentTime();
			std::cout << type << " " << title << " " << RESET;
			std::cout << value << std::endl;
		}

		template <typename T>
		static void warning(std::string title, T value = "")
		{
			log(title, value, "WARNING", YELLOW);
		}

		template <typename T>
		static void error(std::string title, T value = "")
		{
			log(title, value, "ERROR", RED);
		}

		template <typename T>
		static void info(std::string title, T value = "")
		{
			log(title, value, "INFO", GREEN);
		}
		
		template <typename T>
		static void testCase(std::string title, T value = "")
		{
			log(title, value, "TESTCASE", BLUE);
		}

		static void chars(std::string message, bool newline);

		// TODO: adjust tester
		//static void testCase(std::string title, std::string expectedResult = "");
};

/* ============================ Implementations =========================== */
// template <typename T>
// void Logger<T>::info(std::string title, T value, bool newline = false)
// {
// 	if(_isPrintingAllowed() == false)
// 	{
// 		return;
// 	}
// 	std::cout << GREEN;
// 	_printCurrentTime();
// 	std::cout <<"INFO: " << message << RESET;
// 	if(newline == true)
// 		std::cout << std::endl;
// }

#endif
