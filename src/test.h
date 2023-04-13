#pragma once

#include <iostream>

namespace testing {
	void onTestPass();
	void onTestFail();
	void printTestReport();
}



// test macros
#define TEST_FAILED(message, file, line) \
std::cout << "❌ TEST FAILED: " << message << std::endl; \
std::cout << "\t" << file << ": line " << line << std::endl; \
testing::onTestFail(); \

#define TEST_PASSED(message) \
std::cout << "✅ TEST PASSED: " << message << std::endl; \
testing::onTestPass(); \

#define TEST_START(message) \
std::cout << "\nTESTING: " << message << std::endl; \



// equality assertions
#define ASSERT_EQUALS_(left, right, message, file, line) \
TEST_START(message) \
if (left == right) { \
	TEST_PASSED(message) \
} else { \
	TEST_FAILED(message, file, line) \
} \

#define ASSERT_EQUALS(left, right, message) ASSERT_EQUALS_(left, right, message, __FILE__, __LINE__)

#define ASSERT_TRUE(expression, message) ASSERT_EQUALS(expression, true, message)
#define ASSERT_FALSE(expression, message) ASSERT_EQUALS(expression, false, message)



// throw assertion
#define ASSERT_THROWS_(code, message, file, line) \
TEST_START(message) \
try { \
	code; \
	TEST_FAILED(message, file, line) \
} catch(...) { \
	TEST_PASSED(message) \
} \

#define ASSERT_THROWS(code, message) ASSERT_THROWS_(code, message, __FILE__, __LINE__)




// individual tests
void testEvents();
void testInstances();

void runTests();