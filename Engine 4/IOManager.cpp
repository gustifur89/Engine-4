#include "IOManager.h"



IOManager::IOManager()
{
	window = NULL;
	oldTime = 0.0;
	deltaTime = 0.0;
	mouseLockedState = false;

	clearColor = glm::vec3(0.0);
}

// ===================== IOManager - Window =============================

bool IOManager::createWindow(int width, int height, std::string title, int fps)
{
	this->width = width;
	this->height = height;
	this->title = title;
	this->aspectRatio = (double) width / height;
	this->fps = fps;
	this->timeDelay = 1.0 / fps;

	glewExperimental = true;
	if (!glfwInit())
	{
		fprintf(stderr, "glfw failed to init");
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n");
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window); // Initialize GLEW
	glewExperimental = true; // Needed in core profilea
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return false;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	setUpWindowQuad();

	screenShader = Shader::loadShader("screen","screen");

	gBufferTexture = GBufferRenderTexture(width, height);
	screenTexture = RenderTexture2D(width, height);
	
	glBindFramebuffer(GL_FRAMEBUFFER, gBufferTexture.frameBuffer);

	glSetUp();

	glViewport(0, 0, width, height);

	std::cout << "GL Context Version  " << glGetString(GL_VERSION) << "\n";

	return true;
}

void IOManager::setUpWindowQuad()
{
	// The fullscreen quad's FBO
	glGenVertexArrays(1, &windowVAO);
	glBindVertexArray(windowVAO);

	static const GLfloat windowVertexBufferData[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f,  1.0f, 0.0f,
	};

	glGenBuffers(1, &windowVBO);
	glBindBuffer(GL_ARRAY_BUFFER, windowVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(windowVertexBufferData), windowVertexBufferData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,					// attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void IOManager::glSetUp()
{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glClearDepth(1.f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void IOManager::setClearColor(int r, int g, int b)
{
	clearColor = (1.0f / 255.0f) * glm::vec3(r, g, b);
	glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0);
}

bool IOManager::isWindowOpen()
{

	delay(timeDelay);

	/*
	double sTime = glfwGetTime();

	while (glfwGetTime() - sTime < timeDelay)
	{
		//delays
	}

	*/

	return glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0;
}

void IOManager::delay(double timeDelay)
{
	double sTime = glfwGetTime();

	while (glfwGetTime() - sTime < timeDelay)
	{

	}
}

// ===================== IOManager - Keyboard/mouse =============================

bool IOManager::isKeyPressed(int key)
{
	return glfwGetKey(window, key) == GLFW_PRESS;
}

bool IOManager::isMouseDown(int mouseButton) {
	return glfwGetMouseButton(window, mouseButton) == 1;
}

double IOManager::getMouseX() {
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	return xPos;
}

double IOManager::getMouseY() {
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	return yPos;
}

void IOManager::lockMouse() {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	mouseLockedState = true;
}

void IOManager::unlockMouse() {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	mouseLockedState = false;
}

bool IOManager::getMouseLockState() {
	return mouseLockedState;
}

void IOManager::setDeltaMouse() {
	deltaMouseX = getMouseX() - oldMouseX;
	deltaMouseY = getMouseY() - oldMouseY;
	oldMouseX += deltaMouseX;
	oldMouseY += deltaMouseY;
}

void IOManager::toggleMouseState()
{
	if (mouseLockedState)
	{
		unlockMouse();
	}
	else
	{
		lockMouse();
	}
}

// ======================== IOManager - Rendering ========================

void IOManager::display(std::shared_ptr<Camera> camera, std::shared_ptr<GameObject> renderObject)
{
	deltaTime = glfwGetTime() - oldTime;
	oldTime += deltaTime;
	setDeltaMouse();
	if (!window) return;
	
	if (camera != nullptr && renderObject != nullptr)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, gBufferTexture.frameBuffer);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderObject->render(camera, glm::mat4(1.0));

		renderWindow();
	}

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void IOManager::renderWindow()
{ 
	if (!windowShader) return;
 
	//render gBuffer to whatever preprocessing buffers you need.
	// Ambient occlusions and such.
	renderPreProcessing();

	//renders from gbuffer to a simple 2d texture - screenTexture.
	windowShader->useShader();
	glBindTextureUnit(0, gBufferTexture.colTex);
	glBindTextureUnit(1, gBufferTexture.posTex);
	glBindTextureUnit(2, gBufferTexture.normTex);

	glBindFramebuffer(GL_FRAMEBUFFER, screenTexture.frameBuffer);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(windowVAO);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	
	
	//then it renders all of the post processing:
	renderPostProcessing();

	//At the end, render to screen.
	screenShader->useShader();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTextureUnit(0, screenTexture.colTex);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(windowVAO);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	
}

void IOManager::renderPostProcessing()
{
	for (std::shared_ptr<Shader> shader : postProcessingList)
	{
		shader->useShader();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTextureUnit(0, screenTexture.colTex);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(windowVAO);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
	}
}

void IOManager::renderPreProcessing()
{
	for (std::shared_ptr<Shader> shader : preProcessingList)
	{
		shader->useShader();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTextureUnit(0, gBufferTexture.colTex);
		glBindTextureUnit(1, gBufferTexture.posTex);
		glBindTextureUnit(2, gBufferTexture.normTex);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(windowVAO);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
		//shader->eprocess();
	}
}

void IOManager::setWindowShader(std::shared_ptr<Shader> windowShader)
{
	this->windowShader = windowShader;
	windowShader->useShader();
	GLuint colTexLoc = windowShader->getUniformLocation("colTex");
	GLuint posTexLoc = windowShader->getUniformLocation("posTex");
	GLuint normTexLoc = windowShader->getUniformLocation("normTex");
	glUniform1i(colTexLoc, 0);
	glUniform1i(posTexLoc, 1);
	glUniform1i(normTexLoc, 2);

}

void IOManager::setPostProcessingList(std::vector<std::shared_ptr<Shader>> postProcessingList)
{
	this->postProcessingList = postProcessingList;
}

void IOManager::setPreProcessingList(std::vector<std::shared_ptr<Shader>> preProcessingList)
{
	this->preProcessingList = preProcessingList;
}
