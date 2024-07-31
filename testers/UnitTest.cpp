#include "UnitTest.hpp"
#include <assert.h>
#include <exception>
#include "../srcs/Parsing/Configuration.hpp"
#include <iostream>


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

void UnitTest::configTestBlock()
{
	_configTestCase("testers/ConfigFileTest/TestFiles/simpleFile.conf",false);
	_configTestCase("nonExistent", true);
	_configTestCase("testers/ConfigFileTest/TestFiles/noPermission.conf", true);
	_configTestCase("testers/ConfigFileTest/TestFiles/bulshit",true);
	//Configuration conf("testers/ConfigFileTest/TestFiles/simpleFile.conf");
	//assert(conf.getFilePath() == "testers/ConfigFileTest/TestFiles/simpleFile.conf");
	return _testpassed(true);
}
