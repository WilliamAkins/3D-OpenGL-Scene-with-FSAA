//
// This demo performs lighting in world coordinate space and uses a texture to provide basic diffuse surface properties.
//


#version 330

//
// model-view-projection matrices
// note: seperate out model transform matrix so we can move vertices into world coords for lighting
//
uniform mat4 modelMatrix; // to calc world coords of vertex
uniform mat4 invTransposeModelMatrix; // inverse transpose of model matrix to transform normal vector into world coords

uniform mat4 viewProjectionMatrix; // to calc clip coords once lighting done in world space


//
// input vertex packet
//

layout (location = 0) in vec4 vertexPos;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTexCoord;
layout (location = 3) in vec3 vertexTangent;
layout (location = 4) in vec3 vertexBitangent;
layout (location = 5) in vec4 vertexColour;

//
// output vertex packet
//
out vec4 posWorldCoord;
out vec4 colour;
out vec3 normalWorldCoord;
out vec2 texCoord;

void main(void) {

	// vertex position in world coords - for fragment shader
	posWorldCoord = modelMatrix * vertexPos;
	
	// setup output packet (fragment shader gets packet with interpolated values)
	//colour = vertexPos * vertexNormal;

	normalWorldCoord = (invTransposeModelMatrix * vec4(vertexNormal, 0.0)).xyz; // normal transformed to world coordinate space
	//normalWorldCoord = normalize(normalWorldCoord); // can renormalise normal due to scaling (but done in fragment shader anyway!)

	texCoord = vertexTexCoord;

	// vertex position in clip coords - necessary for pipeline
	gl_Position = viewProjectionMatrix * modelMatrix * vertexPos;
}
