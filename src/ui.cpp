#include <iostream>

#include "ui.h"
#include "uiobject.h"
#include "core.h"

void UI::init() {
	UIObject::init();
}

void UI::destroy() {
	UIObject::destroy();
}

void UI::renderUIObjectChildren(Instance* parent, glm::vec2 parentPos, glm::vec2 parentSize) {
	for (Instance* child : parent->getChildren()) {
		UIObject* uiObject = dynamic_cast<UIObject*>(child);
		if (uiObject) {
			uiObject->render(parentPos, parentSize);
		}
	}
}

UI::UI() : 
	Instance(InstanceClass::IC_UI, "UI")
{}

UI::UI(json& data) :
	UI()
{}

void UI::render() {
	if (!rendered) { 
		return;
	}

	glm::vec2 windowSize(
		static_cast<float>(Core::getWindowSize().x), 
		static_cast<float>(Core::getWindowSize().y)
	);
	
	UI::renderUIObjectChildren(dynamic_cast<Instance*>(this), glm::vec2(0.0f, 0.0f), windowSize);
}
