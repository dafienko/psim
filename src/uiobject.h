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
		bool visible = true;

		float backgroundTransparency = 0;
		glm::vec3 backgroundColor = glm::vec3(1);
		
		int borderThickness = 0;
		float borderTransparency = 0;
		glm::vec3 borderColor = glm::vec3(1);
		
		glm::vec2 anchorPoint = glm::vec2(0);
		UICoord position = {glm::vec2(0), glm::vec2(0)};
		UICoord size = {glm::vec2(0), glm::vec2(0)};
		
		void render() override;

		glm::vec2 getAbsolutePosition();
		glm::vec2 getAbosluteSize();

	friend class UI;
};