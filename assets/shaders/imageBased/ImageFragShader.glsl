#version 410
in vec3 vNormal;
in vec4 vPosition;

out vec4 FragColor;

uniform vec3 directionalLight;
uniform vec3 CameraPos;

uniform sampler2D WarpTexture;


void main() 
{
	float d = max(0.0, dot(vNormal, -directionalLight));
	vec3 color = texture(WarpTexture, vec2(d,0)).rgb;
	FragColor = vec4(color, 1);
}