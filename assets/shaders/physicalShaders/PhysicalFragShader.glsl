#version 410
in vec3 vNormal;
in vec3 vTangent;
in vec3 vBiTangent;
in vec4 vPosition;
in vec2 vTexCoord;

out vec4 FragColor;

uniform vec3 directionalLight;
uniform vec3 pointLight;
uniform vec3 CameraPos;

uniform sampler2D Diffuse;
uniform sampler2D NormalTex;
uniform sampler2D SpecTex;



//float OrenNayar(vec3 normal, )
//{
//
//}


void main() 
{
	//Note - no spec on point light currently!


	//calculate normals	from normal map
	vec3 Normal = normalize(vNormal);
	vec3 Tangent = normalize(vTangent);
	Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
	vec3 Bitangent = cross(Tangent, Normal);
	vec3 BumpMapNormal = texture(NormalTex, vTexCoord).xyz;
	BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
	vec3 normalMap;
	mat3 TBN = mat3(Tangent, vBiTangent, Normal);
	normalMap = TBN * BumpMapNormal;
	normalMap = normalize(normalMap);

	vec3 L = normalize(directionalLight);
	vec3 N = normalMap;

	float NdL = max(0.0f, dot(N, L));
	vec3 E = normalize(CameraPos - vPosition.xyz); // surface to eye vector
	float NdE = max(0.0f, dot(N, E));

	float roughness = 0.1f;

	float R2 = roughness * roughness;

	// Oren-Nayar Diffuse Term
	float A = 1.0f - 0.5f * R2 / (R2 + 0.33f);
	float B = 0.45f * R2 / (R2 + 0.09f);

	// CX = Max(0, cos(l,e))
	vec3 lightProjected = normalize(L - N * NdL);
	vec3 viewProjected = normalize(E - N * NdE);
	float CX = max(0.0f, dot(lightProjected, viewProjected));
	
	// DX = sin(alpha) * tan(beta)
	float alpha = sin(max(acos(NdE), acos(NdL)));
	float beta = tan(min(acos(NdE), acos(NdL)));
	float DX = alpha * beta;

	// Calculate Oren-Nayar, replaces the Phong Lambertian Term
	float OrenNayar = NdL * (A + B * CX * DX);	

	//Cook-Torrance Specular Reflectance
	vec3 H = normalize(E + L);
	float NdH = max(0.0f, dot(N, H));
	float NdH2 = NdH * NdH;
	float e = 2.71828182845904523536028747135f;
	float pi = 3.1415926535897932384626433832f;
	
	// Beckman's Distribution Function D
	float exponent = -(1 - NdH2) / (NdH2 * R2);
	float D = pow(e, exponent) / (R2 * NdH2 * NdH2);
	
	// Fresnel Term F
	float FresnelScale = 1;
	float HdE = dot(H, E);
	float F = mix(pow(1 - HdE, 5), 1, FresnelScale);
	
	// Geometric Attenuation Factor G
	float X = 2.0f * NdH / dot(E, H);
	float G = min(1, min(X * NdL, X * NdE));
	
	// Calculate Cook-Torrance
	float CookTorrance = max((D*G*F) / (NdE * pi), 0.0f);

//	vec3 E = normalize( CameraPos - vPosition.xyz ); // surface to eye vector
	vec3 R = reflect( -directionalLight, vNormal.xyz ); // reflected light vector
	
	//ambient
	vec3 enviro =  vec3(0.25f,0.25f,0.25f); //iA - ambient intensity
	vec3 ambientColor = vec3(1,1,1); //iD - ambient colour
	vec4 ambient = texture(Diffuse,vTexCoord) * vec4(enviro, 1) * vec4(ambientColor, 1);

	//diffuse + light
	vec4 diffuse = texture(Diffuse, vTexCoord) * vec4(OrenNayar, OrenNayar, OrenNayar, 1);	//light info comes	
	
	//add point light
	vec3 vertToPoint = vPosition.xyz - pointLight; //hard coded light position of light
	float Distance = length(vertToPoint); //distance from light to vert	

	vec4 Color = diffuse + texture(SpecTex, vTexCoord);
	float Attenuation = 1 + //constant factor
		.25 * Distance + //linear factor
		.25 * Distance * Distance; //exponitial factor

	vec4 pointLight = Color / Attenuation;

	//spec
	vec4 SpecularColor = texture(SpecTex, vTexCoord) * (texture(Diffuse, vTexCoord) + CookTorrance);

	FragColor = diffuse + ambient + SpecularColor + pointLight; //final result

	//FragColor = texture(Diffuse, vTexCoord);
}