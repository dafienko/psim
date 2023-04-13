#pragma once

#include <glm/ext.hpp>

#include "instance.h"
#include "renderable.h"
#include "ui.h"

struct UICoord {
	//! relative scale of the parent container size
	glm::vec2 scale;
	//! absolute offset in pixels
	glm::vec2 offset;
};

class UIObject: public Instance, Renderable {
	public: 
		/**
		 * @brief initialize all the tools used to render UI objects
		 * 
		 */
		static void init();

		/**
		 * @brief destroy all the tools created in the init function
		 * 
		 */
		static void destroy();

	private:
		glm::vec2 absolutePosition;
		glm::vec2 absoluteSize;

		void updateRenderedDimensions(glm::vec2 parentPos, glm::vec2 parentSize);

	protected:
		/**
		 * @brief Construct a new UIObject object
		 * 
		 * @param type the subtype of this instance
		 * @param name the name to initialize the instance with
		 */
		UIObject(InstanceClass type, const std::string& name);

		/**
		 * @brief Construct a new UIObject object from json data
		 * 
		 * @param type the subtype of this instance
		 * @param name the name to initialize the instance with
		 * @param data the json data to use
		 */
		UIObject(InstanceClass type, const std::string& name, json& data);
		
		/**
		 * @brief render the instance given the parent dimensions
		 * 
		 * @param parentPos the top-left position of the parent container
		 * @param parentSize the container size of the parent
		 */
		void render(glm::vec2 parentPos, glm::vec2 parentSize);

	public: 
		//! should this instance and its descendants be rendered
		bool visible = true;

		//! the background transparency of the UIObject
		float backgroundTransparency = 0;
		//! the background color of the UIObject
		glm::vec3 backgroundColor = glm::vec3(1);
		
		//! the border thickness of the UIObject
		int borderThickness = 0;
		//! the border transparency of the UIObject
		float borderTransparency = 0;
		//! the border color of the UIObject
		glm::vec3 borderColor = glm::vec3(1);
		
		//! The point to anchor the UIObject relative to
		glm::vec2 anchorPoint = glm::vec2(0);

		//! The position of the UIObject
		UICoord position = {glm::vec2(0), glm::vec2(0)};

		//! The size of the UIObject
		UICoord size = {glm::vec2(0), glm::vec2(0)};
		
		void render() override;

		/**
		 * @brief Get the last computed pixel position of the UIObject
		 * 
		 * @return glm::vec2 
		 */
		glm::vec2 getAbsolutePosition();

		/**
		 * @brief Get the last computed pixel size of the UIObject
		 * 
		 * @return glm::vec2 
		 */
		glm::vec2 getAbosluteSize();

	friend class UI;
};