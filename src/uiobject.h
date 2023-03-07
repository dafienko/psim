#pragma once

#include <glm/ext.hpp>

#include "instance.h"

struct UICoord {
	glm::vec2 scale;
	glm::vec2 offset;
};

class UIObject: public Instance {
	private:
		glm::vec2 absolutePosition;
		glm::vec2 absoluteSize;

		void updateRenderedDimensions(glm::vec2 parentPos, glm::vec2 parentSize);

	protected:
		UIObject(InstanceClass type, const std::string name);

	public: 
		bool visible;

		glm::vec2 anchorPoint;
		UICoord position;
		UICoord size;

		virtual void render(glm::vec2 parentPos, glm::vec2 parentSize);
};