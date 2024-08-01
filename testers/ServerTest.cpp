#include "ServerTest.hpp"
#include <assert.h>
#include "../srcs/Parsing/Configuration.hpp"
#include <iostream>
#include <vector>
#include "../srcs/Parsing/ServerSettings.hpp"
#include "UnitTest.hpp"

void ServerTest::_portTester(int expectedPort, const ServerSettings& server)
{
	std::cout << "Checking server port. ";
	std::cout << "Expected port is " << expectedPort << std::endl;
	assert(server.getPort() == expectedPort);
	_testpassed();
}

void ServerTest::serverTestCase()
{
	Configuration conf(_constFileFolder + "simpleServer.conf");
	DefaultSettings defSettings;
	std::vector<Token> allTokens = conf.getAllTokens();
	ServerSettings server(1, defSettings, allTokens );
	//server.printServerSettings();

	_portTester(9090, server);

	std::cout << "Checking number of server locations" << std::endl;
	assert(server.getServerLocations().size() == 6);
	_testpassed();



}
