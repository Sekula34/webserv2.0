#include "ServerTest.hpp"
#include "UnitTest.hpp"
#include "CHTest.hpp"



void chTests()
{
	CHTest::runAllTests();
}

int main()
{
	//std::cout << "Just new line " << std::endl;
	
	//UnitTest::allTests();
	//ServerTest::runAllTests();
	//CHTest::runAllTests();
	
	CHTest::simplePrint();
	//UnitTest::stringEndCheck();
	//UnitTest::stringDelimCheck();
	
	//chTests();

	//UnitTest::testDirBlock();
	//UnitTest::configTestBlock();

	//UnitTest::serverTestBlock();
	//ServerTest::serverTestCase();
	//std::cout << "Test Passed" << std::endl;
}
