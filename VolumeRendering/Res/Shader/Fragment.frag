#version 450
layout(location=0) in vec4 inColor;
layout(location=1) in vec2 inTexcoord;

layout(location=0) uniform sampler2D colorSampler;

out vec4 fragColor;

void main(){
	fragColor = inColor * texture(colorSampler, inTexcoord);
}