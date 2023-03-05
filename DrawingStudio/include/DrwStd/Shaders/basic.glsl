R"(

#shader vertex
#version 330 core

layout(location = 0) in vec4 vPos;

uniform vec2 uOffset;
uniform vec2 uPos;
uniform vec2 uTransform;

out vec2 fUV;

void main()
{
	fUV=vPos.zw;
	gl_Position = vec4((vPos.x+uOffset.x*2+uPos.x*2)* uTransform.x, (vPos.y +uOffset.y*2+uPos.y*2)* uTransform.y, 0, 1);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 oColor;

in vec2 fUV;

uniform sampler2D uTextures;

void main()
{
	oColor = texture(uTextures, fUV);
};
)"