#version 410
in vec4 vNormal;
in vec4 vShadowCoord;

out vec4 FragColour;

uniform vec3 lightDir;

uniform sampler2D shadowMap;

//uniform float shadowBias;

void main() {
	
	float d = max(0, dot(normalize(vNormal.xyz), lightDir)); //get the base lighting value

	vec2 poissonDisk[4] = vec2[](
	  vec2( -1.94201624, -1.39906216 ),
	  vec2( 1.94558609, -1.76890725 ),
	  vec2( -1.094184101, -1.92938870 ),
	  vec2( 1.34495938, 1.29387760 )
	);

	for (int i = 0; i < 4; i++)
	{
		if (texture(shadowMap, vShadowCoord.xy + + poissonDisk[i]/700.0).r < vShadowCoord.z - 0.01f) 
		{
			d -= 0.2;
		}		
	}

	//soft shadows - Basic Algorithm for PCF using Poisson Sampling
	//ended up using Poisson Sampling (Non Stratified)
	////http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping


	//http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter17.html	
	//shadow sampler setup http://stackoverflow.com/questions/22419682/glsl-sampler2dshadow-and-shadow2d-clarification
	//http://stackoverflow.com/questions/20877938/glsl-sampler2dshadow-deprecated-past-version-120-what-to-use
	//https://www.opengl.org/discussion_boards/showthread.php/166763-GLSL-Shadow-Map-code-sample/page2


	FragColour = vec4(d, d, d, 1);
}