R"(

#shader vertex
#version 330 core

layout(location = 0) in vec2 vPos;
// layout(location = 1) in vec4 vColor;

// uniform vec2 uOffset;
// uniform vec2 uPos;
// uniform vec2 uTransform;

void main()
{
	// gl_Position = vec4((vPos.x+uOffset.x*2+uPos.x*2)* uTransform.x, (vPos.y +uOffset.y*2+uPos.y*2)* uTransform.y, 0, 1);
	gl_Position = vec4(vPos.x,vPos.y, 0, 1);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 oColor;

in vec2 fUV;

void main()
{
    oColor = vec4(1,1,1,1);
};
)"