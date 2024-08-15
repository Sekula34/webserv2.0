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
	//all();
	//UnitTest::stringDelimCheck();
	UnitTest::urlPathTesterBlock();
	UnitTest::urlPathQueryBlock();
	
	//CHTest::uriTest();
}
