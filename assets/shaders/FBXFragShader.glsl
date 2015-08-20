#version 410
in vec4 vNormal;
in vec4 vPosition;
in vec2 vTexCoord;

out vec4 FragColor;

uniform vec3 lightColour;
uniform vec3 lightDir;
uniform vec3 CameraPos;
uniform float SpecPow;
uniform sampler2D Diffuse;

void main() 
{
	//light direction
//	float d = max(0, dot( normalize(vNormal.xyz), lightDir ) );

	vec3 E = normalize( CameraPos - vPosition.xyz ); // surface to eye vector
	vec3 R = reflect( -lightDir, vNormal.xyz ); // reflected light vector
	
	//ambient
	vec3 enviro =  vec3(0.25f,0.25f,0.25f); //iA
	vec3 ambientColor = vec3(1,1,1); //iD
	vec4 ambient = texture(Diffuse,vTexCoord) * vec4(enviro, 1) * vec4(ambientColor, 1);

	//diffuse
	float NdL = max( 0.0f, dot( normalize(vNormal.xyz), -normalize(lightDir.xyz) ) ); // Lambert term
	vec4 diffuse = texture(Diffuse,vTexCoord) * vec4(1,1,1,1) * NdL;

	//specular found issues with tut, ran with reference from below
	//specular reference http://ogldev.atspace.co.uk/www/tutorial19/tutorial19.html

	vec4 SpecularColor = vec4(0, 0, 0, 0);
	float gMatSpecularIntensity = 16;

	vec3 VertexToEye = normalize(CameraPos - vPosition.xyz);
	vec3 LightReflect = normalize(reflect(lightDir, vNormal.xyz));
	float SpecularFactor = dot(VertexToEye, LightReflect);
	if (SpecularFactor > 0) {
		SpecularFactor = pow(SpecularFactor, SpecPow);
		SpecularColor = vec4(lightColour * gMatSpecularIntensity * SpecularFactor, 1.0f);		
	}

	FragColor = diffuse + ambient + SpecularColor; //final combined output
//	FragColor = SpecularColor; //spec only
}