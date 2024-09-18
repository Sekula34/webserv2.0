#ifndef LOGGER_HPP
# define LOGGER_HPP

# ifndef PRINT 
#  define PRINT 1
# endif

#include <string>

/**
 * @brief static class for printing info, warning and error.
 * _printCurrentTime that this function does is in c and should change that to c++ way.
 * MAYBE YOU SHOULD DELETE THIS BEFORE SUBMITING BECASE OF WRITING TO ANY FILE DESCRIPTOR
 * 
 */
class Logger 
{

	private:
		static const std::string GREEN;
		static const std::string YELLOW;
		static const std::string RED;
		static const std::string RESET;
		static const std::string BLUE;
		static const std::string PURPLE;
		static void _printCurrentTime();
		static bool _isPrintingAllowed();

	public :
		static void info(std::string message, bool newline = false);
		static void warning(std::string message, bool newline = false);
		static void error(std::string message, bool newline = false);
		static void testCase(std::string title, std::string expectedResult = "");
		static void chars(std::string message, bool newline);
};

#endif
