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
    //fbx scale/units doesn't seem to work, force a down scale, hacky work around...
    float x = Position.x * .01;
    float y = Position.y * .01;
    float z = Position.z * .01;
    
    vec4 newPos = vec4(x,y,z,Position.w);
    
	vPosition = newPos;
	vTexCoord = TexCoord;
	gl_Position = ProjectionView*newPos; 
}