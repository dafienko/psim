#include <glm/ext.hpp>

#include "uiobject.h"
#include "font.h"

class UIText: public UIObject {
	public: 
		UIText();
		UIText(json& data);

		std::string text;
		glm::vec3 textColor;
		float textTransparency;
		FontFace font;
		unsigned int textSize;
		glm::vec2 textAnchorPoint;
		glm::vec2 textPadding;

		void render() override;
};