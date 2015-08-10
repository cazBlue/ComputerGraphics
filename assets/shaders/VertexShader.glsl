#version 410

layout(location=0) in vec4 Position;
layout(location=1) in vec4 Colour;

out vec4 vColour;

uniform mat4 ProjectionView;
uniform float Time;
uniform float HeightScale;

void main() 
{
    vColour = Colour; 
	vec4 P = Position;
	P.y += sin( Time + Position.x ) * HeightScale;	
    gl_Position = ProjectionView * P;     
}