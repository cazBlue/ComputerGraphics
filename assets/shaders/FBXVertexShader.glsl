#version 410

layout(location=0) in vec4 Position;
layout(location=1) in vec4 Normal;
layout(location=2) in vec2 TexCoord;

out vec4 vNormal;
out vec4 vPosition;
out vec2 vTexCoord;

uniform mat4 ProjectionView;

void main() 
{ 
	vNormal = Normal;
	vPosition = Position;
	vTexCoord = TexCoord;
	gl_Position = ProjectionView*Position; 
}