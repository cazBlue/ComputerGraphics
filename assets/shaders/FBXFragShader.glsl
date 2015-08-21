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
	//light direction
//	float d = max(0, dot( normalize(vNormal.xyz), directionalLight ) );

	vec3 E = normalize( CameraPos - vPosition.xyz ); // surface to eye vector
	vec3 R = reflect( -directionalLight, vNormal.xyz ); // reflected light vector
	
	//ambient
	vec3 enviro =  vec3(0.25f,0.25f,0.25f); //iA - ambient intensity
	vec3 ambientColor = vec3(1,1,1); //iD - ambient colour
	vec4 ambient = texture(Diffuse,vTexCoord) * vec4(enviro, 1) * vec4(ambientColor, 1);

	//diffuse
	float NdL = max( 0.0f, dot( normalize(vNormal.xyz), -normalize(directionalLight.xyz) ) ); // Lambert term
	vec4 diffuse = texture(Diffuse,vTexCoord) * vec4(1,1,1,1) * NdL;

	//specular found issues with tut, ran with reference from below
	//specular reference http://ogldev.atspace.co.uk/www/tutorial19/tutorial19.html

	vec4 SpecularColor = vec4(0, 0, 0, 0);
	float gMatSpecularIntensity = 16;

	vec3 VertexToEye = normalize(CameraPos - vPosition.xyz);
	vec3 LightReflect = normalize(reflect(directionalLight, vNormal.xyz));
	float SpecularFactor = dot(VertexToEye, LightReflect);
	if (SpecularFactor > 0) {
		SpecularFactor = pow(SpecularFactor, SpecPow);
		SpecularColor = vec4(lightColour * gMatSpecularIntensity * SpecularFactor, 1.0f);		
	}

	//point light hard coded TODO make dynamic...
	//http://ogldev.atspace.co.uk/www/tutorial20/tutorial20.html

	vec3 directionalLightection = vPosition.xyz - pointLight; //hard coded light position of light
    float Distance = length(directionalLightection); //distance from light to vert
    directionalLightection = normalize(directionalLightection); //..

    vec4 Color = diffuse + ambient + SpecularColor;
    float Attenuation = 1 + //constant factor
                        .25 * Distance + //linear factor
                        .25 * Distance * Distance; //exponitial factor


	vec4 pointLight = Color / Attenuation;

//	FragColor = pointLight;

	FragColor = diffuse + ambient + SpecularColor + pointLight; //final combined output
//	FragColor = SpecularColor; //spec only
}