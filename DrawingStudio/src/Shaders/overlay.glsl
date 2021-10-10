R"(

#shader vertex
#version 330 core

layout(location = 0) in vec4 vPos;

uniform vec2 uPos;
uniform vec2 uSize;
uniform vec2 uWindow;

out vec2 fUV;

void main()
{
	fUV=vPos.zw;
	gl_Position = vec4((vPos.x*uSize.x+uPos.x)/uWindow.x*2-1, 1-(vPos.y*uSize.y+uPos.y)/uWindow.y*2, 0, 1);

	//gl_Position = vec4(vPos.x, vPos.y, 0, 1);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 oColor;

in vec2 fUV;

uniform sampler2D uTextures;
uniform vec4 uColor;
uniform int uTextured;

void main()
{
	if(uTextured==1){
		oColor = uColor*texture(uTextures, fUV);
	}else{
		oColor = uColor;
	}
};
)"