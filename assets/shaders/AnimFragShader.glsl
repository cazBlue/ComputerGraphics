#version 410
in vec4 vNormal;
in vec4 vPosition;
in vec2 vTexCoord;

out vec4 FragColor;

uniform vec3 lightColour;
uniform vec3 directionalLight;
uniform vec3 pointLight;
uniform vec3 CameraPos;
uniform float SpecPow;
uniform sampler2D Diffuse;



void main() 
{

	FragColor = texture(Diffuse,vTexCoord);
}