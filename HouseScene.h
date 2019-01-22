#ifndef HOUSESCENE_H
#define HOUSESCENE_H

#include "Camera.h"
#include "Includes.h"

class HouseScene {
	private:
		//the width and height of the fbo
		int screenWidth = 800;
		int screenHeight = 800;

		enum LightType {DIRECTION, POINT};

		struct DirecionalLightParams {
			glm::vec4 direction;
			glm::vec4 diffuse;
			glm::vec4 specular;
			glm::vec4 ambient;
			float exponent;
		};
		vector<DirecionalLightParams> dirLightParams;

		struct DirectionalLightUniforms {
			GLint directionLocation;
			GLint diffuseLocation;
			GLint specularLocation;
			GLint ambientLocation;
			GLint specExpLocation;
		};
		vector<DirectionalLightUniforms> dirLightUniforms;

		struct PointLightParams {
			glm::vec4 position;
			glm::vec4 diffuse;
			glm::vec4 specular;
			glm::vec4 ambient;
			float exponent;
			glm::vec3 attenuation;
		};
		vector<PointLightParams> pointLightParams;

		struct PointLightUniforms {
			GLint directionLocation;
			GLint diffuseLocation;
			GLint specularLocation;
			GLint ambientLocation;
			GLint specExpLocation;
			GLint attenuation;
		};
		vector<PointLightUniforms> pointLightUniforms;

		Sphere							*skySphereModel;
		Sphere							*lightSphereModel;

		Model							*houseModel;
		Model							*landModel;
		Model							*torchModel;
		Model							*doorModel;
		Model							*ceilingLightModel;
		Model							*fenceModel;

		// Move around the earth with a seperate camera to the main scene camera
		Camera							*earthCamera;

		// Textures for multi-texturing the earth model
		vector<GLuint*>					textures;
		GLuint							skySphereTexture;
		GLuint							houseTexture;
		GLuint							landTexture;
		GLuint							torchTexture;
		GLuint							doorTexture;
		GLuint							ceilingLightTexture;
		GLuint							fenceTexture;

		// Shader for multi-texturing the earth
		GLuint							phongShader;


		// Unifom locations for earthShader

		// Texture uniforms
		GLuint							modelTextureUniform;

		// Camera uniforms
		GLint							modelMatrixLocation;
		GLint							invTransposeMatrixLocation;
		GLint							viewProjectionMatrixLocation;

		// Directional light uniforms
		/*vector<GLint>					lightDirectionLocation;
		vector<GLint>					lightDiffuseLocation;
		vector<GLint>					lightSpecularLocation;
		vector<GLint>					lightSpecExpLocation;
		vector<GLint>					lightAttenuation;*/
		GLint							cameraPosLocation;

		//
		// Animation state
		//
		float							sunTheta; // Angle to the Sun in the orbital plane of the Earth (the xz plane in the demo)


		//
		// Framebuffer Object (FBO) variables
		//

		// Actual FBO
		GLuint							demoFBO;

		// Colour texture to render into
		GLuint							fboColourTexture;

		// Depth texture to render into
		GLuint							fboDepthTexture;

		// Flag to indicate that the FBO is valid
		bool							fboOkay;

		void							setupLight(DirecionalLightParams*);
		void							setupLight(PointLightParams*);

		void							updateLight(LightType, int);

		void							renderLightSpheres();

		void							renderModel(Model*, glm::mat4*, glm::mat4*, GLuint* = nullptr, int frontFace = GL_CCW);
		void							renderModel(Sphere*, glm::mat4*, glm::mat4*, GLuint* = nullptr, int frontFace = GL_CCW);
	public:

		HouseScene(int newWidth = 800, int newHeight = 800, int sampleSize = 1);
		~HouseScene();

		// Accessor methods
		void updateScene(int newWidth = 800, int newHeight = 800, int sampleSize = 1);
		Camera* getHouseSceneCamera();
		GLuint getHouseSceneTexture();
		float getSunTheta();
		void updateSunTheta(float thetaDelta);

		// Scene update
		void update(const float timeDelta);

		// Rendering methods
		void render();
};
#endif