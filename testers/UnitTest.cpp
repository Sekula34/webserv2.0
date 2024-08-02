#include "UnitTest.hpp"
#include <assert.h>
#include <exception>
#include "../srcs/Parsing/Configuration.hpp"
#include "../srcs/Parsing/ServerSettings.hpp"
#include <iostream>
#include <vector>
#include "../srcs/Utils/Logger.hpp"



const std::string UnitTest::_constFileFolder = "testers/ConfigFileTest/TestFiles/";

void UnitTest::_testpassed(bool block)
{
	    const std::string green = "\033[32m";
    // ANSI escape code to reset text formatting
    const std::string reset = "\033[0m";
	const char* green_check_mark = "✅";
    
	if(block)
	{
		std::cout << green_check_mark << green_check_mark << green << "TestBlock passed" << reset << std::endl << std::endl;;
		return;
	}
    std::cout << green_check_mark << green << "testCase passed" << reset << std::endl;
	
}



void UnitTest::_configTestCase(std::string filePath, bool exception)
{
	std::cout << "Testing openning: " << filePath << std::endl;
	if(exception)
	{
		std::cout << "Excpetion should happend" << std::endl;
	}
	else 
	{
		std::cout << "Exception should not happend" << std::endl;
	}
	try 
	{
		Configuration conf(filePath);
		std::cout << conf.getNumberOfServers() << std::endl << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
		if(exception == false)
			assert(0);
		else if (exception == true)
		{
			assert(1);
			return _testpassed();
		}
	}
	if(exception == true)
		assert(0);
	//std::cout << "Test passed" << std::endl;
	return _testpassed();
}

void UnitTest::_configNumberOfServers(std::string filePath, int expectedNumberOfServers)
{
	std::cout << "Testing number of servers of config file " << _constFileFolder + filePath << std::endl;
	std::cout << "Expected number of servers is " << expectedNumberOfServers << std::endl;
	Configuration conf(_constFileFolder + filePath);
	assert(conf.getNumberOfServers() == expectedNumberOfServers);
	_testpassed();
}

void UnitTest::_serveTestCase()
{
	Logger::warning("Not broken");
	Configuration conf(_constFileFolder + "simpleServer.conf");
	DefaultSettings defSettings;
	std::vector<Token> allTokens = conf.getAllTokens();
	//Token::printAllTokensInfo(allTokens);
	ServerSettings server(1, defSettings, allTokens);
	server.printServerTokens();
	server.printAllSettings();
}

void UnitTest::configTestBlock()
{
	std::string folder = "testers/ConfigFileTest/TestFiles/";
	_configTestCase("testers/ConfigFileTest/TestFiles/simpleFile.conf",false);
	_configTestCase("nonExistent", true);
	_configTestCase("testers/ConfigFileTest/TestFiles/noPermission.conf", true);
	_configTestCase("testers/ConfigFileTest/TestFiles/bulshit",true);

	_configTestCase(folder + "InvalidContext.conf", true);
	_configTestCase(folder + "InvalidContext2.conf", true);
	_configTestCase(folder + "doublehttp.conf", true);
	_configTestCase(folder + "simpleServer.conf", false);
	_configTestCase(folder + "ThreeServers.conf", false);
	_configTestCase(folder + "zeroServer.conf", false); // not sure about this one, should it be exceptions or not
	return _testpassed(true);
}

void UnitTest::allTests()
{
	configTestBlock();
	_configNumberOfServers("simpleFile.conf", 1);
	_configNumberOfServers("simpleServer.conf",1);
	_configNumberOfServers("ThreeServers.conf",3);
	_configNumberOfServers("zeroServer.conf", 0);
	//_serveTestCase();
}
