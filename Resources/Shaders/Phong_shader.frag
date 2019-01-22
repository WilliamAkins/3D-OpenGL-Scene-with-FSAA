#version 330

//maximum number of supported lights
#define NUM_OF_DIR_LIGHTS 1
#define NUM_OF_POINT_LIGHTS 3

struct DirLight {
    vec4 lightDirection; // direction light comes FROM (specified in World Coordinates)
	vec4 lightDiffuseColour;
	vec4 lightSpecularColour;
	vec4 lightAmbientColour;
	float lightSpecularExponent;
};
uniform DirLight dirLight[NUM_OF_DIR_LIGHTS];

struct PointLight {
    vec4 lightPosition; // direction light comes FROM (specified in World Coordinates)
	vec4 lightDiffuseColour;
	vec4 lightSpecularColour;
	vec4 lightAmbientColour;
	float lightSpecularExponent;
	vec3 lightAttenuation;
};
uniform PointLight pointLight[NUM_OF_POINT_LIGHTS];

vec3 calcDirLight(DirLight light);
vec3 calcPointLight(PointLight light);

uniform vec3 cameraPos; // to calculate specular lighting in world coordinate space, we need the location of the camera since the specular light
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
	// define an output color value
	vec3 output = vec3(0.0);

	// add the directional light's contribution to the output
	for(int i = 0; i < NUM_OF_DIR_LIGHTS; i++)
		output += calcDirLight(dirLight[i]);

	for(int i = 0; i < NUM_OF_POINT_LIGHTS; i++)
		output += calcPointLight(pointLight[i]);

    //fragColour = vec4(output, 1.0);
    // Output final gamma corrected colour to framebuffer
    vec3 P = vec3(1.0 / 0.8);
    fragColour = vec4(pow(output, P), 1.0);
}

vec3 calcDirLight(DirLight light) {
	// make sure light direction vector is unit length (store in L)
	vec4 L = normalize(light.lightDirection);
    
	// important to normalise length of normal otherwise shading artefacts occur
	vec3 N = normalize(normalWorldCoord);
	
    // calculate lambertian term
    float lambertian = clamp(dot(L.xyz, N), 0.0, 1.0);

    //
	// calculate diffuse light colour
    vec4 texColour = texture(texture0, texCoord);
    vec3 diffuseColour = texColour.rgb * light.lightDiffuseColour.rgb * lambertian; // input colour actually diffuse colour
    //

    vec3 amibentColour = texColour.rgb * light.lightAmbientColour.rgb;

    // vectors needed for specular light calculation...
    vec3 E = cameraPos - posWorldCoord.xyz; // vector from point on object surface in world coords to camera
    E = normalize(E);
    vec3 R = reflect(-L.xyz, N); // reflected light vector about normal N

    float specularIntensity = pow(max(dot(R, E), 0.0), light.lightSpecularExponent);
    vec3 specularColour = vec3(1.0f, 1.0f, 1.0f) * light.lightSpecularColour.rgb * specularIntensity * lambertian;

	//
    // combine colour components to get final pixel / fragment colour
    //
    vec3 rgbColour = amibentColour + diffuseColour + specularColour;

	return rgbColour;
}

vec3 calcPointLight(PointLight light) {
	// make sure light direction vector is unit length (store in L)
	vec4 L = normalize(light.lightPosition - posWorldCoord);
    
	// important to normalise length of normal otherwise shading artefacts occur
	vec3 N = normalize(normalWorldCoord);
	
    // calculate lambertian term
    float lambertian = clamp(dot(L.xyz, N), 0.0, 1.0);

    //
	// calculate diffuse light colour
    vec4 texColour = texture(texture0, texCoord);
    vec3 diffuseColour = texColour.rgb * light.lightDiffuseColour.rgb * lambertian; // input colour actually diffuse colour
    //

    //
    // calculate specular light colour
    //

    // vectors needed for specular light calculation...
    vec3 E = cameraPos - posWorldCoord.xyz; // vector from point on object surface in world coords to camera
    E = normalize(E);
    vec3 R = reflect(-L.xyz, N); // reflected light vector about normal N

    float specularIntensity = pow(max(dot(R, E), 0.0), light.lightSpecularExponent);
    vec3 specularColour = texColour.rgb * vec3(1.0f, 1.0f, 1.0f) * light.lightSpecularColour.rgb * specularIntensity * lambertian;

	vec3 amibentColour = texColour.rgb * light.lightAmbientColour.rgb;

	// attenuation
    float dist = length(light.lightPosition - posWorldCoord);
    float attenuation = 1.0f / (light.lightAttenuation.x + light.lightAttenuation.y * dist + light.lightAttenuation.z * (dist * dist));

	//
    // combine colour components to get final pixel / fragment colour
    //
	amibentColour *= attenuation;
    specularColour *= attenuation;
	diffuseColour *= attenuation;

	vec3 rgbColour = amibentColour + diffuseColour + specularColour;

	return rgbColour;
}