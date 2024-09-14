#include "DirectiveTester.hpp"
#include <cassert>
#include <vector>
#include "iostream"
#include "../srcs/Utils/Logger.hpp"
const std::vector<Directive> initDuplicatesVec()
{
	std::vector<Directive> dirVector;
	Directive oneDir("autoindex", "on");
	Directive secondDir("autoindex", "off");
	Directive thirdDir("autoinde1x", "on");
	dirVector.push_back(oneDir);
	dirVector.push_back(secondDir);
	dirVector.push_back(thirdDir);
	return dirVector;
}

const std::vector<Directive> initNonDuplicatesVec()
{
	std::vector<Directive> dirVector;
	Directive oneDir("autoindex", "on");
	Directive secondDir("root", "off");
	Directive thirdDir("listen", "on");
	dirVector.push_back(oneDir);
	dirVector.push_back(secondDir);
	dirVector.push_back(thirdDir);
	return dirVector;
}


const std::vector<Directive> DirectiveTester::duplicates = initDuplicatesVec();
const std::vector<Directive> DirectiveTester::nonDuplicates = initNonDuplicatesVec();

void DirectiveTester::runAll()
{
	testDuplicate();
}

void DirectiveTester::testDuplicate()
{
	Logger::testCase("Testing duplicate directive");
	const Directive* ptr;
	bool result = Directive::isDuplicateDirectivePresent(duplicates, ptr);
	if (result)
	{
		std::cout << "ptr is present here" << std::endl;
		std::cout << ptr->_directiveName << std::endl;
	}
	else
		std::cout << "No duplicates here " << std::endl;
	assert(result == true);
	assert(ptr != NULL);

	result = Directive::isDuplicateDirectivePresent(nonDuplicates, ptr);
	if (result)
	{
		std::cout << "ptr is present here" << std::endl;
		std::cout << ptr->_directiveName;
	}
	else
		std::cout << "No duplicates here " << std::endl;
	assert(result == false);
	assert(ptr == NULL);
	_testpassed();
}
