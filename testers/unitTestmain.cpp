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
	all();
}
