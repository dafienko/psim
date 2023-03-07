#include "uiobject.h"

UIObject::UIObject(InstanceClass type, const std::string name) : 
	Instance(type, name)
{}

void UIObject::updateRenderedDimensions(glm::vec2 parentPos, glm::vec2 parentSize) {
	absoluteSize = glm::vec2(
		parentSize.x * size.scale.x + size.offset.x, 
		parentSize.y * size.scale.y + size.offset.y
	);

	glm::vec2 anchorPos = glm::vec2(
		parentPos.x + parentSize.x * position.scale.x + position.offset.x,
		parentPos.y + parentSize.y * position.scale.y + position.offset.y
	);

	absolutePosition = anchorPos - absoluteSize * anchorPoint;
}

void UIObject::render(glm::vec2 parentPos, glm::vec2 parentSize) {
	if (!visible) {
		return;
	}

	
}