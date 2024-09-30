#pragma once 
#include "../srcs_new/Message/RequestHeader.hpp"
#include "UnitTest.hpp"
#include <string>
#include <utility>
#include <vector>



class RequestHeaderTester : UnitTest
{
	public:
		static std::vector<std::pair<std::string, int> >messages;

		static void fullUrlTest();
		static void uriTest();
		static void testIfThereIsBody();
		static void runAllTests();
		static void copyTest(const std::string fullMessage);
		static void copyTestBlock();
		static void InitVector();
		static void testCHcase(const std::string fullMessage, const std::pair<std::string, int>& expected);
		static void CHBlockTest();
		static void compare(const RequestHeader& actual,const std::pair<std::string, int>& expected);
		static void simplePrint();
		static std::string generateValidHttpReques();
		static void cgiResponseHeader();
	private:
		//static const client

};
