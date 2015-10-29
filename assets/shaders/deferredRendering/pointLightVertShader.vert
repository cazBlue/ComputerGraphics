#version 410
layout(location = 0) in vec4 Position;

// position in world-space
uniform vec3 lightPosition;
uniform float lightRadius;
uniform mat4 ProjectionView;

void main() {

gl_Position = ProjectionView * vec4(Position.xyz * lightRadius + lightPosition, 1);

}