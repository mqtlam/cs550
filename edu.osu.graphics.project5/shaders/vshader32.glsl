#version 150

in vec4 vPosition;
in vec4 vColor;
out vec4 color;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform int depthFlag; // 0 = grayscale, 1 = red, 2 = green, 3 = blue channel

void main()
{
	vec4 pos = vPosition;
	vec4 newColor = vColor;

	if (depthFlag == 1) {
		pos.z = vColor.r;
		newColor.gb = vec2(0, 0);
	} else if (depthFlag == 2) {
		pos.z = vColor.g;
		newColor.rb = vec2(0, 0);
	} else if (depthFlag == 3) {
		pos.z = vColor.b;
		newColor.rg = vec2(0, 0);
	} else {
		pos.z = 0.21*vColor.r + 0.71*vColor.g + 0.08*vColor.b;
	}

    gl_Position = Projection * View * Model * pos;
    color = newColor;
}
