#include "ServerTest.hpp"
#include "UnitTest.hpp"
#include "CHTest.hpp"
#include "AutoIndexTest.hpp"



void all()
{
	UnitTest::allTests();
	ServerTest::runAllTests();
	CHTest::runAllTests();
}

int main()
{
	AutoindexTest::runAll();
}
