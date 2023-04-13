#pragma once

class Renderable {
	public:
		/**
		 * @brief Render self for the current frame
		 * 
		 */
		virtual void render() = 0;

		virtual ~Renderable() = 0;
};
