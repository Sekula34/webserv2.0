#include "ServerTest.hpp"
#include <assert.h>
#include "../srcs/Parsing/Configuration.hpp"
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include "../srcs/Parsing/ServerSettings.hpp"
#include "../srcs/Utils/Logger.hpp"
#include "../srcs/Parsing/ParsingUtils.hpp"

void ServerTest::_portTester(int expectedPort, const ServerSettings& server)
{
	std::string expectedResult = ParsingUtils::toString(expectedPort);
	Logger::testCase("Port Tester",expectedResult);
	assert(server.getPort() == expectedPort);
	_testpassed();
}

void ServerTest::serverTestCase()
{
	std::string fullFilePath = _constFileFolder + "simpleServer.conf";
	std::string title = "Server settings of file :" + fullFilePath;
	Logger::testCase(title, ""); std::cout << std::endl;
	Configuration conf(fullFilePath);
	DefaultSettings defSettings;
	std::vector<Token> allTokens = conf.getAllTokens();
	ServerSettings server(1, defSettings, allTokens);

	_portTester(9090, server);

	std::vector<std::string> expectedLocations;
	expectedLocations.push_back("/");
	expectedLocations.push_back("/redirected/");
	expectedLocations.push_back("/another_location/");
	expectedLocations.push_back("/first/");
	expectedLocations.push_back("/first/second/");
	expectedLocations.push_back("/first/second/third/");

	_locationUriTester(expectedLocations, server);

	//server.printServerTokens();
	//server.printServerSettings();
}

void ServerTest::_locationNumberTester(size_t expectedNumberOfLocations, const ServerSettings& server)
{
	std::string expectedResult = ParsingUtils::toString(expectedNumberOfLocations);
	Logger::testCase("Number of Locations", expectedResult);
	assert(server.getServerLocations().size() == expectedNumberOfLocations);
	_testpassed();
}

void ServerTest::_locationUriTester(const std::vector<std::string> expectedLocations, const ServerSettings& server)
{
	std::string expectedString = ParsingUtils::getStringOutOfVector(expectedLocations);
	Logger::testCase("Servers uri", expectedString); std::cout << std::endl;
	_locationNumberTester(expectedLocations.size(), server);
	for(size_t i = 0; i<expectedLocations.size(); i++)
	{
		std::string fetchedLocationUri = server.getServerLocations()[i].getLocationUri();
		assert(fetchedLocationUri == expectedLocations[i]);
	}
	return _testpassed();
}

void ServerTest::runAllTests()
{
	std::cout<<"Server Tests all" << std::endl;
	serverTestCase();
}
