#ifndef LOGGER_HPP
# define LOGGER_HPP
#include <string>


class Logger 
{

	private:
		static const std::string GREEN;
		static const std::string YELLOW;
		static const std::string RED;
		static const std::string RESET;
		static void _printCurrentTime();

	public :
		static void info(std::string message, bool newline = false);
		static void warning(std::string message, bool newline = false);
		static void error(std::string message, bool newline = false);
};

#endif
