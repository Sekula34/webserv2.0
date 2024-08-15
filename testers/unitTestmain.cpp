#include "ServerTest.hpp"
#include "UnitTest.hpp"
#include "CHTest.hpp"



void all()
{
	UnitTest::allTests();
	ServerTest::runAllTests();
	CHTest::runAllTests();
}

int main()
{
	CHTest::uriTest();
}
