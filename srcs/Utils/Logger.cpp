#include "Logger.hpp"
#include <iostream>
#include <sys/time.h>
#include <ctime>
#include <iomanip>

const std::string Logger::GREEN = "\033[32m";
const std::string Logger::YELLOW = "\033[33m";
const std::string Logger::RED = "\033[31m";
const std::string Logger::RESET = "\033[0m";
const std::string Logger::BLUE = "\033[34m";



bool Logger::_isPrintingAllowed()
{
    if(PRINT == 1)
    {
        return true;
    }
    return false;
}

void  Logger::_printCurrentTime() {
    timeval curTime;
    gettimeofday(&curTime, NULL);

    // Get the current time in seconds
    time_t now = curTime.tv_sec;
    tm* localTime = localtime(&now);

    // Extract minutes, seconds, and milliseconds
    int minutes = localTime->tm_min;
    int seconds = localTime->tm_sec;
    int milliseconds = curTime.tv_usec / 1000;

    // Print the time in the desired format
    std::cout << std::setw(2) << std::setfill('0') << minutes << ":"
              << std::setw(2) << std::setfill('0') << seconds << ":"
              << std::setw(3) << std::setfill('0') << milliseconds << " ";
}


void Logger::info(std::string message, bool newline)
{
    if(_isPrintingAllowed() == false)
    {
        return;
    }
	std::cout << GREEN;
	_printCurrentTime();
	std::cout <<"INFO: " << message << RESET;
    if(newline == true)
        std::cout << std::endl;
}

void printEscapeCharacters(const std::string& str) {
    for (std::string::size_type i = 0; i < str.size(); ++i) {
        char c = str[i];
        switch (c) {
            case '\n': std::cout << "\\n"; break;
            case '\r': std::cout << "\\r"; break;
            case '\t': std::cout << "\\t"; break;
            case '\\': std::cout << "\\\\"; break;
            case '\"': std::cout << "\\\""; break;
            default:
                std::cout << c;
                break;
        }
    }
}

void Logger::chars(std::string message, bool newline)
{
    if(_isPrintingAllowed() == false)
    {
        return;
    }
	std::cout << BLUE;
	_printCurrentTime();
	std::cout <<"INFO: " << message << RESET;
	printEscapeCharacters(message);
    if(newline == true)
        std::cout << std::endl;
}

void Logger::warning(std::string message, bool newline)
{
    if(_isPrintingAllowed() == false)
    {
        return;
    }
	std::cout << YELLOW;
	_printCurrentTime();
	std::cout <<"WARNING: " << message << RESET;
    if(newline == true)
        std::cout << std::endl;
}
void Logger::error(std::string message, bool newline)
{
	std::cout << RED;
	_printCurrentTime();
	std::cerr <<"ERROR: " << message  << RESET;
    if(newline == true)
        std::cout << std::endl;
}

void Logger::testCase(std::string title ,std::string expectedResult)
{
    std::cout << BLUE;
    std::cout << "Test Case :" << title << ". " << RESET << expectedResult << std::endl;
}
