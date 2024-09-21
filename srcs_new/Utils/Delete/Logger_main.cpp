#include "Logger.hpp"
#include <string>

int main()
{
	int i = 42;
	float f = 84.0f;
	std::string phrase("Hello");

	Logger::info("info title", i);
	Logger::warning("warning title", phrase);
	Logger::error("error title", f);
	Logger::log("log title");
	Logger::chars("hello\r\r", true);
}
