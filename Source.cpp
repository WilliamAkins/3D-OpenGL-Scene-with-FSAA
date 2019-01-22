#include "Includes.h"
#include "HouseScene.h"

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void avgFPS(const float);

enum AATYPE { NONE, MSAA, SSAA };

const int SCREEN_WIDTH = 1000, SCREEN_HEIGHT = 800;
const int ANTIALAISING_TYPE = SSAA;
const int SAMPLES = 8; //resolution multiplier and samples for SSAA & MSAA

// Camera settings
// width, heigh, near plane, far plane
Camera_settings camera_settings{ SCREEN_WIDTH, SCREEN_HEIGHT, 0.1, 100.0 };

//Timer
Timer timer;

// Instantiate the camera object with basic data
Camera camera(camera_settings, glm::vec3(0.0f, 0.5f, 4.0f));

double lastX = camera_settings.screenWidth / 2.0f;
double lastY = camera_settings.screenHeight / 2.0f;

bool			showHouseQuad = true;
HouseScene		*houseScene = nullptr;

TexturedQuad	*houseQuad = nullptr;
TexturedQuad	*texturedQuad = nullptr;

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (ANTIALAISING_TYPE == MSAA)
		glfwWindowHint(GLFW_SAMPLES, SAMPLES);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(camera_settings.screenWidth, camera_settings.screenHeight, "CS3S664 OpenGL Assignment 1 - 15029476 | William Akins", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set the callback functions
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Rendering settings
	glfwSwapInterval(0);		// glfw enable swap interval to match screen v-sync
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); //Enables face culling
	glFrontFace(GL_CCW);//Specifies which winding order if front facing
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (ANTIALAISING_TYPE == MSAA)
		glEnable(GL_MULTISAMPLE);

	TextRenderer textRenderer(SCREEN_WIDTH, SCREEN_HEIGHT);

	bool leftCtrlPressed = false;

	//earthScene = new EarthScene();
	texturedQuad = new TexturedQuad(string("Resources\\Models\\bumblebee.png"));

	if (ANTIALAISING_TYPE == NONE || ANTIALAISING_TYPE == MSAA) {
		houseScene = new HouseScene(SCREEN_WIDTH, SCREEN_HEIGHT, 1);
		houseQuad = new TexturedQuad(houseScene->getHouseSceneTexture(), false, SCREEN_WIDTH, SCREEN_HEIGHT, 1, true);
	} else { //else use SSAA
		houseScene = new HouseScene(SCREEN_WIDTH, SCREEN_HEIGHT, SAMPLES);
		houseQuad = new TexturedQuad(houseScene->getHouseSceneTexture(), true, SCREEN_WIDTH, SCREEN_HEIGHT, SAMPLES, true);
	}

	// render loop
	while (!glfwWindowShouldClose(window))
	{	
		// input
		processInput(window);
		timer.tick();

		if (houseScene)
			houseScene->render();

		// Clear the screen
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Reset the viewport
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		static const char *AATypeText[] = {
			"NONE",
			"MSAA x",
			"SSAA x",
		};
		//if (ANTIALAISING_TYPE != NONE)
			//textRenderer.renderText(AATypeText[ANTIALAISING_TYPE] + std::to_string(SAMPLES), 5.0f, 30.0f, 0.6f, glm::vec3(1.0, 1.0f, 1.0f));
		//textRenderer.renderText("FPS: " + std::to_string(timer.averageFPS()) + " SPF: " + std::to_string(timer.currentSPF()), 5.0f, 5.0f, 0.6f, glm::vec3(1.0, 1.0f, 1.0f));

		//avgFPS(timer.getDeltaTimeSeconds());

		// Update houseScene state
		if (houseScene)
			houseScene->update(timer.getDeltaTimeSeconds());

		if (showHouseQuad) {
			if (houseQuad)
				houseQuad->render();
		} else {
			if (texturedQuad)
				texturedQuad->render();
		}

		// glfw: swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}
void avgFPS(const float timeDelta) {
	static float deltaTime = 0.0f;
	deltaTime += 1.0f * timeDelta;

	const int numOfFrames = 5000;

	static bool finished = false;
	static float fps[numOfFrames];
	static float spf[numOfFrames];
	static int frameCount = 0;

	if (deltaTime > 3.0f && !finished) {
		if (frameCount < numOfFrames) {
			fps[frameCount] = timer.averageFPS();
			spf[frameCount] = timer.currentSPF();
			frameCount++;
		} else {
			float fpsSum = 0;
			float spfSum = 0;
			for (int i = 0; i < numOfFrames; i++) {
				fpsSum += fps[i];
				spfSum += spf[i];
			}
			finished = true;
			fpsSum /= numOfFrames;
			spfSum /= numOfFrames;
			std::cout << "Count finished, avg fps was: " << fpsSum;
			std::cout << "Avg spf was: " << spfSum;
		}
	}
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
	timer.updateDeltaTime();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	
	Camera *ecam;
	if (houseScene) {
		ecam = houseScene->getHouseSceneCamera();

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			ecam->processKeyboard(FORWARD, timer.getDeltaTimeSeconds());
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			ecam->processKeyboard(BACKWARD, timer.getDeltaTimeSeconds());
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			ecam->processKeyboard(LEFT, timer.getDeltaTimeSeconds());
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			ecam->processKeyboard(RIGHT, timer.getDeltaTimeSeconds());
	}
	//if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
	//	samples = 1;
	//	setupHouseScene();
	//}
	//if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
	//	samples = 2;
	//	setupHouseScene();
	//}
	//if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
	//	samples = 4;
	//	setupHouseScene();
	//}
	//if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
	//	samples = 8;
	//	setupHouseScene();
	//}
	//if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
	//	samples = 16;
	//	setupHouseScene();
	//}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		showHouseQuad = !showHouseQuad;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	glViewport(0, 0, width, height);
	camera.updateScreenSize(width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		Camera *ecam;
		if (houseScene) {
			ecam = houseScene->getHouseSceneCamera();
			ecam->processMouseMovement(xoffset, yoffset);
		}
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Camera *ecam;
	if (houseScene) {
		ecam = houseScene->getHouseSceneCamera();
		ecam->processMouseScroll(yoffset);
	}
}