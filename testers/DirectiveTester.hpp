#ifndef DIRECTIVETESTER_HPP
# define DIRECTIVETESTER_HPP
# include "UnitTest.hpp"
#include <vector>
#include "../srcs/Parsing/Directive.hpp"


class DirectiveTester : public UnitTest
{
	public :
		static void runAll();
		static void testDuplicate();

	private : 

		const static std::vector<Directive> duplicates;
		const static std::vector<Directive> nonDuplicates;
};

#endif
