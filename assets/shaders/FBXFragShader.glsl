#version 410
in vec4 vNormal;
in vec4 vPosition;
out vec4 FragColor;

uniform vec4 lightColour;
uniform vec3 lightDir;
uniform vec3 CameraPos;
uniform float SpecPow;

void main() 
{
	float d = max(0, dot( normalize(vNormal.xyz), lightDir ) );
	
	vec3 E = normalize( CameraPos - vPosition.xyz );
	vec3 R = reflect( -lightDir, vNormal.xyz );
	float s = max( 0, dot( E, R ) );
	
	s = pow( s, SpecPow ); 
	
	FragColor = lightColour * d + lightColour * s;	
}