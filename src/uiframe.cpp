#include <memory>
#include <iostream>

#include "uiframe.h"

UIFrame::UIFrame() :
	UIObject(InstanceClass::IC_UIFrame, "UIFrame")
{}

UIFrame::UIFrame(json& data) : 
	UIObject(InstanceClass::IC_UIFrame, "UIFrame", data)
{}