#version 410
in vec4 vNormal;

out vec4 FragColour;

uniform vec3 directionalLight;

void main() {
	float d = max(0, dot(normalize(vNormal.xyz), directionalLight));
	FragColour = vec4(d, d, d, 1);
}