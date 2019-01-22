#version 330

uniform sampler2D texture0;

uniform int screenWidth;
uniform int screenHeight;
uniform int samples;

in vec2 texCoord;

layout (location=0) out vec4 fragColour;

void main(void) {
	float tx = 1.0 / (screenWidth * samples);
    float ty = 1.0 / (screenHeight * samples);

	vec4 newColour = vec4(0.0, 0.0, 0.0, 0.0);

	int size = samples / 2;
	int totalSize = (samples + 1) * (samples + 1);

	for (int y = -size; y <= size; y++) {
		for (int x = -size; x <= size; x++) {
			vec2 pos = texCoord + vec2(float(x), float(y));

			vec2 diff = vec2(float(x) * tx, float(y) * ty);

			newColour += texture(texture0, pos + diff) / totalSize;
		}
	}

	fragColour = newColour;
}