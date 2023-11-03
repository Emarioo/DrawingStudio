R"(

#shader vertex
#version 330 core

layout(location = 0) in vec4 vPos;

uniform vec2 uPos;
uniform vec2 uSize;
uniform vec2 uWindow;

out float hue;
out float saturation;
out float value;
out float alpha;
out vec2 fPos;

void main()
{
	hue=vPos.x*6;
	saturation = 1-vPos.x;
	value = 1-vPos.y;
	alpha = vPos.x;

	fPos=vPos.xy;

	gl_Position = vec4((vPos.x*uSize.x+uPos.x)/uWindow.x*2-1, 1-(vPos.y*uSize.y+uPos.y)/uWindow.y*2, 0, 1);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 oColor;

in float hue;
in float saturation;
in float value;
in float alpha;

in vec2 fPos;

uniform vec3 uColor;

uniform int uShaderType;

uniform vec2 uSize;

void main()
{
	if(uShaderType==0){
		oColor = vec4((uColor.xyz*saturation+vec3(1,1,1)*(1-saturation))*value,1);
	}else if(uShaderType==1){
		float red=0;
		float green=0;
		float blue=0;
		if (hue<=1) {
			red = 1;
			green = hue;
			blue = 0;
		}
		else if(hue<=2) {
			red = (2-hue);
			green = 1;
			blue = 0;
		}
		else if (hue <= 3) {
			red = 0;
			green = 1;
			blue = (hue-2);
		}
		else if (hue <= 4) {
			red = 0;
			green = (4-hue);
			blue = 1;
		}
		else if (hue <= 5) {
			red = (hue-4);
			green = 0;
			blue = 1;
		}
		else if (hue <= 6) {
			red = 1;
			green = 0;
			blue = (6-hue);
		}
		oColor = vec4(red,green,blue,1);
	}else if(uShaderType==2){
		float tile = 0.7+mod(int(mod(fPos.x*uSize.x/6,2))+int(mod(fPos.y*uSize.y/6,2)),2)*(1-0.7);
		oColor = vec4(tile*(1-alpha)+uColor.r*alpha,tile*(1-alpha)+uColor.g*alpha,tile*(1-alpha)+uColor.b*alpha,1);
	}
};
)"