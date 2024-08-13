#include "AutoIndexTest.hpp"
#include <cassert>
#include "../srcs/Response/Autoindex.hpp"
#include "../srcs/Utils/Logger.hpp"

void AutoindexTest::runAll()
{
	simpleTest();
}

void AutoindexTest::simpleTest()
{
	Logger::testCase("Teting simple creation of Auto Index", "no error");
	int statusCode = 0;
	Autoindex index("This/is/Folder/Path", statusCode);
	assert(index.getStatusCode() == statusCode);

	Logger::testCase("Teting simple creation of Auto Index", "ERROR");
	statusCode = 400;
	Autoindex second("This/is/folder/path", statusCode);
	assert(second.getStatusCode() != 0);

	_testpassed();
}
