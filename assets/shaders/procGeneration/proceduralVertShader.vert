#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec2 texcoord;

uniform mat4 view_proj;
uniform sampler2D perlin_texture;

out vec2 frag_texcoord;

void main()
{
	vec4 pos = position;
	pos.y += texture(perlin_texture, texcoord).r * 5;

	frag_texcoord = texcoord;
	gl_Position = view_proj * pos;
}