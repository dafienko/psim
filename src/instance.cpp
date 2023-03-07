#include <algorithm>

#include "instance.h"

Instance::Instance(InstanceClass type, std::string name) : 
	type(type), 
	name(name) 
{}

void Instance::addChild(Instance& child) {
	children.push_back(&child);
}

void Instance::removeChild(Instance& child) {
	children.erase(std::find(children.begin(), children.end(), &child));
}

void Instance::setParent(Instance* newParent) {
	if (parent) {
		parent->removeChild(*this);
	}

	if (newParent) {
		parent = newParent;
		newParent->addChild(*this);
	}
}

std::vector<Instance*> Instance::getChildren() const {
	return std::vector<Instance*>(children);
}

Instance* Instance::findChild(const std::string name) const {
	for (Instance* child : children) {
		if (child->name == name) {
			return child;
		}
	}

	return nullptr;
}

// deleting an instance deletes all descendants of that instance
Instance::~Instance() {
	if (parent) {
		parent->removeChild(*this);

		for (Instance* child : children) {
			delete child;
		}
	}
}