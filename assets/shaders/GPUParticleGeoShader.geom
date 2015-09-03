#version 410

// input is points but output a quad
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

// input data from vertex shader
in vec3 position[];
in float lifetime[];
in float lifespan[];
in vec3 velocity[];

// output to fragment shader
out vec4 Colour;
out vec2 TexCoord;

uniform mat4 projectionView;
uniform mat4 cameraTransform;
uniform float sizeStart;
uniform float sizeEnd;
uniform vec4 colourStart;
uniform vec4 colourEnd;

float GetAngle()
{
	vec3 origin = vec3(0,0,0);
	vec3 vertVelocity = normalize(velocity[0]);

	

	return -1;
}

mat4 translate(float x, float y, float z)
{
    return mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(x,   y,   z,   1.0)
    );
}

void main() {
	
	// interpolate colour
	Colour = mix(colourStart, colourEnd, lifetime[0] / lifespan[0] );		

	// calculate the size and create the corners of a quad
	float halfSize = mix(sizeStart,sizeEnd,lifetime[0]/lifespan[0]) * 0.5f;
	
	vec3 corners[4];
	corners[0] = vec3( halfSize, -halfSize, 0 ); //bottom right
	corners[1] = vec3( halfSize, halfSize, 0 ); //top right
	corners[2] = vec3( -halfSize, -halfSize, 0 ); //bottom left
	corners[3] = vec3( -halfSize, halfSize, 0 ); //top left

	//rotate the verts based on velocity
	//build rotation matrix
	vec3 vel = normalize(velocity[0]);
	//float oneMinusDot = 1.0 - dot(L, N);
	//float F = pow(oneMinusDot, 5.0);
	
//	float angle = acos( dot(vel, vec3(0,0,0)));
//	float angle = acos( dot( normalize(y-x), normalize(z-x) ) );

	float angle = 10;

	mat4 rotation = mat4(
			vec4(1.0,         0.0,         0.0, 0.0),
			vec4(0.0,  cos(angle),  sin(angle), 0.0),
			vec4(0.0, -sin(angle),  cos(angle), 0.0),
			vec4(0.0,         0.0,         0.0, 1.0)
		);



	// billboard
	vec3 zAxis = normalize( cameraTransform[3].xyz - position[0] );
	vec3 xAxis = cross( cameraTransform[1].xyz, zAxis );
	vec3 yAxis = cross( zAxis, xAxis );
	mat3 billboard = mat3(xAxis,yAxis,zAxis);
	
	// emit he 4 vertices for the quad
	mat4 transVert;
	mat4 curCorner;
	transVert = mat4(
			vec4(1.0, 0.0, 0.0, 0.0),
			vec4(0.0, 1.0, 0.0, 0.0),
			vec4(0.0, 0.0, 1.0, 0.0),
			vec4(corners[0].x,   corners[0].y,   corners[0].z,   1.0)
		);

	curCorner = rotation * transVert;
//	gl_Position = projectionView * curCorner * vec4(billboard*corners[0]+position[0], 1);
	gl_Position = projectionView * vec4(billboard*corners[0]+position[0], 1);
	TexCoord = vec2( 1, 1); //tr
	EmitVertex();


	transVert = mat4(
			vec4(1.0, 0.0, 0.0, 0.0),
			vec4(0.0, 1.0, 0.0, 0.0),
			vec4(0.0, 0.0, 1.0, 0.0),
			vec4(corners[1].x,   corners[1].y,   corners[1].z,   1.0)
		);

	curCorner = rotation * transVert;	
//	gl_Position = projectionView * curCorner * vec4(billboard*corners[1]+position[0], 1);
	gl_Position = projectionView * vec4(billboard*corners[1]+position[0], 1);
	TexCoord = vec2( 1, 0); //br
	EmitVertex();

	transVert = mat4(
			vec4(1.0, 0.0, 0.0, 0.0),
			vec4(0.0, 1.0, 0.0, 0.0),
			vec4(0.0, 0.0, 1.0, 0.0),
			vec4(corners[2].x,   corners[2].y,   corners[2].z,   1.0)
		);
	
	curCorner = rotation * transVert;
//	gl_Position = projectionView * curCorner * vec4(billboard*corners[2]+position[0], 1);
	gl_Position = projectionView * vec4(billboard*corners[2]+position[0], 1);
	TexCoord = vec2( 0, 1); //tl
	EmitVertex();

	transVert = mat4(
			vec4(1.0, 0.0, 0.0, 0.0),
			vec4(0.0, 1.0, 0.0, 0.0),
			vec4(0.0, 0.0, 1.0, 0.0),
			vec4(corners[3].x,   corners[3].y,   corners[3].z,   1.0)
		);
	
	curCorner = rotation * transVert;	
//	gl_Position = projectionView * curCorner * vec4(billboard*corners[3]+position[0], 1);
	gl_Position = projectionView * vec4(billboard*corners[3]+position[0], 1);
	TexCoord = vec2( 0, 0); //bl
	EmitVertex();
}