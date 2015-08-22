#version 410

layout(location=0) in vec4 Position;
layout(location=1) in vec4 Normal;
layout(location=2) in vec2 TexCoord;
layout(location=3) in vec4 Tangents;
layout(location=4) in vec4 Weights;
layout(location=5) in vec4 Indices;

out vec4 vNormal;
out vec4 vPosition;
out vec2 vTexCoord;

uniform mat4 ProjectionView;
uniform mat4 Global;

const int MAX_BONES = 128;
uniform mat4 bones[MAX_BONES];

void main() 
{ 	 
	// cast the indices to integer's so they can index an array
	ivec4 index = ivec4(Indices);
	
	// sample bones and blend up to 4
	vec4 P = bones[index.x] * Position * Weights.x;
	P += bones[index.y] * Position * Weights.y;
	P += bones[index.z] * Position * Weights.z;
	P += bones[index.w] * Position * Weights.w;

	vNormal = Normal;
	vPosition = Global * P;
	vTexCoord = TexCoord;
	gl_Position = ProjectionView * Global * P;
}