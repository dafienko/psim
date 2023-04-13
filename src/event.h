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
				/**
				 * @brief Construct a new Event Listener object
				 * 
				 * @param listeningTo the event this listener is listening to
				 * @param callback the callback to run on the event's trigger
				 */
				EventListener(Event& listeningTo, std::function<void(Args...)> callback) :
					listeningTo(listeningTo),
					callback(callback)
				{}

				/**
				 * @brief execute this event listener's callback
				 * 
				 * @param args the args to be passed to the callback
				 */
				void fire(Args && ...args) {
					callback(std::forward<Args>(args)...);
				}

			public: 
				/**
				 * @brief Stop listening to the bound event
				 * 
				 */
				void disconnect() {
					listeningTo.disconnect(*this);
				}

			friend class Event;
		};
		
	private:
		std::vector<EventListener*> listeners;

	protected: 
		/**
		 * @brief disconnect a bound event listener from this event
		 * 
		 * @param listener 
		 */
		void disconnect(EventListener& listener) {
			listeners.erase(std::find(listeners.begin(), listeners.end(), &listener));
			delete &listener;
		}

	public:
		Event() {}

		/**
		 * @brief Bind a callback to this event
		 * 
		 * @param callback 
		 * The callback to be run when the event is fired
		 */
		void bind(std::function<void(Args...)> callback) {
			listeners.push_back(new EventListener(*this, callback));
		}

		/**
		 * @brief Fire all bound event callbacks
		 * 
		 * @param args 
		 * The arguments forwarded to each callback function 
		 */
		void fire(Args & ...args) {
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