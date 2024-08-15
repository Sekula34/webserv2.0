#ifndef UNITTEST_HPP
#define UNITTEST_HPP
#include <string>

class UnitTest
{
	private :
		//static void _testBlockPassed();
		static void _configFileSyntaxCheck(std::string filePath, bool exception = false);
		static void _configNumberOfServers(std::string filePath, int expectedNumberOfServers);
		static void _testDirGetter(std::string FullPath, const std::string expectedResult);
		static void _baseNameTestCase(std::string url, std::string serverLocation, std::string expected);
		static void _baseNameBlock();
	protected: 
		static void _testpassed(bool block = false);
		static const std::string _constFileFolder;
	public:
		static void urlPathTester(std::string urlSuffix, std::string expectePath);
		static void urlQueryTester(std::string suffixString, std::string expectedQuery);
		static void urlPathQueryBlock();
		static void urlPathTesterBlock();
		static void stringEndCheck();
		static void stringDelimCheck();
		static void allTests();
		static void configSyntaxBlock();
		static void configNumberOfServersBlock();
		static void testDirBlock();
};
#endif
