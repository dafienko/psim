#include "test.h"

int testsPassed = 0;
int totalTests = 0;

void testing::onTestPass() {
	testsPassed++;
	totalTests++;
}

void testing::onTestFail() {
	totalTests++;
}

void testing::printTestReport() {
	std::cout << testsPassed << " / " << totalTests << " tests passed (" << totalTests - testsPassed << " test(s) failed)" << std::endl;
}

void runTests() {
	std::cout << "TESTING EVENTS..." << std::endl;
	testEvents();
	std::cout << std::endl << std::endl;

	std::cout << "TESTING INSTANCES..." << std::endl;
	testInstances();
	std::cout << std::endl << std::endl;

	testing::printTestReport();
}