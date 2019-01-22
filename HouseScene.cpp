#include "HouseScene.h"
#include "TextureLoader.h"
#include "ShaderLoader.h"
#include <iostream>

using namespace std;

HouseScene::HouseScene(int newWidth, int newHeight, int samples) {
	screenWidth = newWidth * samples;
	screenHeight = newHeight * samples;

	// Camera settings
	//							  width, heigh, near plane, far plane
	Camera_settings camera_settings{ screenWidth, screenHeight, 0.1, 100.0 };


	skySphereModel = new Sphere(32, 16, 30.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), CG_RIGHTHANDED);
	lightSphereModel = new Sphere(16, 8, 0.2f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), CG_RIGHTHANDED);

	houseModel =  new Model("Resources\\Models\\house\\house.obj");
	landModel = new Model("Resources\\Models\\land\\land.obj");
	torchModel = new Model("Resources\\Models\\torch\\torch.obj");
	doorModel = new Model("Resources\\Models\\door\\door.obj");
	ceilingLightModel = new Model("Resources\\Models\\ceilingLight\\ceilingLight.obj");
	fenceModel = new Model("Resources\\Models\\fence\\fence.obj");

	// Instanciate the camera object with basic data
	earthCamera = new Camera(camera_settings, glm::vec3(13.0, 5.0, 0.0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -180.0, -10.0);

	//
	// Setup textures for rendering the Earth model
	//

	skySphereTexture = TextureLoader::loadTexture(string("Resources\\Models\\sky.bmp"));
	houseTexture = TextureLoader::loadTexture(string("Resources\\Models\\house\\house.bmp"));
	landTexture = TextureLoader::loadTexture(string("Resources\\Models\\land\\land.bmp"));
	torchTexture = TextureLoader::loadTexture(string("Resources\\Models\\torch\\torch.bmp"));
	doorTexture = TextureLoader::loadTexture(string("Resources\\Models\\door\\door.bmp"));
	ceilingLightTexture = TextureLoader::loadTexture(string("Resources\\Models\\ceilingLight\\ceilingLight.bmp"));
	fenceTexture = TextureLoader::loadTexture(string("Resources\\Models\\fence\\fence.bmp"));

	textures.push_back(&skySphereTexture);
	textures.push_back(&houseTexture);
	textures.push_back(&landTexture);
	textures.push_back(&torchTexture);
	textures.push_back(&doorTexture);
	textures.push_back(&fenceTexture);

	GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\Phong_shader.vert"),
		string("Resources\\Shaders\\Phong_shader.frag"),
		&phongShader);

	
	// Setup uniform locations for shader
	modelTextureUniform = glGetUniformLocation(phongShader, "texture0");

	modelMatrixLocation = glGetUniformLocation(phongShader, "modelMatrix");

	invTransposeMatrixLocation = glGetUniformLocation(phongShader, "invTransposeModelMatrix");

	viewProjectionMatrixLocation = glGetUniformLocation(phongShader, "viewProjectionMatrix");

	//the sun
	dirLightParams.push_back(DirecionalLightParams());
	dirLightParams.back().direction = glm::vec4(12.0f, 12.0f, 0.0f, 0.0f);
	dirLightParams.back().diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	dirLightParams.back().specular = glm::vec4(0.0005f, 0.0005f, 0.0005f, 1.0f);
	dirLightParams.back().ambient = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
	dirLightParams.back().exponent = 0.1f;
	setupLight(&dirLightParams.back());

	//left torch light
	pointLightParams.push_back(PointLightParams());
	pointLightParams.back().position = glm::vec4(4.55f, 1.7f, 2.0f, 0.0f);
	pointLightParams.back().diffuse = glm::vec4(0.9412f, 0.3765f, 0.0f, 1.0f);
	pointLightParams.back().specular = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	pointLightParams.back().ambient = glm::vec4(0.9412f, 0.3765f, 0.0f, 1.0f);
	pointLightParams.back().exponent = 1.0f;
	pointLightParams.back().attenuation = glm::vec3(1.0, 0.35, 0.44);
	setupLight(&pointLightParams.back());

	//right torch light
	pointLightParams.push_back(PointLightParams());
	pointLightParams.back().position = glm::vec4(4.55f, 1.7f, -2.0f, 0.0f);
	pointLightParams.back().diffuse = glm::vec4(0.9412f, 0.3765f, 0.0f, 1.0f);
	pointLightParams.back().specular = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	pointLightParams.back().ambient = glm::vec4(0.9412f, 0.3765f, 0.0f, 1.0f);
	pointLightParams.back().exponent = 1.0f;
	pointLightParams.back().attenuation = glm::vec3(1.0, 0.35, 0.44);
	setupLight(&pointLightParams.back());

	//house light
	pointLightParams.push_back(PointLightParams());
	pointLightParams.back().position = glm::vec4(0.0f, 5.1f, 0.0f, 0.0f);
	pointLightParams.back().diffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	pointLightParams.back().specular = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	pointLightParams.back().ambient = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	pointLightParams.back().exponent = 1.0f;
	pointLightParams.back().attenuation = glm::vec3(1.0, 0.7, 2.0);
	setupLight(&pointLightParams.back());

	cameraPosLocation = glGetUniformLocation(phongShader, "cameraPos");
	// Set constant uniform data (uniforms that will not change while the application is running)
	// Note: Remember we need to bind the shader before we can set uniform variables!
	glUseProgram(phongShader);
	glUniform1i(modelTextureUniform, 0);
	glUseProgram(0);


	//
	// Setup FBO (which Earth rendering pass will draw into)
	//

	glGenFramebuffers(1, &demoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, demoFBO);


	//
	// Setup textures that will be drawn into through the FBO
	//

	// Setup colour buffer texture.
	// Note:  The texture is stored as linear RGB values (GL_RGBA8).  
	//There is no need to pass a pointer to image data - 
	//we're going to fill in the image when we render the Earth scene at render time!
	glGenTextures(1, &fboColourTexture);
	glBindTexture(GL_TEXTURE_2D, fboColourTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	// Setup depth texture

	glGenTextures(1, &fboDepthTexture);
	glBindTexture(GL_TEXTURE_2D, fboDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	//
	// Attach textures to the FBO
	//

	// Attach the colour texture object to the framebuffer object's colour attachment point #0
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		fboColourTexture,
		0);

	// Attach the depth texture object to the framebuffer object's depth attachment point
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D,
		fboDepthTexture,
		0);


	//
	// Before proceeding make sure FBO can be used for rendering
	//


	GLenum demoFBOStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (demoFBOStatus != GL_FRAMEBUFFER_COMPLETE) {

		fboOkay = false;
		cout << "Could not successfully create framebuffer object to render texture!" << endl;

	}
	else {

		fboOkay = true;
		cout << "FBO successfully created" << endl;
	}

	// Unbind FBO for now! (Plug main framebuffer back in as rendering destination)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

HouseScene::~HouseScene() {}

// Accessor methods
Camera* HouseScene::getHouseSceneCamera() {

	return earthCamera;
}


GLuint HouseScene::getHouseSceneTexture() {

	return fboColourTexture;
}


float HouseScene::getSunTheta() {

	return sunTheta;
}


void HouseScene::updateSunTheta(float thetaDelta) {

	sunTheta += thetaDelta;
}


// Scene update
void HouseScene::update(const float timeDelta) {

	// Update rotation angle ready for next frame
	//earthTheta += 15.0f * float(timeDelta);
	//sunTheta -= 15.0f * float(timeDelta);

	static float timer = 0.0f;
	timer += 1.0f * timeDelta;

	if (timer >= 1.0f) {
		glm::vec4 colour;
		timer = 0.0f;

		static int num = 0;

		switch (num) {
		case 0:
			colour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		case 1:
			colour = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
			break;
		case 2:
			colour = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
			break;
		}

		pointLightParams[2].ambient = colour;
		pointLightParams[2].diffuse = colour;
		updateLight(POINT, 2);

		if (num < 2)
			num++;
		else
			num = 0;
	}
}

void HouseScene::setupLight(DirecionalLightParams* lp) {
	dirLightUniforms.push_back(DirectionalLightUniforms());

	int lightNum = dirLightParams.size() - 1;
	char buffer[64];

	sprintf_s(buffer, "dirLight[%i].lightDirection", lightNum);
	dirLightUniforms.back().directionLocation = glGetUniformLocation(phongShader, buffer);

	sprintf_s(buffer, "dirLight[%i].lightDiffuseColour", lightNum);
	dirLightUniforms.back().diffuseLocation = glGetUniformLocation(phongShader, buffer);

	sprintf_s(buffer, "dirLight[%i].lightSpecularColour", lightNum);
	dirLightUniforms.back().specularLocation = glGetUniformLocation(phongShader, buffer);

	sprintf_s(buffer, "dirLight[%i].lightAmbientColour", lightNum);
	dirLightUniforms.back().ambientLocation = glGetUniformLocation(phongShader, buffer);

	sprintf_s(buffer, "dirLight[%i].lightSpecularExponent", lightNum);
	dirLightUniforms.back().specExpLocation = glGetUniformLocation(phongShader, buffer);

	glUseProgram(phongShader);
	glUniform4f(dirLightUniforms.back().directionLocation, lp->direction.x, lp->direction.y, lp->direction.z, lp->direction.w); // world coordinate space vector
	glUniform4f(dirLightUniforms.back().diffuseLocation, lp->diffuse.x, lp->diffuse.y, lp->diffuse.z, lp->diffuse.w); // white diffuse light
	glUniform4f(dirLightUniforms.back().specularLocation, lp->specular.x, lp->specular.y, lp->specular.z, lp->specular.w); // white specular light
	glUniform4f(dirLightUniforms.back().ambientLocation, lp->ambient.x, lp->ambient.y, lp->ambient.z, lp->ambient.w); // ambient light
	glUniform1f(dirLightUniforms.back().specExpLocation, lp->exponent); // specular exponent / falloff
	glUseProgram(0);
}

void HouseScene::setupLight(PointLightParams* lp)
{
	pointLightUniforms.push_back(PointLightUniforms());

	int lightNum = pointLightParams.size() - 1;
	char buffer[64];

	sprintf_s(buffer, "pointLight[%i].lightPosition", lightNum);
	pointLightUniforms.back().directionLocation = glGetUniformLocation(phongShader, buffer);

	sprintf_s(buffer, "pointLight[%i].lightDiffuseColour", lightNum);
	pointLightUniforms.back().diffuseLocation = glGetUniformLocation(phongShader, buffer);

	sprintf_s(buffer, "pointLight[%i].lightSpecularColour", lightNum);
	pointLightUniforms.back().specularLocation = glGetUniformLocation(phongShader, buffer);

	sprintf_s(buffer, "pointLight[%i].lightAmbientColour", lightNum);
	pointLightUniforms.back().ambientLocation = glGetUniformLocation(phongShader, buffer);

	sprintf_s(buffer, "pointLight[%i].lightSpecularExponent", lightNum);
	pointLightUniforms.back().specExpLocation = glGetUniformLocation(phongShader, buffer);

	sprintf_s(buffer, "pointLight[%i].lightAttenuation", lightNum);
	pointLightUniforms.back().attenuation = glGetUniformLocation(phongShader, buffer);

	glUseProgram(phongShader);
	glUniform4f(pointLightUniforms.back().directionLocation, lp->position.x, lp->position.y, lp->position.z, lp->position.w); // world coordinate space vector
	glUniform4f(pointLightUniforms.back().diffuseLocation, lp->diffuse.x, lp->diffuse.y, lp->diffuse.z, lp->diffuse.w); // white diffuse light
	glUniform4f(pointLightUniforms.back().specularLocation, lp->specular.x, lp->specular.y, lp->specular.z, lp->specular.w); // white specular light
	glUniform4f(pointLightUniforms.back().ambientLocation, lp->ambient.x, lp->ambient.y, lp->ambient.z, lp->ambient.w); // ambient light
	glUniform1f(pointLightUniforms.back().specExpLocation, lp->exponent); // specular exponent / falloff
	glUniform3f(pointLightUniforms.back().attenuation, lp->attenuation.x, lp->attenuation.y, lp->attenuation.z); // attenuation
	glUseProgram(0);
}

void HouseScene::updateLight(LightType type, int lightID)
{
	if (type == POINT) {
		PointLightUniforms* curLight = &pointLightUniforms[lightID];
		PointLightParams* lp = &pointLightParams[lightID];

		glUseProgram(phongShader);
		glUniform4f(curLight->directionLocation, lp->position.x, lp->position.y, lp->position.z, lp->position.w); // world coordinate space vector
		glUniform4f(curLight->diffuseLocation, lp->diffuse.x, lp->diffuse.y, lp->diffuse.z, lp->diffuse.w); // white diffuse light
		glUniform4f(curLight->specularLocation, lp->specular.x, lp->specular.y, lp->specular.z, lp->specular.w); // white specular light
		glUniform4f(curLight->ambientLocation, lp->ambient.x, lp->ambient.y, lp->ambient.z, lp->ambient.w); // ambient light
		glUniform1f(curLight->specExpLocation, lp->exponent); // specular exponent / falloff
		glUniform3f(curLight->attenuation, lp->attenuation.x, lp->attenuation.y, lp->attenuation.z); // attenuation
		glUseProgram(0);
	}
}

void HouseScene::renderModel(Model* newModel, glm::mat4* transform, glm::mat4* T, GLuint* newTexture, int frontFace) {
	if (newModel) {
		// Calculate inverse transpose of the modelling transform for correct transformation of normal vectors
		glm::mat4 inverseTranspose = glm::transpose(glm::inverse(*transform));

		glUseProgram(phongShader);

		// Get the location of the camera in world coords and set the corresponding uniform in the shader
		glm::vec3 cameraPos = earthCamera->getCameraPosition();
		glUniform3fv(cameraPosLocation, 1, (GLfloat*)&cameraPos);

		// Set the model, view and projection matrix uniforms (from the camera data obtained above)
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(*transform));
		glUniformMatrix4fv(invTransposeMatrixLocation, 1, GL_FALSE, glm::value_ptr(inverseTranspose));
		glUniformMatrix4fv(viewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(*T));

		// Activate and Bind the textures to texture units
		if (newTexture) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, *newTexture);
		}

		//Render the model
		glFrontFace(frontFace);
		newModel->render();
		glFrontFace(frontFace);

		// Restore default OpenGL shaders (Fixed function operations)
		glUseProgram(0);
	}
}

void HouseScene::renderModel(Sphere* newModel, glm::mat4* transform, glm::mat4* T, GLuint* newTexture, int frontFace) {
	if (newModel) {
		// Calculate inverse transpose of the modelling transform for correct transformation of normal vectors
		glm::mat4 inverseTranspose = glm::transpose(glm::inverse(*transform));

		glUseProgram(phongShader);

		// Get the location of the camera in world coords and set the corresponding uniform in the shader
		glm::vec3 cameraPos = earthCamera->getCameraPosition();
		glUniform3fv(cameraPosLocation, 1, (GLfloat*)&cameraPos);

		// Set the model, view and projection matrix uniforms (from the camera data obtained above)
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(*transform));
		glUniformMatrix4fv(invTransposeMatrixLocation, 1, GL_FALSE, glm::value_ptr(inverseTranspose));
		glUniformMatrix4fv(viewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(*T));

		// Activate and Bind the textures to texture units
		if (newTexture) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, *newTexture);
		}

		//Render the model
		glFrontFace(frontFace);
		newModel->render();
		glFrontFace(frontFace);

		// Restore default OpenGL shaders (Fixed function operations)
		glUseProgram(0);
	}
}

void HouseScene::renderLightSpheres() {
	glm::mat4 T = earthCamera->getProjectionMatrix() * earthCamera->getViewMatrix();
	glm::mat4 modelTransform;

	for (int i = 0; i < dirLightParams.size(); i++) {
		modelTransform = glm::translate(glm::mat4(1.0), glm::vec3(dirLightParams[i].direction.x, dirLightParams[i].direction.y, dirLightParams[i].direction.z));
		renderModel(lightSphereModel, &modelTransform, &T);
	}

	for (int i = 0; i < pointLightParams.size(); i++) {
		modelTransform = glm::translate(glm::mat4(1.0), glm::vec3(pointLightParams[i].position.x, pointLightParams[i].position.y, pointLightParams[i].position.z));
		renderModel(lightSphereModel, &modelTransform, &T);
	}
}

// Rendering methods
void HouseScene::render() {

	if (!fboOkay)
		return; // Don't render anything if the FBO was not created successfully

	// Bind framebuffer object so all rendering redirected to attached images (i.e. our texture)
	glBindFramebuffer(GL_FRAMEBUFFER, demoFBO);

	// All rendering from this point goes to the bound textures (setup at initialisation time) and NOT the actual screen!!!!!

	// Clear the screen (i.e. the texture)
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set viewport to specified texture size (see above)
	glViewport(0, 0, screenWidth, screenHeight);

	// Get view-projection transform as a CGMatrix4
	glm::mat4 T = earthCamera->getProjectionMatrix() * earthCamera->getViewMatrix();
	glm::mat4 modelTransform;

	modelTransform = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 0.0f));
	renderModel(skySphereModel, &modelTransform, &T, &skySphereTexture, GL_CW);

	modelTransform = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 0.0f));
	renderModel(houseModel, &modelTransform, &T, &houseTexture);

	modelTransform = glm::translate(glm::mat4(1.0), glm::vec3(1.65f, 0.0f, -1.9f));
	modelTransform = glm::rotate(modelTransform, -50.0f * (3.1459f / 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	renderModel(doorModel, &modelTransform, &T, &doorTexture);

	modelTransform = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -0.5f, 0.0f));
	renderModel(landModel, &modelTransform, &T, &landTexture);

	modelTransform = glm::translate(glm::mat4(1.0), glm::vec3(-4.6f, 0.3f, 0.0f));
	renderModel(ceilingLightModel, &modelTransform, &T, &ceilingLightTexture);

	for (int i = 0; i < 15; i++) {
		modelTransform = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -0.5f, 0.0f));
		modelTransform = glm::rotate(modelTransform, ((i * 17.0f) - 210.0f) * (3.1459f / 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		renderModel(fenceModel, &modelTransform, &T, &fenceTexture);
	}

	for (int i = 0; i < 2; i++) {
		modelTransform = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -0.5f, i * 4));
		renderModel(torchModel, &modelTransform, &T, &torchTexture);
	}

	//will render a sphere on the origin point of each light
	renderLightSpheres();

	// Set OpenGL to render to the MAIN framebuffer (ie. the screen itself!!)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}