#pragma once

#include <glm/ext.hpp>

#include "renderable.h"
#include "instance.h"

class UIObject;

class UI: public Renderable, public Instance {
	public:
		static void init();
		static void destroy();

	protected:
		/**
		 * @brief render all of an instance's UI children
		 * 
		 * @param parent the parent of the UI objects to render
		 * @param parentPos the top-left position of the parent
		 * @param parentSize the container size of the parent
		 */
		static void renderUIObjectChildren(Instance* parent, glm::vec2 parentPos, glm::vec2 parentSize);

	public: 
		//! should this instance be rendered
		bool rendered = true;

		/**
		 * @brief Construct a new UI object
		 * 
		 */
		UI();

		/**
		 * @brief Construct a new UI object from json data
		 * 
		 * @param data the json data to use
		 */
		explicit UI(json& data);

		void render() override;

	friend class UIObject; // give UIObject access to renderUIObjectChildren
};