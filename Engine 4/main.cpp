#include "Header.h"
#include "IOManager.h"
//#include "WindowShader.h"
#include "Shader.h"
#include "Toggle.h"
#include "Primitives.h"
#include "Physics.h"
#include "Octree.h"
#include "GrapplingHook.h"
#include "SkyBox.h"
#include "Portal.h"
#include "Network.h"
#include "FileReader.h"

void checkGLError(std::string tag)
{
	/*GLenum err;
	while ((err = glGetError())) {
		std::cout << "error : " << err << "\n";
	}*/

	GLenum err;
	while ((err = glGetError())) {
		std::cout << tag << " : error : " << (err) << "\n";
	}

}

glm::vec3 cameraRotation;
glm::quat cameraOrientation = glm::quat(glm::radians(glm::vec3(0,0,0)));
bool cameraMode = true;
Toggle movemodeToggle;
float playerFriction = 0.8;
float playerElasticity = 0.0;
float playerRadius = 0.2;
float slipRange = 1.0; // this is the radius of uncertainty the network players can be in before it snaps
float fov = 120.0;

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
		/*double deltaPitch = IO.deltaMouseY * mouseSensitivity;
		double deltaPivot = -IO.deltaMouseX * mouseSensitivity;
		cameraRotation.x += deltaPitch;
		cameraRotation.y += deltaPivot;
		if (cameraRotation.x > 89.9) cameraRotation.x = 89.9;
		if (cameraRotation.x < -89.9) cameraRotation.x = -89.9;
		*/
		float framePitch = IO.deltaMouseY * mouseSensitivity;
		float frameYaw = -IO.deltaMouseX * mouseSensitivity;
		cameraOrientation = framePitch * cameraOrientation * frameYaw;
	}

	player->transform.setRotation(cameraOrientation);
	
	movemodeToggle.toggle(IO.isKeyPressed(GLFW_KEY_V));

	if (movemodeToggle.getState())
	{
		//fly
		glm::vec3 directionForward = Transform::getTransformedZ(cameraOrientation);
		glm::vec3 directionStrafe = Transform::getTransformedX(cameraOrientation);
		glm::vec3 directionVertical = Transform::getTransformedY(cameraOrientation);
		//glm::vec3 directionForward = Transform::getTransformedZ(cameraRotation);
		//glm::vec3 directionStrafe = Transform::getTransformedX(cameraRotation);
		//glm::vec3 directionVertical = Transform::getTransformedY(cameraRotation);
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

	float speed = 10.0;
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

	double mouseSensitivity = 0.4;


	if (IO.getMouseLockState()) {
		
		/*double deltaPitch = IO.deltaMouseY * mouseSensitivity;
		double deltaPivot = -IO.deltaMouseX * mouseSensitivity;
		cameraRotation.x += deltaPitch;
		cameraRotation.y += deltaPivot;
		if (cameraRotation.x > 89.9) cameraRotation.x = 89.9;
		if (cameraRotation.x < -89.9) cameraRotation.x = -89.9;
		*/

		float pitch = IO.deltaMouseY * mouseSensitivity;
		float yaw = -IO.deltaMouseX * mouseSensitivity;
		if (cameraMode)
		{
			//euler
			cameraRotation = glm::degrees(glm::eulerAngles(cameraOrientation));
			if (std::fabs(cameraRotation.z) >= 90) {
				cameraRotation.x += 180.f;
				cameraRotation.y = 180.f - cameraRotation.y;
				cameraRotation.z += 180.f;
			}
		
			if(fabs(cameraRotation.z) < 10.0)
				cameraRotation.z = 0.0;
			if (fabs(cameraRotation.z - 360) < 10.0)
				cameraRotation.z = 360.0;
			cameraRotation.x += pitch;
			cameraRotation.y += yaw;

			//clamping the angles...
			if (cameraRotation.x > 180)
			{
				if (cameraRotation.x < 271) cameraRotation.x = 271;
			}
			else
			{
				if (cameraRotation.x > 89.9) cameraRotation.x = 89.9;
				if (cameraRotation.x < -89.9) cameraRotation.x = -89.9;
			}
			cameraOrientation = glm::quat(glm::radians(cameraRotation));
		}
		else
		{
			//quats
			glm::quat framePitch = glm::quat(glm::radians(glm::vec3(pitch, 0, 0)));
			glm::quat frameYaw = glm::quat(glm::radians(glm::vec3(0, yaw, 0)));
			cameraOrientation = frameYaw * cameraOrientation * framePitch;
		}
		
		/*float cPitch = glm::degrees(glm::eulerAngles(cameraOrientation)).x;
		if (cPitch > 89)
		{
			framePitch = glm::quat(glm::radians(glm::vec3(89 - cPitch, 0, 0)));
			cameraOrientation = cameraOrientation * framePitch;
		}
		if (cPitch < -89)
		{
			framePitch = glm::quat(glm::radians(glm::vec3(-89 - cPitch, 0, 0)));
			cameraOrientation = cameraOrientation * framePitch;
		}*/
	}

	player->transform.setRotation(cameraOrientation);

	movemodeToggle.toggle(IO.isKeyPressed(GLFW_KEY_V));

	if (movemodeToggle.getState())
	{
		//fly
		glm::vec3 directionForward = Transform::getTransformedZ(cameraOrientation);
		glm::vec3 directionStrafe = Transform::getTransformedX(cameraOrientation);
		glm::vec3 directionVertical = Transform::getTransformedY(cameraOrientation);
		//glm::vec3 directionForward = player->transform.getTransformedZ();
		//glm::vec3 directionStrafe = player->transform.getTransformedX();
		//glm::vec3 directionVertical = player->transform.getTransformedY();
		glm::vec3 vel = xSpeed * directionStrafe + zSpeed * directionForward + ySpeed * directionVertical;

		float speedModifier = slow ? 0.5f : 1.0f;

		//std::cout << "fly\n";

		//player->velocity = glm::vec3(0);
		player->velocity = glm::vec3(0);
		return speedModifier * vel;

	}
	else
	{
		//walk
		//std::cout << "walk\n";
		glm::vec3 directionForward = Transform::getTransformedZ(cameraOrientation);
		glm::vec3 directionStrafe = Transform::getTransformedX(cameraOrientation);
		glm::vec3 directionVertical = Transform::getTransformedY(cameraOrientation);
		glm::vec3 vel = xSpeed * directionStrafe + zSpeed * directionForward;// +ySpeed * directionVertical;
		
		//std::cout << vel.x << " : " << vel.y << " : " << vel.z << "\n";
		float speed = glm::length(vel);
		if(speed > 0)
			vel = speed * glm::normalize(glm::vec3(vel.x, 0, vel.z));

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
	if (!object->physiceEnabled && !object->neverDisable) return;
	glm::vec3 thisP = object->transform.getPosition();
	glm::vec3 nextP = thisP + difference;

	glm::vec3 faceNorm;
	
	glm::quat newRot = object->transform.getRotationQuat();//cameraOrientation;
	bool didTeleport = false;
	for (std::shared_ptr<Portal> portal : Portal::portalList)
	{
		portal->action(object, difference, &didTeleport, &thisP, &nextP, &newRot);
	}
	object->transform.setRotation(newRot);
	glm::vec3 nP = nextP;

	if (!object->noClip && collider->collide(object->radius, thisP, nextP, &nP, &faceNorm, 4))
	{
		if (object->collisionReactEnabled)
			object->velocity = collisionResolution(object->velocity, faceNorm, object->friction, object->elasticity);

		//need to collide to stop.
		if (glm::length(nP - thisP) < GameObject::stopSpeed)
		{
			object->physiceEnabled = false;
		}
	}

	object->transform.setPosition(nP);
}

int main()
{
	srand(0);
	IOManager IO;
	//1280,800
	int WIDTH = 1000;
	int HEIGHT = 800;
	IO.createWindow(WIDTH, HEIGHT, "test", 30);
	IO.setClearColor(0, 0, 0);
	Primitive::init();

	std::map<std::string, std::shared_ptr<Shader>> shaderCollection = FileReader::readShaderFile("shaders.txt");
	std::map<std::string, std::shared_ptr<Mesh>> meshCollection = FileReader::readMeshFile("meshes.txt");
	std::map<std::string, std::shared_ptr<Texture>> textureCollection = FileReader::readTextureFile("textures.txt");

	std::static_pointer_cast<WindowShader>(shaderCollection["window"])->setGlobalLight(glm::normalize(glm::vec3(-1, 1, -1)));
	std::static_pointer_cast<WindowShader>(shaderCollection["window"])->setAmbient(1.0);
	IO.setWindowShader(std::static_pointer_cast<WindowShader>(shaderCollection["window"]));

	Portal::setInternalShader(std::static_pointer_cast<PortalShader>(shaderCollection["portalInternal"]));
	Portal::setDebugShader(std::static_pointer_cast<Shader>(shaderCollection["debug"]));

	std::shared_ptr<GameObject> stage(new GameObject);
	
	std::shared_ptr<SkyBox> skyBox = std::shared_ptr<SkyBox>(new SkyBox());
	skyBox->shader = std::static_pointer_cast<SkyBoxShader>(shaderCollection["skybox"]);
	skyBox->transform.setRotation(180, 0, 0);
	skyBox->texture = std::static_pointer_cast<SkyBoxTexture>(textureCollection["blueSky"]);;
	skyBox->transform.setScale(0.1, 0.1, 0.1);
	stage->addChild(skyBox);

	std::shared_ptr<GameObjectTexture> floor = std::shared_ptr<GameObjectTexture>(new GameObjectTexture);
	floor->transform.setPosition(0, 0, 0);
	floor->shader = std::static_pointer_cast<TextureShader>(shaderCollection["texture"]);//textureShader;//textureShader autoTextureShader
	floor->texture = textureCollection["yellowRoom"];//t_BakedRender; cargoHauler
	floor->mesh = meshCollection["yellowRoom"];//bakedMesh; baked cargoHauler
	stage->addChild(floor);

//	std::shared_ptr<TempSoup> testStup = std::shared_ptr<TempSoup>(new TempSoup);
	std::shared_ptr<TempSoup> tree = std::shared_ptr<TempSoup>(new TempSoup);
//	std::shared_ptr<Octree> tree = std::shared_ptr<Octree>(new Octree);
	tree->create(stage, glm::mat4(1.0));
	std::shared_ptr<TempSoup> empty = std::shared_ptr<TempSoup>(new TempSoup);
	empty->create(NULL, glm::mat4(1.0));

	//std::shared_ptr<ColorMesh> nStage = ColorMesh::meshFromTriangles(tree->triangles, 100, 100, 100, 0.1);
	//std::shared_ptr<GameObjectColor> nFloor = std::shared_ptr<GameObjectColor>(new GameObjectColor);
	//nFloor->transform.setPosition(0, 0, 0);
	//nFloor->shader = colorShader; 
	//nFloor->mesh = nStage;
	//stage->addChild(nFloor);

	std::shared_ptr<Camera> camera(new Camera(fov, IO.aspectRatio, 0.1f, 100.0f));
	
	Toggle mouseLockToggle;
	Toggle shootToggle;
	Toggle physicsToggle;
	Toggle toggleA;
	Toggle noClip;
	float shootDelay = 1.0f / 10.0f;
	float reloadTime = 0.0f;
	
	float dt = 1.0 / 30.0f;

	std::shared_ptr<GameObjectColor> player = std::shared_ptr<GameObjectColor>(new GameObjectColor);
	player->transform.setPosition(0, 0.2, 0);
	stage->addChild(player);

	std::shared_ptr<GameObject> hand = std::shared_ptr<GameObject>(new GameObject);
	glm::vec3 handOffset(0, -0.1, 0);
	hand->transform.setPosition(handOffset);
	player->addChild(hand);
	player->friction = playerFriction;
	player->elasticity = playerElasticity;
	player->radius = playerRadius;
	player->neverDisable = true;
	player->collisionReactEnabled = true;
		
	std::vector<std::shared_ptr<GameObject>> physicsList;
	physicsList.push_back(player);
	float maxSpeed = 40.0;

	glm::vec3 gravity = 30.0f * glm::vec3(0, -1, 0);

	//Portal::loadPortalList(stage, WIDTH, HEIGHT, portalShader, portalMesh, "portals.txt");
	Portal::loadPortalList(stage, WIDTH, HEIGHT, std::static_pointer_cast<PortalShader>(shaderCollection["portal"]), std::static_pointer_cast<ColorMesh>(meshCollection["portal"]), "portals.txt");

	std::shared_ptr<Server> network;
	if (network = std::static_pointer_cast<Server>(Network::makeNetwork(Network::NETWORK_TYPE::SERVER, "addressServer.txt")))
	{
		network->start();
		//sleeps until the server starts...
		while (!network->connectionValid) { Sleep(10); };
	}

	std::shared_ptr<Client> networkClient = std::static_pointer_cast<Client>(Network::makeNetwork(Network::NETWORK_TYPE::CLIENT, "addressClient.txt"));
	networkClient->start();

	checkGLError("setup");

	do
	{
		dt = IO.deltaTime;
		if (mouseLockToggle.toggle(IO.isKeyPressed(GLFW_KEY_2)))
		{
			IO.toggleMouseState();
		}

		//if (shootToggle.toggle(IO.isMouseDown(GLFW_MOUSE_BUTTON_1)))
		shootToggle.toggle(IO.isMouseDown(GLFW_KEY_B));
		if(shootToggle.getState() && IO.isMouseDown(GLFW_MOUSE_BUTTON_1) && shootDelay <= reloadTime)
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

				std::shared_ptr<GameObjectColor> ball = Primitive::makeSphere(0.1, std::static_pointer_cast<ColorShader>(shaderCollection["color"]), r, g, b, 3.0);
				
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
			player->transform.setPosition(0, 0.2, 0);
			player->velocity = glm::vec3(0);
		}

		physicsToggle.toggle(IO.isKeyPressed(GLFW_KEY_P));
		player->noClip = physicsToggle.getState();


		//now move character based on input.
		glm::vec3 controlVelocity = getControllerVelocity(IO, player);
		
		float vy = player->velocity.y;
		player->velocity = controlVelocity;
		camera->setRotation(cameraOrientation);
		if (movemodeToggle.getState())
		{
			player->velocity -= gravity * dt;
		}
		else
		{
			player->velocity.y = vy;
		}

		//Network:
		if (toggleA.toggle(IO.isKeyPressed(GLFW_KEY_M)))
		{
			std::string msg = "heyo ";
			msg += std::to_string(IO.deltaTime);
			networkClient->msgBuffer.push(msg);
		}
		while (networkClient->clientsToAdd.size())
		{
			int r = rand() % 155 + 100;
			int g = rand() % 155 + 100;
			int b = rand() % 155 + 100;
			std::shared_ptr<GameObjectColor> nPlayer = Primitive::makeSphere(playerRadius, std::static_pointer_cast<ColorShader>(shaderCollection["color"]), r, g, b, 3.0);
			nPlayer->transform.setPosition(0, 0, 0);
			stage->addChild(nPlayer);
			nPlayer->friction = playerFriction;
			nPlayer->elasticity = playerElasticity;
			nPlayer->radius = playerRadius;
			nPlayer->neverDisable = true;
			nPlayer->collisionReactEnabled = true;
			nPlayer->gravityAffected = false;
			physicsList.push_back(nPlayer);
			networkClient->clientsToAdd.front()->clientObject = nPlayer;
			networkClient->clientsToAdd.pop();
			std::cout << "NEW PLAYER!!!!!\n";
		}
		for (auto const& clientPair : networkClient->clientMap)
		{
			//read the pos/vel
			/* SNAP UPDATE PARADIGM
			if (clientPair.second)
			{
				if (!clientPair.second->updateMutex)
				{
					clientPair.second->updateMutex = true;
					if (clientPair.second->posQue.size() > 0)
					{
						glm::vec3 posInt;
						while (clientPair.second->posQue.size() > 0)
						{
							posInt = clientPair.second->posQue.front();
							clientPair.second->posQue.pop();
						}
						clientPair.second->clientObject->transform.setPosition(posInt);
					}
					if (clientPair.second->velQue.size() > 0)
					{
						glm::vec3 velInt;
						while (clientPair.second->velQue.size() > 0)
						{
							velInt = clientPair.second->velQue.front();
							clientPair.second->velQue.pop();
							clientPair.second->clientObject->velocity = velInt;
						}
					}
					clientPair.second->updateMutex = false;
				}
			}
			*/
			// Dead reckon paradigm
			// set velocity + lerp (with physics...).
			if (clientPair.second)
			{
				if (!clientPair.second->updateMutex)
				{
					clientPair.second->updateMutex = true;
					if (clientPair.second->posQue.size() > 0)
					{
						glm::vec3 posInt;
						while (clientPair.second->posQue.size() > 0)
						{
							posInt = clientPair.second->posQue.front();
							clientPair.second->posQue.pop();
						}
						//check the snap
						float slip = glm::length(posInt - clientPair.second->clientObject->transform.getPosition());
						if (slip > slipRange)
						{
							clientPair.second->clientObject->transform.setPosition(posInt);
						}
						//clientPair.second->clientObject->transform.setPosition(posInt);
					}
					if (clientPair.second->velQue.size() > 0)
					{
						glm::vec3 velInt;
						while (clientPair.second->velQue.size() > 0)
						{
							velInt = clientPair.second->velQue.front();
							clientPair.second->velQue.pop();
							clientPair.second->clientObject->velocity = velInt;
						}
					}
					clientPair.second->updateMutex = false;
				}
			}
		}
		
		//apply physics to everything
		for (std::shared_ptr<GameObject> object : physicsList)
		{
			if(object->gravityAffected)
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
		
		cameraOrientation = player->transform.getRotationQuat();
		camera->setRotation(cameraOrientation);
		camera->setPosition(player->transform.getPosition());// +glm::vec3(0, 1, 0));
		hand->transform.setRotation(cameraRotation);
		

		networkClient->updateNetworkPosVel(player->transform.getPosition(), player->velocity);
		Portal::preRenderPortals(camera, 2);
		std::static_pointer_cast<WindowShader>(shaderCollection["window"])->setViewMatrix(camera->getTransformMatrix());
		IO.display(camera, stage);
		checkGLError("in");
	} while (IO.isWindowOpen());
		
	return 0;
}
