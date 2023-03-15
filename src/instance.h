#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "event.h"

using json = nlohmann::json; 

class Instance {
	public:
		enum InstanceClass {
			IC_UI,
			IC_UIObject,
			IC_UIFrame,
			IC_UIText,
		};

		static Instance* fromJSON(const std::string& path);
	private:
		std::vector<Instance*> children;
		Instance* parent = nullptr;

		Event<Instance&, Instance&> childAdded;
		Event<Instance&, Instance&> childRemoved;

		void addChild(Instance& child);
		void removeChild(Instance& child);
		
	public:
		const InstanceClass type;
		std::string name = "";

		void onChildAdded(std::function<void(Instance&, Instance&)> callback);
		void onChildRemoved(std::function<void(Instance&, Instance&)> callback);

	public:
		Instance(InstanceClass type, const std::string& name);

		void setParent(Instance* parent);

		std::vector<Instance*> getChildren() const;
		Instance* findChild(const std::string& name) const;
		Instance* getChild(const std::string& name) const;

		virtual ~Instance();
};