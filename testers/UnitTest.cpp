#include "UnitTest.hpp"
#include <assert.h>
#include <cerrno>
#include <cstdio>
#include <exception>
#include "../srcs/Parsing/Configuration.hpp"
#include <iostream>
#include <string>
#include "../srcs/Utils/Logger.hpp"
#include "../srcs/Parsing/ParsingUtils.hpp"
#include "../srcs/Utils/UrlSuffix.hpp"
#include "../srcs/Utils/FileUtils.hpp"


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

void UnitTest::stringEndCheck()
{
	Logger::testCase("Testing is string end function", "");
	bool result = ParsingUtils::isStringEnd("hej\r\n\r\n", "\r\n\r\n");
	assert(result == true);
	_testpassed();
}

void UnitTest::filerOrFolderTestCase(const std::string path, int expected, int expectedHttp)
{
	Logger::testCase("Testing file or folder function of path: ", path);
	int httpStatus;
	int result = FileUtils::isPathFileOrFolder(path, httpStatus);
	assert(result == expected);
	if(result == -1)
	{
		std::cout << "Real status code: " << httpStatus << "expected status code :" << expectedHttp << std::endl;
		assert(httpStatus == expectedHttp);
	}
	_testpassed();
}

void UnitTest::filerOrFolderBlock()
{
	filerOrFolderTestCase("html", 2, 0);
	filerOrFolderTestCase("html/403.html", 1, 0);
	filerOrFolderTestCase("heheheheh", -1 , 404);
	filerOrFolderTestCase("cgi-bin", 2, 0);
	_testpassed(true);
}

void UnitTest::urlPathTesterBlock()
{
	urlPathTester("/path/to/resource", "/path/to/resource");
	urlPathTester("", "");
	urlPathTester("/path/to/resource?hej", "/path/to/resource");
	urlPathTester("/path/to/resource", "/path/to/resource");
	urlPathTester("/path/to/resource?query=1", "/path/to/resource");
	urlPathTester("/path/to/resource?querry = 2&3", "/path/to/resource");
	_testpassed(true);
}

void UnitTest::urlPathTester(std::string suffixString, std::string expectedpath)
{
	Logger::testCase("Testing url path", expectedpath);
	UrlSuffix urlsuffix(suffixString);
	std::cout << "Testing ["<<suffixString<<"]" << "Result " << urlsuffix.getPath() << std::endl;
	assert(urlsuffix.getPath() == expectedpath);
	_testpassed();
}



void UnitTest::urlPathQueryBlock()
{
	urlQueryTester("/path/to/resource", "");
	urlQueryTester("", "");
	urlQueryTester("/path/to/resource?hej", "hej");
	urlQueryTester("/path/to/resource", "");
	urlQueryTester("/path/to/resource?query=1", "query=1");
	urlQueryTester("/path/to/resource?querry = 2&3", "querry = 2&3");
	_testpassed(true);
}



void UnitTest::urlQueryTester(std::string suffixString, std::string expectedQuery)
{
	Logger::testCase("Testing url query", expectedQuery);
	UrlSuffix urlsuffix(suffixString);
	std::cout << "Testing ["<<suffixString<<"]" << "Result " << urlsuffix.getQueryParameters() << std::endl;
	assert(urlsuffix.getQueryParameters() == expectedQuery);
	_testpassed();
}

void UnitTest::stringDelimCheck()
{
	Logger::testCase("Extract testing ", "");
	std::string hej = ParsingUtils::extractUntilDelim("hej ja sam\r\n\r\n djevojka", "\r\n\r\n");
	assert(hej == "hej ja sam\r\n\r\n");
	std::string result = ParsingUtils::extractAfterDelim("hej/zasto/basti?mojaIkono", "?");
	assert(result == "mojaIkono");
	std::string result1 = ParsingUtils::extractAfterDelim("hej/zasto/basti?mojaIkono", "g");
	assert(result1 == "");
	_testpassed();
}


void UnitTest::testingOpeninDirBlock()
{
	testOpeningDirCase("Configuration", 0);
	testOpeningDirCase("./Configuration/", 0);
	testOpeningDirCase("html/403.html", 404);
	testOpeningDirCase("testers/NoPermisionFolder", 403);
	testOpeningDirCase("testers/EmptyFolder/", 0);
}

void UnitTest::testOpeningDirCase(const std::string path, int expected_error)
{
	Logger::testCase("Testing opening dir "); std::cout << path << std::endl;
	int statusCode = 0;
	bool result =  FileUtils::isDirectoryValid(path, statusCode);
	std::cout << "Result is " << result << std::endl;
	perror("Opening dir failed");
	errno = 0;
	assert(statusCode == expected_error);
	_testpassed();
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
	_configFileSyntaxCheck(folder + "ErrorOutOfRange.conf", true);
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
	stringEndCheck();
	_baseNameBlock();
	configSyntaxBlock();
	configNumberOfServersBlock();
	testDirBlock();
	urlPathTesterBlock();
	urlPathQueryBlock();
	testingOpeninDirBlock();
	filerOrFolderBlock();
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
