#pragma once

#include <glm/ext.hpp>

#include "instance.h"
#include "renderable.h"
#include "ui.h"

struct UICoord {
	glm::vec2 scale;
	glm::vec2 offset;
};

class UIObject: public Instance, Renderable {
	public: 
		static void init();
		static void destroy();

	private:
		glm::vec2 absolutePosition;
		glm::vec2 absoluteSize;

		void updateRenderedDimensions(glm::vec2 parentPos, glm::vec2 parentSize);

	protected:
		UIObject(InstanceClass type, const std::string name);
		UIObject(InstanceClass type, const std::string name, json& data);

		void render(glm::vec2 parentPos, glm::vec2 parentSize);

	public: 
		bool visible;

		float backgroundTransparency;
		glm::vec3 backgroundColor;
		
		int borderThickness;
		float borderTransparency;
		glm::vec3 borderColor;
		
		glm::vec2 anchorPoint;
		UICoord position;
		UICoord size;
		
		void render() override;

		glm::vec2 getAbsolutePosition();
		glm::vec2 getAbosluteSize();

	friend class UI;
};