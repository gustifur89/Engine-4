#include "Header.h"
#include "IOManager.h"
#include "WindowShader.h"
#include "Toggle.h"
#include "Primitives.h"
#include "Physics.h"
#include "Octree.h"
#include "GrapplingHook.h"

glm::vec3 cameraRotation;
Toggle movemodeToggle;
void move(IOManager& IO, std::shared_ptr<GameObject> player)
{
	bool w = IO.isKeyPressed(GLFW_KEY_W);
	bool a = IO.isKeyPressed(GLFW_KEY_A);
	bool s = IO.isKeyPressed(GLFW_KEY_S);
	bool d = IO.isKeyPressed(GLFW_KEY_D);
	bool e = IO.isKeyPressed(GLFW_KEY_SPACE);
	bool q = IO.isKeyPressed(GLFW_KEY_LEFT_CONTROL);

	bool slow = IO.isKeyPressed(GLFW_KEY_Q);

	float speed = 20.0;
	float jumpSpeed = 8.0;

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

	//player->transform.setRotation(cameraRotation);
	
	movemodeToggle.toggle(IO.isKeyPressed(GLFW_KEY_V));

	if (movemodeToggle.getState())
	{
		//fly
		glm::vec3 directionForward = Transform::getTransformedZ(cameraRotation);
		glm::vec3 directionStrafe = Transform::getTransformedX(cameraRotation);
		glm::vec3 directionVertical = Transform::getTransformedY(cameraRotation);
		//glm::vec3 directionForward = player->transform.getTransformedZ();
		//glm::vec3 directionStrafe = player->transform.getTransformedX();
		//glm::vec3 directionVertical = player->transform.getTransformedY();
		glm::vec3 vel = xSpeed * directionStrafe + zSpeed * directionForward + ySpeed * directionVertical;

		float speedModifier = slow ? 0.5f : 1.0f;

		player->velocity = speedModifier * vel;

	}
	else
	{
		//walk
		
		float velZ = cos(cameraRotation.y * TO_RAD) * zSpeed - sin(cameraRotation.y * TO_RAD) * xSpeed;
		float velX = sin(cameraRotation.y * TO_RAD) * zSpeed + cos(cameraRotation.y * TO_RAD) * xSpeed;

		glm::vec3 vel(velX, player->velocity.y, velZ);

		glm::vec3 acceleration(velZ, 0, velX);

		float speedModifier = slow ? 0.5f : 1.0f;

		if (e)
		{
			vel.y = jumpSpeed;
		}
		if (q)
		{
			vel.y = -jumpSpeed;
		}
		


		player->velocity = speedModifier* vel;
		
	}
	
	//player->transform.translate(speedModifier * IO.deltaTime * vel);
}

glm::vec3 getControllerVelocity(IOManager& IO, std::shared_ptr<GameObject> player)
{
	bool w = IO.isKeyPressed(GLFW_KEY_W);
	bool a = IO.isKeyPressed(GLFW_KEY_A);
	bool s = IO.isKeyPressed(GLFW_KEY_S);
	bool d = IO.isKeyPressed(GLFW_KEY_D);
	bool e = IO.isKeyPressed(GLFW_KEY_SPACE);
	bool q = IO.isKeyPressed(GLFW_KEY_LEFT_CONTROL);

	bool slow = IO.isKeyPressed(GLFW_KEY_Q);

	float speed = 20.0;
	float jumpSpeed = 8.0;

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

	//player->transform.setRotation(cameraRotation);

	movemodeToggle.toggle(IO.isKeyPressed(GLFW_KEY_V));

	if (movemodeToggle.getState())
	{
		//fly
		glm::vec3 directionForward = Transform::getTransformedZ(cameraRotation);
		glm::vec3 directionStrafe = Transform::getTransformedX(cameraRotation);
		glm::vec3 directionVertical = Transform::getTransformedY(cameraRotation);
		//glm::vec3 directionForward = player->transform.getTransformedZ();
		//glm::vec3 directionStrafe = player->transform.getTransformedX();
		//glm::vec3 directionVertical = player->transform.getTransformedY();
		glm::vec3 vel = xSpeed * directionStrafe + zSpeed * directionForward + ySpeed * directionVertical;

		float speedModifier = slow ? 0.5f : 1.0f;

		player->velocity = glm::vec3(0);

		return speedModifier * vel;

	}
	else
	{
		//walk

		float velZ = cos(cameraRotation.y * TO_RAD) * zSpeed - sin(cameraRotation.y * TO_RAD) * xSpeed;
		float velX = sin(cameraRotation.y * TO_RAD) * zSpeed + cos(cameraRotation.y * TO_RAD) * xSpeed;

		glm::vec3 vel(velX, player->velocity.y, velZ);

		glm::vec3 acceleration(velZ, 0, velX);

		float speedModifier = slow ? 0.5f : 1.0f;

		if (e)
		{
			//vel.y = jumpSpeed;
			player->velocity.y = jumpSpeed; // this works with physics..
		}
		if (q)
		{
			//vel.y = -jumpSpeed;
			player->velocity.y = -jumpSpeed; // this works with physics..
		}

		return speedModifier * vel;
	}
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

glm::vec3 collisionResolution(glm::vec3 velocity, glm::vec3 normal, float friction, float elasticity)
{
	glm::vec3 newVelocity = velocity;
	glm::vec3 velocityAlong = normal * glm::dot(normal, velocity);
	
	//remove the perpendicular component
	newVelocity -= velocityAlong;// *(1 + elasticity);
	//apply friction to parallel part

	newVelocity *= (1.0f - friction);
	if (glm::length(newVelocity) < 0.001)
		newVelocity = glm::vec3(0.0);
	
	//re-add velocity for elasticity
	newVelocity -= elasticity * velocityAlong;

	return newVelocity;
}

void tryMoveObject(std::shared_ptr<GameObject> object, glm::vec3 difference, std::shared_ptr<CollisionStructure> collider)
{
	glm::vec3 thisP = object->transform.getPosition();
	glm::vec3 nextP = thisP + difference;

	glm::vec3 faceNorm;
	glm::vec3 nP = nextP;
	if (collider->collide(object->radius, thisP, nextP, &nP, &faceNorm, 6))
	{
		if (object->collisionReactEnabled)
			object->velocity = collisionResolution(object->velocity, faceNorm, object->friction, object->elasticity);
	}

	object->transform.setPosition(nP);
}

int main()
{
	srand(0);
	IOManager IO;
	//1280,800
	IO.createWindow(800, 600, "test", 30);
	IO.setClearColor(200, 60, 100);// pink
	//IO.setClearColor(214, 252, 255);
	std::shared_ptr<WindowShader> windowShader = WindowShader::loadShader("screen", "window");
	windowShader->setGlobalLight(glm::normalize(glm::vec3(-1, 1, -1)));
	windowShader->setAmbient(1.0);

	Primitive::init();

	IO.setWindowShader(windowShader);

	std::shared_ptr<ColorShader> colorShader = ColorShader::loadShader("color");

	std::shared_ptr<ColorMesh> cubeMesh = ColorMesh::loadFromFile("cube");
	std::shared_ptr<ColorMesh> torusMesh = ColorMesh::loadFromFile("torus");
	std::shared_ptr<ColorMesh> sphereMesh = ColorMesh::loadFromFile("smoothSphere");
	std::shared_ptr<ColorMesh> smoothTorusMesh = ColorMesh::loadFromFile("smoothTorus");
	std::shared_ptr<ColorMesh> arbitMesh = ColorMesh::loadFromFile("testGround"); //random testGround
	std::shared_ptr<ColorMesh> trisMesh = ColorMesh::loadFromFile("tris");
	std::shared_ptr<ColorMesh> grapplingHookMesh = ColorMesh::loadFromFile("GrapplingHook");
	std::shared_ptr<ColorMesh> grappleRopeMesh = ColorMesh::loadFromFile("GrappleRope");

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
	floor->transform.setPosition(0,0,0);
	floor->shader = colorShader;
	floor->mesh = arbitMesh;
	stage->addChild(floor);
	
	//std::shared_ptr<TempSoup> testStup = std::shared_ptr<TempSoup>(new TempSoup);
	//std::shared_ptr<TempSoup> tree = std::shared_ptr<TempSoup>(new TempSoup);
	std::shared_ptr<Octree> tree = std::shared_ptr<Octree>(new Octree);
	tree->create(stage, glm::mat4(1.0));


	std::shared_ptr<ColorMesh> nStage = ColorMesh::meshFromTriangles(tree->triangles, 100, 100, 100, 0.1);
	std::shared_ptr<GameObjectColor> nFloor = std::shared_ptr<GameObjectColor>(new GameObjectColor);
	nFloor->transform.setPosition(0, 0, 0);
	nFloor->shader = colorShader; 
	nFloor->mesh = nStage;
	//stage->addChild(nFloor);

	std::shared_ptr<Camera> camera(new Camera(90.0f, IO.aspectRatio, 0.1f, 100.0f));
	
	Toggle mouseLockToggle;
	Toggle shootToggle;
	Toggle physicsToggle;
	float shootDelay = 1.0f / 10.0f;
	float reloadTime = 0.0f;
	
	float dt = 1.0 / 30.0f;

	std::shared_ptr<GameObjectColor> player = std::shared_ptr<GameObjectColor>(new GameObjectColor);
	player->transform.setPosition(0, 6, 0);
	stage->addChild(player);

	std::shared_ptr<GameObject> hand = std::shared_ptr<GameObject>(new GameObject);
	glm::vec3 handOffset(0, -0.1, 0);
	hand->transform.setPosition(handOffset);
	player->addChild(hand);
	player->friction = 0.8;
	player->elasticity = 0.0;
	player->radius = 0.6;
	player->collisionReactEnabled = false;

	std::shared_ptr<GameObjectColor> grapplingHook = std::shared_ptr<GameObjectColor>(new GameObjectColor);
	grapplingHook->shader = colorShader;
	grapplingHook->mesh = grapplingHookMesh;
	grapplingHook->transform.setRotation(0, 182, 0);
	grapplingHook->transform.setPosition(-0.6, 0.0, 0.0);
	hand->addChild(grapplingHook);

	std::shared_ptr<GrapplingHook> grapple = std::shared_ptr<GrapplingHook>(new GrapplingHook(100.0f, 0.0f, 30.0f));
	grapple->mesh = grappleRopeMesh;
	grapple->shader = colorShader;
	stage->addChild(grapple);
		
	std::vector<std::shared_ptr<GameObject>> physicsList;
	physicsList.push_back(player);
	float maxSpeed = 40.0;

	glm::vec3 gravity = 30.0f * glm::vec3(0, -1, 0);

	do
	{
		if (mouseLockToggle.toggle(IO.isKeyPressed(GLFW_KEY_2)))
		{
			IO.toggleMouseState();
		}

		//if (shootToggle.toggle(IO.isMouseDown(GLFW_MOUSE_BUTTON_1)))
		if(IO.isMouseDown(GLFW_MOUSE_BUTTON_1) && shootDelay <= reloadTime)
		{
			reloadTime = 0.0;
			//spawn a fancy ball.
			for (int i = 0; i < 10; i++)
			{
				int r = rand() % 255;
				int g = rand() % 255;
				int b = rand() % 255;

				float xa = ((rand() % 10000) / 10000.0 - 0.5);
				float ya = ((rand() % 10000) / 10000.0 - 0.5);
				float za = ((rand() % 10000) / 10000.0 - 0.5);
				float angleSweep = 30.0f * TO_RAD;
				float speed = 6.0f * ((rand() % 10000) / 10000.0) + 8.0f;
				float elast = 0.66 * ((rand() % 10000) / 10000.0);

				std::shared_ptr<GameObjectColor> ball = Primitive::makeSphere(0.1, colorShader, r, g, b, 3.0);
				
				glm::vec3 angles = angleSweep * glm::vec3(xa, ya, za);

				glm::mat4 randomVariation = glm::mat4(glm::quat(angles));
				glm::mat4 rotMat = camera->getRotationMatrix();
				glm::vec3 vel = speed * rotMat * randomVariation * glm::vec4(0, 0, 1, 0);

				ball->friction = 0.1;
				ball->elasticity = elast;
				ball->velocity = vel;// 10.0f * camera->getTransformedZ();
				ball->transform.setPosition(camera->getPosition());
				stage->addChild(ball);
				physicsList.push_back(ball);
			}
		}
		else
		{
			reloadTime += dt;
		}
		
		if (IO.isKeyPressed(GLFW_KEY_8))
		{
			player->transform.setPosition(0, 6, 0);
			player->velocity = glm::vec3(0);
		}

		physicsToggle.toggle(IO.isKeyPressed(GLFW_KEY_6));

		//do the player grapple:  (will use physics, so we do it up here.)
		glm::vec3 grappleForce = grapple->action(IO.isMouseDown(GLFW_MOUSE_BUTTON_2), tree, player->transform.getPosition() + handOffset, Transform::getTransformedZ(cameraRotation));
		player->velocity += grappleForce * dt;
		
		//now move character based on input.
		glm::vec3 contolVelocity = getControllerVelocity(IO, player);
		tryMoveObject(player, dt * getControllerVelocity(IO, player), tree);

		//apply physics to everything
		for (std::shared_ptr<GameObject> object : physicsList)
		{
			object->velocity += gravity * dt;

			if (glm::length(object->velocity) > maxSpeed)
			{
				object->velocity = maxSpeed * glm::normalize(object->velocity);
			}
				
			tryMoveObject(object, dt * object->velocity, tree);

			/*
			if (glm::length(object->velocity) > 0.0)
			{
				glm::vec3 nP;
				glm::vec3 thisP = object->transform.getPosition();
				glm::vec3 nextP = thisP + dt * object->velocity;

				glm::vec3 faceNorm;
				nP = nextP;
				if (testStup->collide(object->radius, thisP, nextP, &nP, &faceNorm, 6))
				{
					if(object->collisionReactEnabled)
						object->velocity = collisionResolution(object->velocity, faceNorm, object->friction, object->elasticity);
				}

				object->transform.setPosition(nP);

			}
			*/
		}
		
		//camera adjust
		camera->setRotation(cameraRotation);
		camera->setPosition(player->transform.getPosition());// +glm::vec3(0, 1, 0));
		hand->transform.setRotation(cameraRotation);

		windowShader->setViewMatrix(camera->getTransformMatrix());
		IO.display(camera, stage);
	} while (IO.isWindowOpen());
		
	return 0;
}
