#include "DummyServerSettings.hpp"
#include "DummyClient.hpp"
#include "DummyMessage.hpp"
#include <iostream>

int	main()
{
	DummyServerSettings serverSettings("test");
	// std::cout << serverSettings << std::endl;
	DummyMessage* message = new DummyMessage("test", 0);
	// std::cout << message << std::endl;
	DummyClient client(4, 7777, 4);
	std::cout << std::endl;
	std::cout << client << std::endl;
	client.setRequestMsg(message);
	std::cout << *client.getMsg(DummyClient::REQ_MSG) << std::endl;
	// std::cout << "-----------------DEBUGGING-----------------" << std::endl;
	delete message;
	return (0);
}

//============================================================================
// Old Code:
// #include <string>
// #include <iostream>
// #include <fstream>
// #include <sstream>
// #define MESSAGE_HEADER "DummyMessageHeader.txt"
// #define MESSAGE_BODY "DummyMessageBody.txt"
// #define CONFIGFILE "DummyConfig.txt"
// void	createServerSettings()
// {
// 	DummyServerSettings serverSettings;
// 	serverSettings.parseFile(CONFIGFILE);
// 	serverSettings.printAllValues();
// }
// void	createMessage()
// {
// 	DummyMessage message;
// 	std::ifstream file(MESSAGE_HEADER);
// 	if (!file.is_open())
// 	{
// 		std::cerr << "Unable to open" << MESSAGE_HEADER << std::endl;
// 		return ;
// 	}
// 	std::string line;
// 	std::map<std::string, std::string> header;
// 	while (std::getline(file, line))
// 	{
// 		std::string key;
// 		std::string value;
// 		std::istringstream lineStream(line);
// 		if (std::getline(lineStream, key, '=') && std::getline(lineStream, value))
// 			header[key] = value;
// 	}
// 	file.close();
// 	file.open(MESSAGE_BODY);
// 	if (!file.is_open())
// 	{
// 		std::cerr << "Unable to open" << MESSAGE_HEADER << std::endl;
// 		return ;
// 	}
// 	std::stringstream buffer;
// 	buffer << file.rdbuf();
// 	std::string body = buffer.str();
// 	file.close();
// 	message.setHeader(header);
// 	message.setBody(body);
// 	message.printMessage();
// 	return ;
// }
