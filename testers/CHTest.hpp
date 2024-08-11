#pragma once 
#include "../srcs/Client/ClientHeader.hpp"
#include "UnitTest.hpp"
#include <string>
#include <utility>
#include <vector>



class CHTest : UnitTest
{
	public:
		static std::vector<std::pair<std::string, int> >messages;


		static void testIfThereIsBody();
		static void runAllTests();
		static void copyTest(const std::string fullMessage);
		static void copyTestBlock();
		static void InitVector();
		static void testCHcase(const std::string fullMessage, const std::pair<std::string, int>& expected);
		static void CHBlockTest();
		static void compare(const ClientHeader& actual,const std::pair<std::string, int>& expected);
		static void simplePrint();
		static std::string generateValidHttpReques();
	private:
		//static const client

};
