#version 410
in vec4 Colour;

in vec2 TexCoord;

out vec4 fragColour;

uniform sampler2D diffuse;

void main() {
	fragColour = texture(diffuse,TexCoord);
}