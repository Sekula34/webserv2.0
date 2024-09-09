#include "ServerTest.hpp"
#include "UnitTest.hpp"
#include "CHTest.hpp"
#include "AutoIndexTest.hpp"
#include "HeaderTester.hpp"



void all()
{
	UnitTest::allTests();
	ServerTest::runAllTests();
	CHTest::runAllTests();
	AutoindexTest::runAll();
	HeaderTester::runAll();

}

int main()
{
	//AutoindexTest::validSimpleTest();
	//UnitTest::filerOrFolderBlock();
	//UnitTest::testingOpeninDirBlock();
	//all();
	//CHTest::fullUrlTest();
	HeaderTester::runAll();
	//UnitTest::stringDelimCheck();
	//UnitTest::urlPathTesterBlock();
	//UnitTest::urlPathQueryBlock();
	//UnitTest::urlFragmetBlock();
	//CHTest::uriTest();
	//UnitTest::urlValid();
}
