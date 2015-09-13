#version 410

in vec2 fTexCoord;

out vec4 FragColour;

uniform sampler2D target;

// just sample the target and return the colour
vec4 Simple() {
	return texture(target, fTexCoord);	
}


// simple box blur
vec4 BoxBlur() {

	vec2 texel = 1.0f / textureSize(target, 0).xy;

	// 9-tap box kernel
	vec4 colour = texture(target, fTexCoord);
	colour += texture(target, fTexCoord + vec2(-texel.x, texel.y));
	colour += texture(target, fTexCoord + vec2(-texel.x, 0));
	colour += texture(target, fTexCoord + vec2(-texel.x, -texel.y));
	colour += texture(target, fTexCoord + vec2(0, texel.y));
	colour += texture(target, fTexCoord + vec2(0, -texel.y));
	colour += texture(target, fTexCoord + vec2(texel.x, texel.y));
	colour += texture(target, fTexCoord + vec2(texel.x, 0));
	colour += texture(target, fTexCoord + vec2(texel.x, -texel.y));

	return colour / 9;
}

vec4 Sobel()
{
	//formula texel = square root of x^2 + y^2

	vec2 texelSize = 1.0f / textureSize( target, 0 ).xy;

	vec4 colourx;
	vec4 coloury;
	vec4 finalColour;	

	

	// calculate x kernal
	colourx += texture( target, fTexCoord - vec2( texelSize.x, 0 ) + vec2(0, texelSize.y) )  * -1; //top left
	colourx += texture( target, fTexCoord - vec2( texelSize.x, 0 ) )  * -2; //middle left
	colourx += texture( target, fTexCoord - vec2( texelSize.x, texelSize.y )) * -1; //bottom left

	colourx += texture( target, fTexCoord + vec2( texelSize.x, texelSize.y ) )  * 1; //top right
	colourx += texture( target, fTexCoord + vec2( texelSize.x, 0 ) ) * 2; //middle right
	colourx += texture( target, fTexCoord + vec2( texelSize.x, 0 ) - vec2(0,texelSize.y ) ) * 1; //bottom right

	//calculate y kernal
	coloury += texture( target, fTexCoord - vec2( texelSize.x, 0 ) + vec2( 0, texelSize.y) ) * 1; //top left
	coloury += texture( target, fTexCoord + vec2( 0, texelSize.y) )  * 2; //middle top
	coloury += texture( target, fTexCoord + vec2( texelSize.x, texelSize.y ) ) * 1; //top right

	coloury += texture( target, fTexCoord - vec2( texelSize.x, texelSize.y) ) * -1; //bottom left
	coloury += texture( target, fTexCoord - vec2( 0, texelSize.y ) ) * -2; //middle bottom
	coloury += texture( target, fTexCoord + vec2( texelSize.x, 0 ) - vec2(0,texelSize.y) ) * -1; //bottom right

	finalColour = sqrt((colourx * colourx) + (coloury  * coloury));


	return finalColour;
}

vec4 Sharpen()
{	
	vec2 texelSize = 1.0f / textureSize( target, 0 ).xy;

	vec4 colour;

	colour = texture( target, fTexCoord ) * (11 / 3);
	colour += texture( target, fTexCoord + vec2( 0, texelSize.y ) ) * (-2 / 3);
	colour += texture( target, fTexCoord - vec2( 0, texelSize.y ) ) * (-2 / 3);
	colour += texture( target, fTexCoord + vec2( texelSize.x, 0 ) ) * (-2 / 3);
	colour += texture( target, fTexCoord - vec2( texelSize.x, 0 ) ) * (-2 / 3);
	
	return colour;
}

vec4 Distort() {
	vec2 mid = vec2(0.5f);

	float distanceFromCentre = distance(fTexCoord, mid);
	vec2 normalizedCoord = normalize(fTexCoord - mid);
	float bias = distanceFromCentre + sin(distanceFromCentre * 15) * 0.02f;
	vec2 newCoord = mid + bias * normalizedCoord;
	return texture(target, newCoord);
}

void main() {
//	FragColour = vec4(1,0,0,1);
//	FragColour = Simple();
//	FragColour = BoxBlur();
//	FragColour = Distort();
//	FragColour = Sharpen();
	FragColour = Sobel();
}