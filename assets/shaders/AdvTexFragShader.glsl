#version 410
in vec3 vNormal;
in vec3 vTangent;
in vec3 vBiTangent;
in vec4 vPosition;
in vec2 vTexCoord;

out vec4 FragColor;

uniform vec3 lightColour;
uniform vec3 directionalLight;
uniform vec3 pointLight;
uniform vec3 CameraPos;
uniform float SpecPow;

uniform sampler2D Diffuse;
uniform sampler2D NormalTex;


void main() 
{
	//calculate normals	
	vec3 Normal = normalize(vNormal);
    vec3 Tangent = normalize(vTangent);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec3 BumpMapNormal = texture(NormalTex, vTexCoord).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, vBiTangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);

	float d = max( 0, dot( NewNormal, normalize( directionalLight ))); 

	vec3 E = normalize( CameraPos - vPosition.xyz ); // surface to eye vector
	vec3 R = reflect( -directionalLight, vNormal.xyz ); // reflected light vector
	
	//ambient
	vec3 enviro =  vec3(0.25f,0.25f,0.25f); //iA - ambient intensity
	vec3 ambientColor = vec3(1,1,1); //iD - ambient colour
	vec4 ambient = texture(Diffuse,vTexCoord) * vec4(enviro, 1) * vec4(ambientColor, 1);

	//diffuse
	vec4 diffuse = texture(Diffuse, vTexCoord) * vec4(d,d,d,1);	//light info comes from normals now

	//specular found issues with tut, ran with reference from below
	//specular reference http://ogldev.atspace.co.uk/www/tutorial19/tutorial19.html

	vec4 SpecularColor = vec4(0, 0, 0, 0);
	float gMatSpecularIntensity = 8;

	vec3 VertexToEye = normalize(CameraPos - vPosition.xyz);
	vec3 LightReflect = normalize(reflect(directionalLight, NewNormal));
	float SpecularFactor = dot(VertexToEye, LightReflect);
	if (SpecularFactor > 0) {
		SpecularFactor = pow(SpecularFactor, SpecPow);
		SpecularColor = vec4(lightColour * gMatSpecularIntensity * SpecularFactor, 1.0f);
		//SpecularColor = SpecularColor * vec4(d,d,d,1);
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

//	FragColor = pointLight; //point light only
//	FragColor = SpecularColor; //spec only
//	FragColor = ambient; //ambient onlt

	FragColor = diffuse + ambient + SpecularColor; //final result
}