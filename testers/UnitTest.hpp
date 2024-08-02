#ifndef UNITTEST_HPP
#define UNITTEST_HPP
#include <string>

class UnitTest
{
	private :
		//static void _testBlockPassed();
		static void _configFileSyntaxCheck(std::string filePath, bool exception = false);
		static void _configNumberOfServers(std::string filePath, int expectedNumberOfServers);
	
	protected: 
		static void _testpassed(bool block = false);
		static const std::string _constFileFolder;
	public:
		static void allTests();
		static void configSyntaxBlock();
		static void configNumberOfServersBlock();
};
#endif
