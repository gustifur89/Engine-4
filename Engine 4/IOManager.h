#pragma once
#include "Headers.h"
#include "GameObject.h"
#include "RenderTexture.h"
#include "Light.h"

class IOManager
{
protected:
	float oldTime;
	double oldMouseX, oldMouseY;
	void renderWindow();
	void renderPostProcessing();
	void renderPreProcessing();

	glm::vec3 clearColor;

	RenderTexture2D screenTexture;
	GBufferRenderTexture gBufferTexture;

	GLuint windowVBO;
	GLuint windowVAO;

	std::shared_ptr<Shader> screenShader;

	std::vector<std::shared_ptr<Shader>> postProcessingList;
	std::vector<std::shared_ptr<Shader>> preProcessingList;

public:
	IOManager();

	// Window
	bool createWindow(int width, int height, std::string title, int fps);
	void setUpWindowQuad();
	void glSetUp();


	//Keyboard

	bool isWindowOpen();
	bool isKeyPressed(int key);
	bool isMouseDown(int mouseButton);;
	double getMouseX();
	double getMouseY();
	void lockMouse();
	void unlockMouse();
	bool getMouseLockState();
	void setDeltaMouse();
	void toggleMouseState();

	//Time
	void delay(double timeDelay);

	//rendering
	void setClearColor(int r, int g, int b);
	void display(std::shared_ptr<Camera> camera, std::shared_ptr<GameObject> renderObject);
	void setWindowShader(std::shared_ptr<Shader> windowShader);
	void setPostProcessingList(std::vector<std::shared_ptr<Shader>> postProcessingList);
	void setPreProcessingList(std::vector<std::shared_ptr<Shader>> preProcessingList);

	//parameters
	GLFWwindow* window;
	int width, height;
	double aspectRatio;
	double timeDelay;
	int fps;
	bool mouseLockedState;
	double deltaMouseX, deltaMouseY;
	double r, g, b;
	std::string title;
	
	float deltaTime;
	std::shared_ptr<Shader> windowShader;
};

