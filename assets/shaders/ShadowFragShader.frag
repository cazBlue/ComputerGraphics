#version 410
in vec4 vNormal;
in vec4 vShadowCoord;

out vec4 FragColour;

uniform vec3 lightDir;

uniform sampler2D shadowMap;

//uniform float shadowBias;

void main() {
	
	float d = max(0, dot(normalize(vNormal.xyz), lightDir));

	if (texture(shadowMap, vShadowCoord.xy).r < vShadowCoord.z - 0.01f) 
	{
		d = 0;
	}

//	d = texture(shadowMap, vShadowCoord.xyz, vShadowCoord.z - 0.5f);

	//soft shadows - Basic Algorithm for PCF
	//http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter17.html
	//http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/#PCF
	//shadow sampler setup http://stackoverflow.com/questions/22419682/glsl-sampler2dshadow-and-shadow2d-clarification
	//http://stackoverflow.com/questions/20877938/glsl-sampler2dshadow-deprecated-past-version-120-what-to-use
	//https://www.opengl.org/discussion_boards/showthread.php/166763-GLSL-Shadow-Map-code-sample/page2

//	vec3 color = texture2D(Checker, TexCoord).stp;
//	color *= shadow2DProj(ShadowMap, ProjShadow).r;
//	color *= Diffuse;
//    gl_FragColor = vec4(color, 1);

	FragColour = vec4(d, d, d, 1);
}