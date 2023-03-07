#pragma once

#include <string>
#include <vector>

enum InstanceClass {
	UI,
	UIObject,
	UIFrame,
};

class Instance {
	private:
		std::vector<Instance*> children;
		Instance* parent;

		void addChild(Instance& child);
		void removeChild(Instance& child);

	public:
		const InstanceClass type;
		std::string name;

	public:
		Instance(InstanceClass type, std::string name);

		void setParent(Instance* parent);

		std::vector<Instance*> getChildren() const;
		Instance* findChild(std::string name) const;

		~Instance();
};