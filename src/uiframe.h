#pragma once

#include "uiobject.h"

class UIFrame: public UIObject {
	public: 
		/**
		 * @brief Construct a new UIFrame object
		 * 
		 */
		UIFrame();

		/**
		 * @brief Construct a new UIFrame object from json data
		 * 
		 * @param data the json data to construct from
		 */
		explicit UIFrame(json& data);

};