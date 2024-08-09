#include "ServerTest.hpp"
#include "UnitTest.hpp"
#include "CHTest.hpp"



void chTests()
{
	CHTest::CHBlockTest();
}

int main()
{
	//std::cout << "Just new line " << std::endl;
	
	UnitTest::allTests();
	ServerTest::runAllTests();
	chTests();

	//UnitTest::testDirBlock();
	//UnitTest::configTestBlock();

	//UnitTest::serverTestBlock();
	//ServerTest::serverTestCase();
	//std::cout << "Test Passed" << std::endl;
}
