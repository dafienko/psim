/*
#include "event.h"

EventListener::EventListener(Event& listeningTo, EventCallback callback) :
	listeningTo(listeningTo),
	callback(callback)
{}

void EventListener::fire(Instance& instance) {
	callback(instance);
}

void EventListener::disconnect() {
	listeningTo.disconnect(*this);
}

Event::Event(Instance& eventOf) : 
	eventOf(eventOf)
{}

void Event::bind(EventCallback callback) {
	listeners.push_back(new EventListener(*this, callback));
}

void Event::disconnect(EventListener& listener) {
	listeners.erase(std::find(listeners.begin(), listeners.end(), &listener));
	delete &listener;
}

void Event::fire() {
	for (EventListener* listener : listeners) {
		listener->fire(eventOf);
	}
 }

Event::~Event() {
	for (EventListener* listener : listeners) {
		delete listener;
	}
}
*/