#include "UnitTest.hpp"
#include <assert.h>
#include <exception>
#include "../srcs/Parsing/Configuration.hpp"
#include "ServerTest.hpp"
#include <iostream>
#include <vector>



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

void UnitTest::_serveTestCase()
{
	Configuration conf(_constFileFolder + "simpleServer.conf");
	DefaultSettings defSettings;
	std::vector<Token> allTokens = conf.getAllTokens();
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
	return _testpassed(true);
}

void UnitTest::allTests()
{
	//configTestBlock();
	//ServerTest::serverTestCase();
	_serveTestCase();
}
