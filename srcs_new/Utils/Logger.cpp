#include "Logger.hpp"
#include <cstddef>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <ctime>
#include <iomanip>

std::map<Logger::e_color, std::string> autoSetColorMap()
{
	std::map<Logger::e_color, std::string> customMap;
	customMap[Logger::GREEN] = "\033[32m";
	customMap[Logger::YELLOW] = "\033[33m";
	customMap[Logger::RED] = "\033[31m";
	customMap[Logger::RESET] = "\033[0m";
	customMap[Logger::BLUE] = "\033[34m";
	customMap[Logger::PURPLE] = "\033[35m";
	return customMap;
}

std::map<Logger::e_color, std::string> Logger::_colorMap = autoSetColorMap();


std::ostringstream&	Logger::log(std::string title, std::string type, e_color color)
{
	return Logger::log(title, "", type, color);
}

std::string Logger::createFancyTitle(const std::string title, char c, size_t numberOfChars)
{
	std::ostringstream oss;
	oss << getCharSequence(numberOfChars, c);
	oss << title; 
	oss << getCharSequence(numberOfChars, c);
	return oss.str();
}

// void printEscapeCharacters(const std::string& str) // MR: Old function prototype
void printEscapeCharacters(const std::string& str, int mode)
{
	char previous = '\0';
	for (std::string::size_type i = 0; i < str.size(); ++i)
	{
		char c = str[i];
		switch (c)
		{
			case '\n': std::cout << "\\n"; break;
			case '\r': std::cout << "\\r"; break;
			case '\t': std::cout << "\\t"; break;
			case '\\': std::cout << "\\\\"; break;
			case '\"': std::cout << "\\\""; break;
			default: std::cout << c; break;
		}
		// MR: START Proposed changes
		if (mode == 2 && c == '\n')
			std::cout << std::endl;
		else if (mode == 3 && c == '\n' && (previous == '\r' || i == str.size() - 1))
			std::cout << std::endl;
		previous = c;
		// MR: END Proposed changes
	}
}

// void Logger::chars(std::string message, bool newline) // MR: Old function prototype
void Logger::chars(std::string message, int mode)
{
	if(_isPrintingAllowed() == false)
	{
		return;
	}
	std::cout << _colorMap[PURPLE];
	std::cout << _printCurrentTime().str();
	std::cout <<"CHARS: " << _colorMap[RESET];
	// printEscapeCharacters(message,);
	// if(newline == true)
	printEscapeCharacters(message, mode); // MR: Proposed change
	if(mode == true)
		std::cout << std::endl;
}

std::string  Logger::getCharSequence(const size_t numberOfChars, char c)
{
	std::ostringstream oss;
	for (size_t i = 0; i < numberOfChars; i++)
		oss << c;
	return  oss.str();
}

std::ostringstream&    Logger::_printCurrentTime()
{
    static std::ostringstream oss;
    oss.str(""); // Clear the stream to reuse it
    oss.clear(); // Clear any error flags

    // Get the current time in seconds
    time_t now = time(0);
    tm* localTime = localtime(&now);

    // Extract minutes and seconds
    int minutes = localTime->tm_min;
    int seconds = localTime->tm_sec;

    // Approximate milliseconds using clock ticks since the program started
    int milliseconds = static_cast<int>((clock() % CLOCKS_PER_SEC) * 1000 / CLOCKS_PER_SEC);

    // Print time in the desired format
    oss << std::setw(2) << std::setfill('0') << minutes << ":"
        << std::setw(2) << std::setfill('0') << seconds << ":"
        << std::setw(3) << std::setfill('0') << milliseconds << " ";
    return (oss);
}

// std::ostringstream&  Logger::_printCurrentTime() {
// 	timeval curTime;
// 	gettimeofday(&curTime, NULL);
// 	static std::ostringstream oss;
//     oss.str(""); // Clear the stream
//     oss.clear(); // Clear any error flags
//
// 	// Get the current time in seconds
// 	time_t now = curTime.tv_sec;
// 	tm* localTime = localtime(&now);
//
// 	// Extract minutes, seconds, and milliseconds
// 	int minutes = localTime->tm_min;
// 	int seconds = localTime->tm_sec;
// 	int milliseconds = curTime.tv_usec / 1000;
//
// 	// Print the time in the desired format
// 	oss << std::setw(2) << std::setfill('0') << minutes << ":"
// 			<< std::setw(2) << std::setfill('0') << seconds << ":"
// 			<< std::setw(3) << std::setfill('0') << milliseconds << " ";
// 	return  oss;
// }

bool Logger::_isPrintingAllowed()
{
	if (PRINT == 1)
	{
		return true;
	}
	return false;
}
