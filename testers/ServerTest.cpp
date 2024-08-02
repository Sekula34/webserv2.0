#include "ServerTest.hpp"
#include <assert.h>
#include "../srcs/Parsing/Configuration.hpp"
#include <iostream>
#include <vector>
#include "../srcs/Parsing/ServerSettings.hpp"
#include "../srcs/Utils/Logger.hpp"

void ServerTest::_portTester(int expectedPort, const ServerSettings& server)
{
	std::cout << "Checking server port. ";
	std::cout << "Expected port is " << expectedPort << std::endl;
	std::cout << server.getPort();
	assert(server.getPort() == expectedPort);
	_testpassed();
}

void ServerTest::serverTestCase()
{
	Logger::warning("Broken test case");
	Configuration conf(_constFileFolder + "simpleServer.conf");
	DefaultSettings defSettings;
	std::vector<Token> allTokens = conf.getAllTokens();
	//Token::printAllTokensInfo(allTokens);
	ServerSettings server(1, defSettings, allTokens);
	DefaultSettings def2;
	ServerSettings server2(1, def2, allTokens);
	//server.printServerTokens();
	server.printAllSettings();

	server2.printAllSettings();
	std::cout << conf.getNumberOfServers();
}

void ServerTest::runAllTests()
{
	std::cout<<"Server Tests all" << std::endl;
	serverTestCase();
}
