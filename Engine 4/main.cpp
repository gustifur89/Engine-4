#include "Header.h"
#include "IOManager.h"
#include "WindowShader.h"
#include "Toggle.h"
#include "Primitives.h"
#include "Physics.h"
#include "Octree.h"

glm::vec3 cameraRotation;
void move(IOManager& IO, std::shared_ptr<Camera> camera)
{
	bool w = IO.isKeyPressed(GLFW_KEY_W);
	bool a = IO.isKeyPressed(GLFW_KEY_A);
	bool s = IO.isKeyPressed(GLFW_KEY_S);
	bool d = IO.isKeyPressed(GLFW_KEY_D);
	bool e = IO.isKeyPressed(GLFW_KEY_SPACE);
	bool q = IO.isKeyPressed(GLFW_KEY_LEFT_CONTROL);

	float speed = 20.0;

	float xSpeed = 0.0;
	float ySpeed = 0.0;
	float zSpeed = 0.0;

	if (w == s)
	{
		zSpeed = 0.0;
	}
	else if (w)
	{
		zSpeed = speed;
	}
	else
	{
		zSpeed = -speed;
	}

	if (a == d)
	{
		xSpeed = 0.0;
	}
	else if (a)
	{
		xSpeed = speed;
	}
	else
	{
		xSpeed = -speed;
	}

	if (e == q)
	{
		ySpeed = 0.0;
	}
	else if (e)
	{
		ySpeed = speed;
	}
	else
	{
		ySpeed = -speed;
	}
	
	double mouseSensitivity = 1.0;

	if (IO.getMouseLockState()) {
		double deltaPitch = IO.deltaMouseY * mouseSensitivity;
		double deltaPivot = -IO.deltaMouseX * mouseSensitivity;
		cameraRotation.x += deltaPitch;
		cameraRotation.y += deltaPivot;
		if (cameraRotation.x > 89.9) cameraRotation.x = 89.9;
		if (cameraRotation.x < -89.9) cameraRotation.x = -89.9;
	}

	camera->setRotation(cameraRotation);
	
	glm::vec3 directionForward = camera->getTransformedZ();
	glm::vec3 directionStrafe = camera->getTransformedX();
	glm::vec3 directionVertical = camera->getTransformedY();

	glm::vec3 vel = xSpeed * directionStrafe + zSpeed * directionForward + ySpeed * directionVertical;

	camera->translate(IO.deltaTime * vel);
}

void makeBunchOfStuff(std::shared_ptr<GameObject> stage, std::shared_ptr<ColorMesh> mesh, std::shared_ptr<ColorShader> colorShader, int number)
{
	for (int i = 0; i < number; i++)
	{
		std::shared_ptr<GameObjectColor> thing(new GameObjectColor(mesh, colorShader));
		thing->transform.setPosition(rand() % 30 - 15, rand() % 30 - 15, rand() % 30 - 15);
		thing->transform.setRotation(rand() % 360, rand() % 360, rand() % 360);
		thing->setFillColor(rand() % 255, rand() % 255, rand() % 255);
		thing->setShininess((rand() % 1000) / 1000.0);
		stage->addChild(thing);
	}
}

int main()
{
	srand(0);
	IOManager IO;
	IO.createWindow(1280, 800, "test", 30);
	//IO.setClearColor(200, 60, 100); pink
	IO.setClearColor(214, 252, 255);
	std::shared_ptr<WindowShader> windowShader = WindowShader::loadShader("screen", "window");
	windowShader->setGlobalLight(glm::normalize(glm::vec3(-1, 1, -1)));
	windowShader->setAmbient(0.4);

	Primitive::init();

	IO.setWindowShader(windowShader);

	std::shared_ptr<ColorShader> colorShader = ColorShader::loadShader("color");

	std::shared_ptr<ColorMesh> cubeMesh = ColorMesh::loadFromFile("cube");
	std::shared_ptr<ColorMesh> torusMesh = ColorMesh::loadFromFile("torus");
	std::shared_ptr<ColorMesh> sphereMesh = ColorMesh::loadFromFile("smoothSphere");
	std::shared_ptr<ColorMesh> smoothTorusMesh = ColorMesh::loadFromFile("smoothTorus");
	std::shared_ptr<ColorMesh> arbitMesh = ColorMesh::loadFromFile("random");


	std::shared_ptr<GameObject> stage(new GameObject);

	//makeBunchOfStuff(stage, cubeMesh, colorShader, 25);
	//makeBunchOfStuff(stage, sphereMesh, colorShader, 25);
	//makeBunchOfStuff(stage, smoothTorusMesh, colorShader, 25);
	/*
	Physics physicsSim;
	physicsSim.setGravity(glm::vec3(0, -6, 0));
	physicsSim.setBounceBoundary(glm::vec3(15));

	glm::vec3 maxBound(12,30,12);
	glm::vec3 minBound(-12, 2, -12);
	glm::vec3 difBound = maxBound - minBound;*/

	std::shared_ptr<GameObjectColor> floor = std::shared_ptr<GameObjectColor>(new GameObjectColor);
	floor->transform.setPosition(0,-6,0);
	floor->shader = colorShader;
	floor->mesh = arbitMesh;
	stage->addChild(floor);

	std::shared_ptr<TempSoup> testStup = std::shared_ptr<TempSoup>(new TempSoup);

	testStup->create(stage, glm::mat4(1.0));

	std::shared_ptr<ColorMesh> nStage = ColorMesh::meshFromTriangles(testStup->triangles, 100, 100, 100, 0.2);
	std::shared_ptr<GameObjectColor> nFloor = std::shared_ptr<GameObjectColor>(new GameObjectColor);
	nFloor->transform.setPosition(0, 0, 0);
	nFloor->shader = colorShader;
	nFloor->mesh = nStage;
	stage->addChild(nFloor);

	std::shared_ptr<Camera> camera(new Camera(90.0f, IO.aspectRatio, 0.1f, 100.0f));
	
	Toggle mouseLockToggle;
	Toggle shootToggle;

	float dt = 1.0 / 30.0f;

	std::vector<std::shared_ptr<GameObject>> physicsList;

	do

	{
		if (mouseLockToggle.toggle(IO.isKeyPressed(GLFW_KEY_2)))
		{
			IO.toggleMouseState();
		}

		if (shootToggle.toggle(IO.isMouseDown(GLFW_MOUSE_BUTTON_1)))
		{
			//spawn a fancy ball.
			int r = rand() % 255;
			int g = rand() % 255;
			int b = rand() % 255;

			std::shared_ptr<GameObjectColor> ball = Primitive::makeSphere(0.2, colorShader, r, g, b, 3.0);
			ball->velocity = 10.0f * camera->getTransformedZ();
			ball->transform.setPosition(camera->getPosition());
			stage->addChild(ball);
			physicsList.push_back(ball);
		}


		for (std::shared_ptr<GameObject> object : physicsList)
		{
			glm::vec3 nP;
			glm::vec3 thisP = object->transform.getPosition();
			glm::vec3 nextP = thisP + dt * object->velocity;
			testStup->collide(0.0, thisP, nextP, &nP);

			object->transform.setPosition(nP);

		}


		move(IO, camera);

		windowShader->setViewMatrix(camera->getTransformMatrix());
		IO.display(camera, stage);
	} while (IO.isWindowOpen());
		
	return 0;
}
