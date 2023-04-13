#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "instance.h"
#include "ui.h"
#include "uiframe.h"
#include "uitext.h"

Instance* createInstanceFromJSON(json& data) {
	std::string type = data["type"];

	Instance* instance;
	if (type == "UI") {
		instance = new UI(data);
	} else if (type == "UIFrame") {
		instance = new UIFrame(data);
	} else if (type == "UIText") {
		instance = new UIText(data);
	} else {
		std::cerr << "invalid instance type " << type;
		exit(-1);
	}

	for (const auto& item : data.items())
    {
        std::string key = item.key();
        if (key == "name") {
			instance->name = item.value();
		}
    }

	if (data.contains("children")) {
		for (auto& child : data["children"]) {
			createInstanceFromJSON(child)->setParent(instance);
		}
	}

	return instance;
}

Instance* Instance::fromJSON(const std::string& path) { 
	std::ifstream file(path, std::ifstream::in);
	if (file) {
		json data = json::parse(file);
		return createInstanceFromJSON(data);

	} else {
		std::cerr << "invalid file: " << path << std::endl;
		exit(-1);
	} 
}

Instance::Instance(InstanceClass type, const std::string& name) : 
	childAdded(),
	childRemoved(),
	type(type), 
	name(name)
{}

void Instance::addChild(Instance& child) {
	children.push_back(&child);

	childAdded.fire(*this, child); 
}

void Instance::removeChild(Instance& child) {
	children.erase(std::find(children.begin(), children.end(), &child));

	childRemoved.fire(*this, child);
}

void Instance::setParent(Instance* newParent) {
	if (newParent == dynamic_cast<Instance*>(this)) {
		throw std::invalid_argument("cannot set parent to self");
	}

	if (parent) {
		parent->removeChild(*this);
	}

	parent = newParent;

	if (newParent) {
		newParent->addChild(*this);
	}
}

Instance* Instance::getParent() {
	return parent;
}

std::vector<Instance*> Instance::getChildren() const {
	return std::vector<Instance*>(children);
}

Instance* Instance::findChild(const std::string& name) const {
	auto child = std::find_if(children.begin(), children.end(), [&name](Instance* child) {
		return child->name == name;
	});

	return child == children.end() ? nullptr : *child;
}

Instance* Instance::getChild(const std::string& name) const {
	Instance* child = findChild(name);
	if (!child) {
		std::cerr << "'" << name << "' is not a child of '" << this->name << "'" << std::endl;
		exit(-1);
	}

	return child;
}

// deleting an instance deletes all descendants of that instance
Instance::~Instance() {}