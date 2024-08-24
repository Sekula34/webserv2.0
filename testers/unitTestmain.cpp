#include "ServerTest.hpp"
#include "UnitTest.hpp"
#include "CHTest.hpp"
#include "AutoIndexTest.hpp"



void all()
{
	UnitTest::allTests();
	ServerTest::runAllTests();
	CHTest::runAllTests();
	AutoindexTest::runAll();
}

int main()
{
	//AutoindexTest::validSimpleTest();
	//UnitTest::filerOrFolderBlock();
	//UnitTest::testingOpeninDirBlock();
	all();
	//UnitTest::stringDelimCheck();
	//UnitTest::urlPathTesterBlock();
	//UnitTest::urlPathQueryBlock();
	//UnitTest::urlFragmetBlock();
	//CHTest::uriTest();
	//UnitTest::urlValid();
}
