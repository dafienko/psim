#pragma once 

#include <functional>
#include <vector>
#include <memory>

template <typename... Args>
class Event {
	public: 
		class EventListener {
			private:
				Event& listeningTo;
				std::function<void(Args...)> callback;

			protected:
				EventListener(Event& listeningTo, std::function<void(Args...)> callback) :
					listeningTo(listeningTo),
					callback(callback)
				{}

				void fire(Args && ...args) {
					callback(std::forward<Args>(args)...);
				}

			public: 
				void disconnect() {
					listeningTo.disconnect(*this);
				}

			friend class Event;
		};
		
	private:
		std::vector<EventListener*> listeners;

	protected: 
		void disconnect(EventListener& listener) {
			listeners.erase(std::find(listeners.begin(), listeners.end(), &listener));
			delete &listener;
		}

	public:
		Event() {}

		void bind(std::function<void(Args...)> callback) {
			listeners.push_back(new EventListener(*this, callback));
		}

		void fire(Args && ...args) {
			for (EventListener* listener : listeners) {
				listener->fire(std::forward<Args>(args)...);
			}
		}

		~Event() {
			for (EventListener* listener : listeners) {
				delete listener;
			}
		}

	friend class EventListener;
};