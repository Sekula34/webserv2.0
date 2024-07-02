#include "Logger.hpp"
#include <iostream>
#include <sys/time.h>
#include <ctime>
#include <iomanip>

const std::string Logger::GREEN = "\033[32m";
const std::string Logger::YELLOW = "\033[33m";
const std::string Logger::RED = "\033[31m";
const std::string Logger::RESET = "\033[0m";



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


void Logger::info(std::string message)
{
	std::cout << GREEN;
	_printCurrentTime();
	std::cout <<"INFO: " << message << std::endl << RESET;
}

void Logger::warning(std::string message)
{
	std::cout << YELLOW;
	_printCurrentTime();
	std::cout <<"WARNING: " << message << std::endl << RESET;
}
void Logger::error(std::string message)
{
	std::cout << RED;
	_printCurrentTime();
	std::cerr <<"ERROR: " << message << std::endl << RESET;
}
