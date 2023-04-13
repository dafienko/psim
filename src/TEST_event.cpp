#include <string>

#include "test.h"
#include "event.h"
void testEvents() {
	int eventRuns = 0;
	Event<> e;

	e.bind([&] () {
		eventRuns++;
	});

	ASSERT_EQUALS(eventRuns, 0, "New event hasn't fired")
	
	for (int i = 1; i < 3; i++) {
		e.fire();

		ASSERT_EQUALS(eventRuns, i, "New event fired " + std::to_string(i) + " time(s)")
	}

	Event<int, bool, std::string> eventWithArgs;
	int first = 124;
	bool second = false;
	std::string third("hello");

	bool ran = false;
	eventWithArgs.bind([=, &ran] (int a, bool b, std::string c) {
		ran = true;
		ASSERT_EQUALS(first, a, "Event with args first argument correct")
		ASSERT_EQUALS(second, b, "Event with args second argument correct")
		ASSERT_EQUALS(third, c, "Event with args third argument correct")
	});

	eventWithArgs.fire(first, second, third);

	ASSERT_TRUE(ran, "Event with args ran")
}