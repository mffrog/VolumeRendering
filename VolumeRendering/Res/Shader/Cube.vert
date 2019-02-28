#version 450
layout(location=0) in vec3 inPosition;
layout(location=1) in vec4 inColor;
layout(location=2) in vec2 texcoord;

layout(location=0) out vec4 outColor;

layout(std140, binding = 0) buffer ViewPorj{
	mat4 view;
	mat4 proj;
	mat4 viewproj;
};

void main(){
	outColor = inColor;
	gl_Position = viewproj * vec4(inPosition, 1.0);
}