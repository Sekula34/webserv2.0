#include "DummyServerSettings.hpp"
// #include "DummyClient.hpp"
#include "DummyMessage.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#define CONFIGFILE "DummyConfig.txt"
#define MESSAGE_HEADER "DummyMessageHeader.txt"
#define MESSAGE_BODY "DummyMessageBody.txt"

void	createMessage()
{
	DummyMessage message;
	std::ifstream file(MESSAGE_HEADER);
	if (!file.is_open())
	{
		std::cerr << "Unable to open" << MESSAGE_HEADER << std::endl;
		return ;
	}
	std::string line;
	std::map<std::string, std::string> header;
	while (std::getline(file, line))
	{
		std::string key;
		std::string value;
		std::istringstream lineStream(line);
		if (std::getline(lineStream, key, '=') && std::getline(lineStream, value))
			header[key] = value;
	}
	file.close();

	file.open(MESSAGE_BODY);
	if (!file.is_open())
	{
		std::cerr << "Unable to open" << MESSAGE_HEADER << std::endl;
		return ;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string body = buffer.str();
	file.close();

	message.setHeader(header);
	message.setBody(body);
	message.printMessage();

	return ;
}

void	createServerSettings()
{
	DummyServerSettings serverSettings;
	serverSettings.parseFile(CONFIGFILE);
	serverSettings.printAllValues();
}

int	main()
{
	createServerSettings();
	createMessage();
	// std::cout << "-----------------DEBUGGING-----------------" << std::endl;
	return (0);
}
