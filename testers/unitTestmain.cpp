#include "ServerTest.hpp"
#include "UnitTest.hpp"

#include <iostream>

int main()
{
	std::cout << "Just new line " << std::endl;
	UnitTest::allTests();
	ServerTest::runAllTests();
	//UnitTest::configTestBlock();

	//UnitTest::serverTestBlock();
	//ServerTest::serverTestCase();
	//std::cout << "Test Passed" << std::endl;
}
