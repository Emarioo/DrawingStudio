R"(

#shader vertex
#version 330 core

layout(location = 0) in vec2 vPos;
layout(location = 1) in vec4 vColor;

uniform vec2 uPos;
uniform vec2 uSize;
uniform vec2 uWindow;

out vec4 fColor;

void main()
{
	fColor=vColor;
	gl_Position = vec4((vPos.x*uSize.x+uPos.x)/uWindow.x*2-1, 1-(vPos.y*uSize.y+uPos.y)/uWindow.y*2, 0, 1);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 oColor;

in vec4 fColor;

void main()
{
	oColor = fColor;
};
)"