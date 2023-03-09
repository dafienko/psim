#pragma once

#include <glm/ext.hpp>

#include "renderable.h"
#include "instance.h"

class UIObject;

class UI: public Renderable, public Instance {
	private:

	public:
		static void init();
		static void destroy();

	protected:
		static void renderUIObjectChildren(Instance* parent, glm::vec2 parentPos, glm::vec2 parentSize);

	public: 
		bool rendered = true;

		UI();
		UI(json& data);

		void render() override;

	friend class UIObject; // give UIObject access to renderUIObjectChildren
};