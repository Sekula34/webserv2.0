#include "ServerTest.hpp"
#include "UnitTest.hpp"
#include "RequestHeaderTester.hpp"
#include "AutoIndexTest.hpp"



void all()
{
	UnitTest::allTests();
	ServerTest::runAllTests();
	RequestHeaderTester::runAllTests();
	AutoindexTest::runAll();
}

int main()
{
	//AutoindexTest::validSimpleTest();
	//UnitTest::filerOrFolderBlock();
	//UnitTest::testingOpeninDirBlock();
	RequestHeaderTester::cgiResponseHeader();
	UnitTest::configSyntaxBlock();
	//all();
	//CHTest::fullUrlTest();
	//UnitTest::stringDelimCheck();
	//UnitTest::urlPathTesterBlock();
	//UnitTest::urlPathQueryBlock();
	//UnitTest::urlFragmetBlock();
	//CHTest::uriTest();
	//UnitTest::urlValid();
	//all();
}
