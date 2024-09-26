#include "DirectiveTester.hpp"
#include "ServerTest.hpp"
#include "UnitTest.hpp"
#include "RequestHeaderTester.hpp"
#include "AutoIndexTest.hpp"
#include "../srcs_new/Utils/Logger.hpp"


// void all()
// {
// 	UnitTest::allTests();
// 	ServerTest::runAllTests();
// 	RequestHeaderTester::runAllTests();
// 	AutoindexTest::runAll();
// 	DirectiveTester::runAll();
// 	Logger::info("\nPassed all unit Testes", true);
// }

int main()
{
	Logger::info("Pa gdje si ruzo", "");
	UnitTest::testingToken();
	// UnitTest::filerOrFolderBlock();
	//AutoindexTest::validSimpleTest();
	//UnitTest::testingOpeninDirBlock();
	//RequestHeaderTester::cgiResponseHeader();
	//UnitTest::configSyntaxBlock();
	// DirectiveTester::testDuplicate();
	// all();
	//CHTest::fullUrlTest();
	//UnitTest::stringDelimCheck();
	//UnitTest::urlPathTesterBlock();
	//UnitTest::urlPathQueryBlock();
	//UnitTest::urlFragmetBlock();
	//CHTest::uriTest();
	//UnitTest::urlValid();
	//all();
}
