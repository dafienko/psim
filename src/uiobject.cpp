#include <GL/glew.h>
#include <glm/ext.hpp>

#include "uiobject.h"
#include "shader.h"
#include "core.h"
#include "quad.h"

std::unique_ptr<ShaderProgram> uiShader;

void UIObject::init() {
	uiShader = std::make_unique<ShaderProgram>("shaders/uiObject.vsh", "shaders/uiObject.fs");
}

void UIObject::destroy() {
	uiShader.release();
}

UIObject::UIObject(InstanceClass type, const std::string name) : 
	Instance(type, name),
	visible(true),
	backgroundColor(glm::vec3(1, 1, 1))
{}

UIObject::UIObject(InstanceClass type, const std::string name, json& data) : 
	UIObject(type, name)
{
	for (const auto& item : data.items())
    {
        std::string key = item.key();
		const auto& value = item.value();

        if (key == "backgroundColor") {
			backgroundColor = glm::vec3(value[0], value[1], value[2]);	
		} else if (key == "backgroundTransparency") {
			backgroundTransparency = value;
		} else if (key == "size") {
			if (value.contains("scale")) {
				size.scale = glm::vec2(value["scale"][0], value["scale"][1]);
			} 
			if (value.contains("offset")) {
				size.offset = glm::vec2(value["offset"][0], value["offset"][1]);
			}
		} else if (key == "position") {
			if (value.contains("scale")) {
				position.scale = glm::vec2(value["scale"][0], value["scale"][1]);
			} 
			if (value.contains("offset")) {
				position.offset = glm::vec2(value["offset"][0], value["offset"][1]);
			}
		} else if (key == "anchorPoint") {
			anchorPoint = glm::vec2(value[0], value[1]);
		}
    }
}

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
	
	updateRenderedDimensions(parentPos, parentSize);

	render();

	UI::renderUIObjectChildren(dynamic_cast<Instance*>(this), absolutePosition, absoluteSize);
}

void UIObject::render() {
	if (backgroundTransparency >= 1) {
		return;
	}

	uiShader->bind();

	glm::mat4x4 ortho = glm::ortho(0.0f, (float)Core::screenWidth, (float)Core::screenHeight, 0.0f);
	GLint projLoc = glGetUniformLocation(uiShader->getProgram(), "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &ortho[0][0]);

	glm::vec4 colorWithAlpha = glm::vec4(
		backgroundColor.x, backgroundColor.y, backgroundColor.z, 
		1 - backgroundTransparency
	);
	glUniform4fv(glGetUniformLocation(uiShader->getProgram(), "backgroundColor"), 1, &colorWithAlpha[0]);

	Quad::render(
		absolutePosition,
		absolutePosition + absoluteSize
	);
}