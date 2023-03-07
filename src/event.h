#include <functional>
#include <vector>

class EventListener {
	private:
		bool connected;
		const Event* listeningTo;

	protected:
		EventListener(Event& listeningTo);

	public: 
		void disconnect();
	
	friend class Event;
};

class Event {
	private:
		std::vector<EventListener> listeners;

	public:
		Event();

		void bind(std::function<void()> callback);
};