//
// This demo performs lighting in world coordinate space and uses a texture to provide basic diffuse surface properties.
//

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

uniform sampler2D texture0;

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
    float lambertian = clamp(dot(L.xyz, N), 0.0, 1.0);

    //
	// calculate diffuse light colour
    vec4 texColour = texture(texture0, texCoord);
    vec3 diffuseColour = texColour.rgb * lightDiffuseColour.rgb * lambertian; // input colour actually diffuse colour
    //

    //
    // calculate specular light colour
    //

    // vectors needed for specular light calculation...
    vec3 E = cameraPos - posWorldCoord.xyz; // vector from point on object surface in world coords to camera
    E = normalize(E);
    vec3 R = reflect(-L.xyz, N); // reflected light vector about normal N

    float specularIntensity = pow(max(dot(R, E), 0.0), lightSpecularExponent);
    vec3 specularColour = vec3(1.0f, 1.0f, 1.0f) * lightSpecularColour.rgb * specularIntensity * lambertian;


    //
    // combine colour components to get final pixel / fragment colour
    //
    vec3 rgbColour = diffuseColour + specularColour;

    fragColour = vec4(rgbColour, 1.0);
    // Output final gamma corrected colour to framebuffer
    //vec3 P = vec3(1.0 / 2.2);
    //fragColour = vec4(pow(rgbColour, P), 1.0);
}
