#include "ServerTest.hpp"
#include "UnitTest.hpp"
#include "RequestHeaderTester.hpp"
#include "AutoIndexTest.hpp"
#include "../srcs/Utils/Logger.hpp"


void all()
{
	UnitTest::allTests();
	ServerTest::runAllTests();
	RequestHeaderTester::runAllTests();
	AutoindexTest::runAll();
	Logger::info("\nPassed all unit Testes", true);
}

int main()
{
	//AutoindexTest::validSimpleTest();
	//UnitTest::filerOrFolderBlock();
	//UnitTest::testingOpeninDirBlock();
	//RequestHeaderTester::cgiResponseHeader();
	//UnitTest::configSyntaxBlock();
	all();
	//CHTest::fullUrlTest();
	//UnitTest::stringDelimCheck();
	//UnitTest::urlPathTesterBlock();
	//UnitTest::urlPathQueryBlock();
	//UnitTest::urlFragmetBlock();
	//CHTest::uriTest();
	//UnitTest::urlValid();
	//all();
}
