#ifndef UNITTEST_HPP
#define UNITTEST_HPP
#include <string>

class UnitTest
{
	private :
		static void _testpassed(bool block = false);
		//static void _testBlockPassed();
		static void _configTestCase(std::string filePath, bool exception = false);
	
	public:
		static void allTests();
		static void configTestBlock();
};
#endif
