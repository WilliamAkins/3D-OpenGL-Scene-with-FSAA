#version 330

//
// basic directional light model
//
uniform vec4		lightDirection; // direction light comes FROM (specified in World Coordinates)
uniform vec4		lightDiffuseColour;
uniform vec4        lightSpecularColour;
uniform float       lightSpecularExponent;

uniform vec3        cameraPos; // to calculate specular lighting in world coordinate space, we need the location of the camera since the specular light
    // term is viewer dependent

uniform sampler2D dayTexture, nightTexture, cloudMaskTexture;

//
// input fragment packet (contains interpolated values for the fragment calculated by the rasteriser)
//
in vec4 posWorldCoord;
in vec4 colour;
in vec3 normalWorldCoord;
in vec2 texCoord;

//
// output fragment colour
//
layout (location = 0) out vec4 fragColour;


void main(void) {

	// make sure light direction vector is unit length (store in L)
	vec4 L = normalize(lightDirection);
    
	// important to normalise length of normal otherwise shading artefacts occur
	vec3 N = normalize(normalWorldCoord);
	
    // calculate lambertian term
    float dp = dot(L.xyz, N);
    float lambertian = clamp(dp, 0.0, 1.0);

    //
	// calculate diffuse light colour
    vec4 dayTexColour = texture(dayTexture, texCoord);
    vec3 diffuseColour = dayTexColour.rgb * lightDiffuseColour.rgb * lambertian; // input colour actually diffuse colour

    //
    // calculate specular light colour
    //

    // vectors needed for specular light calculation...
    vec3 E = cameraPos - posWorldCoord.xyz; // vector from point on object surface in world coords to camera
    E = normalize(E);
    vec3 R = reflect(-L.xyz, N); // reflected light vector about normal N

    float specularIntensity = pow(max(dot(R, E), 0.0), lightSpecularExponent);
    vec3 specularColour = vec3(1.0f, 1.0f, 1.0f) * lightSpecularColour.rgb * specularIntensity * lambertian;

    
    vec3 clouds = texture(cloudMaskTexture, texCoord).rgb;

    vec3 dayTime = (diffuseColour + (specularColour * clouds.g)) * (1.0 - clouds.r) + clouds.r * lambertian;

    vec3 nightTime = texture(nightTexture, texCoord).rgb * (1.0 - clouds.r);



    //
    // combine colour components to get final pixel / fragment colour
    //
    
    vec3 finalColour = dayTime;

    if (dp < -0.1)
		finalColour = nightTime;
	if (abs(dp) < 0.1)
		finalColour = mix(nightTime, dayTime, (dp + 0.1) * 5.0);
    
    fragColour = vec4(finalColour, 1.0);
    // Output final gamma corrected colour to framebuffer
   // vec3 P = vec3(1.0 / 2.2);
   // fragColour = vec4(pow(finalColour, P), 1.0);

    // output final (linear) colour to framebuffer
	//fragColour = vec4(finalColour, 1.0);
}
