R"(
#shader vertex
#version 430 core

layout(std430, binding = 0) restrict buffer SSBO
{
	float data[];
} buf;
const int bufStride = 2+4+1;

out vec4 fColor;

uniform mat4 uProj;

uniform vec2 uOffset;
uniform vec2 uTransform;
uniform float uZoom;

void main() {
	// since structs can't have vec3 this is the way I have to do it.
    
    float x = 2*(buf.data[bufStride*gl_VertexID + 0]+uOffset.x) * uTransform.x;
    float y = 2*(buf.data[bufStride*gl_VertexID + 1]+uOffset.y) * uTransform.y;
    
    fColor = vec4(buf.data[bufStride*gl_VertexID+2],
        buf.data[bufStride*gl_VertexID+3],
        buf.data[bufStride*gl_VertexID+4],
        buf.data[bufStride*gl_VertexID+5]);
    
    gl_PointSize = buf.data[bufStride*gl_VertexID+6]*uZoom;
    
    // fColor = vec4(1,1,1,1);

	gl_Position = uProj * vec4(x,y,-1,1);
};

#shader fragment
#version 430 core

layout(location = 0) out vec4 oColor;

in vec4 fColor;

void main()
{
	oColor = fColor;
};
)"