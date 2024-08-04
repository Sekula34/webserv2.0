#include "UnitTest.hpp"
#include <assert.h>
#include <exception>
#include "../srcs/Parsing/Configuration.hpp"
#include <iostream>
#include <string>
#include "../srcs/Utils/Logger.hpp"
#include "../srcs/Parsing/ParsingUtils.hpp"


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



void UnitTest::_configFileSyntaxCheck(std::string filePath, bool exception)
{
	std::string expecetedMessage;
	std::string title = "Syntax of file: " + filePath;
	if(exception)
		expecetedMessage ="Excpetion should happend";
	else 
		expecetedMessage = "Exception should not happend";

	Logger::testCase(title, expecetedMessage); std::cout << std::endl;
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
	return _testpassed();
}

void UnitTest::_configNumberOfServers(std::string filePath, int expectedNumberOfServers)
{
	std::string fullFilePath = _constFileFolder + filePath;
	_configFileSyntaxCheck(fullFilePath, false);
	std::string title = "Testing number of servers of config file " + fullFilePath; 
	std::string expecetedMessage = ParsingUtils::toString(expectedNumberOfServers);
	Logger::testCase(title, expecetedMessage); std::cout << std::endl;
	Configuration conf(_constFileFolder + filePath);
	assert(conf.getNumberOfServers() == expectedNumberOfServers);
	_testpassed();
}

void UnitTest::_testDirGetter(std::string FullPath, const std::string expectedResult)
{
	Logger::testCase("Dir Getter", expectedResult);
	std::string result = ParsingUtils::getDirName(FullPath);
	//std::cout << "["<< result << "]"<< std::endl;
	assert(result == expectedResult);
	_testpassed();
}

void UnitTest::testDirBlock()
{
	_testDirGetter("hej/I/am/Filip", "hej/I/am/");
	_testDirGetter("hej/I/am/Filip/", "hej/I/am/");
	_testDirGetter("hej/I/am/Filip//", "hej/I/am/Filip/");
	_testDirGetter("hej", "");
	_testDirGetter("/hej", "/");
	_testpassed(true);

}


void UnitTest::configSyntaxBlock()
{
	std::string folder = "testers/ConfigFileTest/TestFiles/";
	_configFileSyntaxCheck("testers/ConfigFileTest/TestFiles/simpleFile.conf",false);
	_configFileSyntaxCheck("nonExistent", true);
	_configFileSyntaxCheck("testers/ConfigFileTest/TestFiles/noPermission.conf", true);
	_configFileSyntaxCheck("testers/ConfigFileTest/TestFiles/bulshit",true);

	_configFileSyntaxCheck(folder + "InvalidContext.conf", true);
	_configFileSyntaxCheck(folder + "InvalidContext2.conf", true);
	_configFileSyntaxCheck(folder + "doublehttp.conf", true);
	_configFileSyntaxCheck(folder + "simpleServer.conf", false);
	_configFileSyntaxCheck(folder + "ThreeServers.conf", false);
	_configFileSyntaxCheck(folder + "zeroServer.conf", false); // not sure about this one, should it be exceptions or not
	return _testpassed(true);
}


void UnitTest::configNumberOfServersBlock()
{
	_configNumberOfServers("simpleFile.conf", 1);
	_configNumberOfServers("simpleServer.conf",1);
	_configNumberOfServers("ThreeServers.conf",3);
	_configNumberOfServers("zeroServer.conf", 0);
	return _testpassed(true);
}
void UnitTest::allTests()
{
	_baseNameBlock();
	configSyntaxBlock();
	configNumberOfServersBlock();
	testDirBlock();
	//_serveTestCase();
}

void UnitTest::_baseNameBlock()
{
	_baseNameTestCase("/localhost/bokte/nece/stajeovo/", "/", "localhost/bokte/nece/stajeovo/");
	_baseNameTestCase("/localhost/bokte/nece/stajeovo/", "/localhost/", "bokte/nece/stajeovo/");
	_baseNameTestCase("/localhost/bokte/nece/stajeovo/", "/localhost/bokte/nece/", "stajeovo/");
	_baseNameTestCase("/localhost/bokte/nece/stajeovo", "/localhost/bokte/nece/", "stajeovo");
	_baseNameTestCase("/html/first/second.html", "/html/first/", "second.html");
	_baseNameTestCase("/", "/", "");
}
void UnitTest::_baseNameTestCase(std::string url, std::string serverLocation, std::string expected)
{
	std::string titleMessage = "Base name tester of " + url + " and " + serverLocation + "\n";
	Logger::testCase(titleMessage, expected);
	std::string baseName = ParsingUtils::getBaseName(url, serverLocation);
	assert(baseName == expected);
}
