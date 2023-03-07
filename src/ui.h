#pragma once

#include "renderable.h"
#include "uiobject.h"
#include "instance.h"

class UI: public Renderable, Instance {
	private:


	public: 
		UI();

	public:
		bool rendered;

		void render() override;
};