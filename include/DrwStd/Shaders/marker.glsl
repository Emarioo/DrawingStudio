R"(

#shader vertex
#version 330 core

layout(location = 0) in vec4 vPos;

uniform vec2 uPos;
uniform vec2 uWindow;
uniform float uRadius;
uniform float uThickness;

out vec2 fPos;

void main()
{
	// fPos = vec2(2*(vPos.x-0.5)*(uRadius+uThickness)+uPos.x, 2*(vPos.y-0.5)*(uRadius+uThickness)+uPos.y);
    // vec4((vPos.x*(uRadius+uThickness)*2+uPos.x)/uWindow.x*2-1, 1-(vPos.y*(uRadius+uThickness)*2.y+uPos.y)/uWindow.y*2, 0, 1);
	// fPos = vec2((vPos.x*(uRadius+uThickness)*2+uPos.x)/uWindow.x*2-1, 1-(vPos.y*(uRadius+uThickness)*2.y+uPos.y)/uWindow.y*2);
	
    fPos = vec2((vPos.x-0.5)*(uRadius+uThickness)*2+uPos.x, (vPos.y-0.5)*(uRadius+uThickness)*2+uPos.y);
	gl_Position = vec4(fPos.x/uWindow.x*2-1,1-fPos.y/uWindow.y*2, 0, 1);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 oColor;

in vec2 fPos;
uniform vec2 uPos;
uniform float uRadius;
uniform float uThickness;
uniform vec4 uColor;

void main()
{
	float dist = sqrt(pow(uPos.x-fPos.x,2)+pow(uPos.y-fPos.y,2));
	
	if(dist>=uRadius&&dist<=uRadius+uThickness){
		oColor = uColor;
	}else{
		oColor = vec4(1,1,1,0);
	}
};
)"