#pragma once

class Renderable {
	public:
		virtual void render() = 0;

		virtual ~Renderable() = 0;
};
