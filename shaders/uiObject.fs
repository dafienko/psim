#version 410

out vec4 color;

in vec2 relPosition;

uniform vec4 backgroundColor;
uniform vec4 borderColor;
uniform vec2 borderRelPosStart;

void main()
{
	if (relPosition.x >= borderRelPosStart.x && relPosition.y >= borderRelPosStart.y && relPosition.x <= 1 - borderRelPosStart.x && relPosition.y <= 1 - borderRelPosStart.y) {
		color = backgroundColor;
	} else {
		color = borderColor;
	}
}