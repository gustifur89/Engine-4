#include "Header.h"
#include "IOManager.h"
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
float teleportRange = 3.0; // this is the radius of uncertainty the network players can be in before it snaps
float fov = 94.0;
float playerSpeed = 10.0;
float playerJump = 10.0;
float playerSensitivity = 0.4;
float playerRadius = 0.3;
float playerHeight = 1.6;
float eyeHeight = 1.32;
float crouchEyeHeight = 0.46;
float stepHieght = 0.15625;
float slipPercentPerSecond = 12.0;
float slipRotPercentPerSecond = 12.0;
float maxSpeed = 40.0;
std::string playerTex = "blankFigureTex";

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

void animatePlayer(std::shared_ptr<GameObject> player, std::shared_ptr<GameObject> playerMesh, float dt, std::string* state)
{
	//std::cout << player->onGround << "\n";
	if (!player->onGround)
	{
		//jump
		playerMesh->updateAnimation(dt, "jump");
		*state = "jump";
	}
	else if (glm::length(player->velocity) > 3.0)
	{
		//run
		playerMesh->updateAnimation(dt, "run");
		*state = "run";
	}
	else
	{
		//stand
		playerMesh->updateAnimation(dt, "stand");
		*state = "stand";
	}
	playerMesh->transform.setPosition(player->transform.getPosition());
	playerMesh->transform.setRotation(glm::vec3(0, player->transform.getRotation().y + 180, 0));

}

void setAnimation(std::shared_ptr<GameObject> player, std::shared_ptr<GameObject> playerMesh, float dt, std::string state)
{
	playerMesh->updateAnimation(dt, state);
	playerMesh->transform.setPosition(player->transform.getPosition());
	playerMesh->transform.setRotation(glm::vec3(0, player->transform.getRotation().y + 180, 0));
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
	bool run = IO.isKeyPressed(GLFW_KEY_LEFT_SHIFT);

	float speed = playerSpeed;
	float jumpSpeed = playerJump;

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
			//cameraRotation = glm::degrees(glm::eulerAngles(cameraOrientation));
			//if (std::fabs(cameraRotation.z) >= 90) {
			//	cameraRotation.x += 180.f;
			//	cameraRotation.y = 180.f - cameraRotation.y;
			//	cameraRotation.z += 180.f;
			//}
		
			//if(fabs(cameraRotation.z) < 10.0)
			//	cameraRotation.z = 0.0;
			//if (fabs(cameraRotation.z - 360) < 10.0)
			//	cameraRotation.z = 360.0;

			/*cameraRotation = glm::degrees(glm::eulerAngles(cameraOrientation));
			if (std::fabs(cameraRotation.z) >= 90) {
				cameraRotation.x += 180.f;
				cameraRotation.y = 180.f - cameraRotation.y;
				cameraRotation.z += 180.f;
			}*/

			cameraRotation.x += pitch;
			cameraRotation.y += yaw;

			//clamping the angles...
			if (cameraRotation.x > 89.9) cameraRotation.x = 89.9;
			if (cameraRotation.x < -89.9) cameraRotation.x = -89.9;
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

		float speedModifier = run ? 2.0f : 1.0f;

		player->velocity = glm::vec3(0);
		return speedModifier * vel;
	}
	else
	{
		//walk
		glm::vec3 directionForward = Transform::getTransformedZ(cameraOrientation);
		glm::vec3 directionStrafe = Transform::getTransformedX(cameraOrientation);
		glm::vec3 directionVertical = Transform::getTransformedY(cameraOrientation);
		glm::vec3 vel = xSpeed * directionStrafe + zSpeed * directionForward;
		
		float speed = glm::length(vel);
		if(speed > 0)
			vel = speed * glm::normalize(glm::vec3(vel.x, 0, vel.z));

		float speedModifier = run ? 2.0f : 1.0f;

		if (e && player->onGround)
		{
			//vel.y = jumpSpeed;
			player->velocity.y = jumpSpeed; // this works with physics..
		}
		if (q)
		{
			//vel.y = -jumpSpeed;
			//player->velocity.y = -jumpSpeed; // this works with physics..
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

void tryMoveObject(std::shared_ptr<GameObject> object, glm::vec3 difference, float dt, std::shared_ptr<CollisionStructure> collider)
{
	if (!object->physiceEnabled && !object->neverDisable) return;
	glm::vec3 thisP = object->transform.getPosition();
	glm::vec3 nextP = thisP + difference;

	//glm::vec3 faceNorm;
	std::vector<glm::vec3> faceNorms;
	
	glm::quat newRot = object->transform.getRotationQuat();//cameraOrientation;
	bool didTeleport = false;
	for (std::shared_ptr<Portal> portal : Portal::portalList)
	{
		portal->action(object, difference, &didTeleport, &thisP, &nextP, &newRot);
	}
	object->transform.setRotation(newRot);

	glm::vec3 nP = nextP;

	bool didHitGround = false;

	if (!object->noClip && collider->collide(object->collider, thisP, nextP, &nP, &faceNorms, 4))
	{
	//	std::cout << "IN SOLID?\n";

		if (object->collisionReactEnabled)
			object->velocity = collisionResolution(object->velocity, faceNorms[0], object->friction, object->elasticity);

		float upVal = faceNorms[0].y;

		if (faceNorms[0].y > 0.0)
		{
			object->onGround = true;
			didHitGround = true;
		}

		//need to collide to stop.
		if (glm::length(nP - thisP) < GameObject::stopSpeed && object->onGround)
		{
			object->physiceEnabled = false;
		}
	}

	if (!didHitGround)
	{
		//if it didn't hit ground.. update coyote time
		object->offgroundTime += dt;
		if (object->offgroundTime > GameObject::coyoteTime)
		{
			object->onGround = false;
		}
	}

	object->transform.setPosition(nP);
}

void tryMovePlayer(std::shared_ptr<GameObject> object, glm::vec3 difference, float dt, std::shared_ptr<CollisionStructure> collider, glm::vec3 gravity)
{
	//if (!object->physiceEnabled && !object->neverDisable) return;
	if (object->gravityAffected)
		object->velocity += gravity * dt;

	if (glm::length(object->velocity) > maxSpeed)
	{
		object->velocity = maxSpeed * glm::normalize(object->velocity);
	}

	glm::vec3 thisP = object->transform.getPosition();
	glm::vec3 nextP = thisP + difference;

	float xzStepDistance = glm::length(glm::vec2(difference.x, difference.z));

	std::vector<glm::vec3> faceNorms;

	glm::quat newRot = object->transform.getRotationQuat();//cameraOrientation;
	bool didTeleport = false;
	for (std::shared_ptr<Portal> portal : Portal::portalList)
	{
		portal->action(object, difference, &didTeleport, &thisP, &nextP, &newRot);
	}
	object->transform.setRotation(newRot);

	glm::vec3 nP = nextP;

	bool didHitGround = false;

	if (!object->noClip)
	{
		if (collider->collide(object->collider, thisP, nextP, &nP, &faceNorms, 4))
		{

			for (glm::vec3 norm : faceNorms)
			{
				if (norm.y > 0.0)
				{
					object->onGround = true;
					object->offgroundTime = 0.0;
					didHitGround = true;
				}

				if (abs(norm.y) > 0.1)
				{
					object->velocity.y = 0.0f;
				}
			}
			
			
			glm::vec3 step = nP - thisP;
			
			float xzDidStepDistance = glm::length(glm::vec2(step.x, step.z));
			
			

			if (xzDidStepDistance < xzStepDistance * 0.9 && object->onGround) // we consider hitting a wall if the step is 
			{
				//std::cout << "try step\n";
				//to step...
				// we try move up the step distance, try move forwards, then try move 
				//step up...
				glm::vec3 stepNextP = thisP + glm::vec3(0, stepHieght, 0);
				glm::vec3 stepUp = stepNextP;
				std::vector<glm::vec3> newNorm;

				//try step up...
				collider->collide(object->collider, thisP, stepNextP, &stepUp, &newNorm, 0);

				glm::vec3 stepFor = stepUp + difference;
				glm::vec3 n_step = stepFor;
				collider->collide(object->collider, stepUp, stepFor, &n_step, &newNorm, 4);
				//then try to step down..
				glm::vec3 stepDown = n_step - glm::vec3(0, stepHieght, 0);
				glm::vec3 finalStep = stepDown;

				collider->collide(object->collider, n_step, stepDown, &finalStep, &newNorm, 0);
				nP = finalStep;
			}
		}
	}

	//std::cout << object->onGround << "\n";
	//std::cout << object->offgroundTime << " : " << GameObject::coyoteTime << "\n";
	if (!didHitGround)
	{
		//if it didn't hit ground.. update coyote time
		object->offgroundTime += dt;
		if (object->offgroundTime > GameObject::coyoteTime)
		{
			object->onGround = false;
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
	IO.createWindow(WIDTH, HEIGHT, "test", 30.0);
	IO.setClearColor(0, 0, 0);
	Primitive::init();

	std::map<std::string, std::shared_ptr<Shader>> shaderCollection = FileReader::readShaderFile("shaders.txt");
	std::map<std::string, std::shared_ptr<Mesh>> meshCollection = FileReader::readMeshFile("meshes.txt");
	std::map<std::string, std::shared_ptr<Texture>> textureCollection = FileReader::readTextureFile("textures.txt");
	FileReader::setPlayerSettings("settings.txt", &fov, &playerSensitivity, &playerSpeed, &playerTex);


	std::static_pointer_cast<WindowShader>(shaderCollection["window"])->setGlobalLight(glm::normalize(glm::vec3(-1, 1, -1)));
	std::static_pointer_cast<WindowShader>(shaderCollection["window"])->setAmbient(1.0);
	IO.setWindowShader(std::static_pointer_cast<WindowShader>(shaderCollection["window"]));

	Portal::setInternalShader(std::static_pointer_cast<PortalShader>(shaderCollection["portalInternal"]));
	Portal::setDebugShader(std::static_pointer_cast<Shader>(shaderCollection["debug"]));

	std::shared_ptr<GameObject> stage(new GameObject);
	

	std::shared_ptr<GameObjectTexture> floor = std::shared_ptr<GameObjectTexture>(new GameObjectTexture);
	floor->transform.setPosition(0, 0, 0);
	floor->shader = std::static_pointer_cast<TextureShader>(shaderCollection["texture"]);//textureShader;//textureShader autoTextureShader
	floor->mesh = meshCollection["yellowRoom"]; // redRoom
	floor->texture = textureCollection["yellowRoom"];//redRoomHi redRoom
	//floor->mesh = meshCollection["redRoom"];//bakedMesh; baked cargoHauler yellowRoomObj yellowRoomObj yellowRoom
	//floor->texture = textureCollection["redRoom"];//t_BakedRender; cargoHauler yellowRoom
	//floor->multiMesh = true;
	//floor->assignMultiTextures(textureCollection);//textureCollection["shadowTex"];//t_BakedRender; cargoHauler yellowRoom
	floor->visible = true;
	stage->addChild(floor);

	std::shared_ptr<GameObjectSky> cover = std::shared_ptr<GameObjectSky>(new GameObjectSky);
	cover->transform.setPosition(0, 0, 0);
	cover->shader = std::static_pointer_cast<SkyTexShader>(shaderCollection["skyTex"]);
	cover->mesh = meshCollection["yellowRoomCover"];
	cover->texture = std::static_pointer_cast<SkyBoxTexture>(textureCollection["deepSpace"]);;
	cover->visible = true;
	stage->addChild(cover);

	std::shared_ptr<BSP> bspTest = std::shared_ptr<BSP>(new BSP);
	bspTest->create(stage, glm::mat4(1.0));
	meshCollection["bsp"] = bspTest->getMesh();

	int texSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);
	std::cout << texSize << "\n";

//	std::shared_ptr<TempSoup> testStup = std::shared_ptr<TempSoup>(new TempSoup);
	/*
	std::shared_ptr<TempSoup> tree = std::shared_ptr<TempSoup>(new TempSoup);
//	std::shared_ptr<Octree> tree = std::shared_ptr<Octree>(new Octree);
	tree->create(stage, glm::mat4(1.0));
	std::shared_ptr<TempSoup> empty = std::shared_ptr<TempSoup>(new TempSoup);
	empty->create(NULL, glm::mat4(1.0));
	*/

	/*//SKYBOX
	std::shared_ptr<SkyBox> skyBox = std::shared_ptr<SkyBox>(new SkyBox());
	skyBox->shader = std::static_pointer_cast<SkyBoxShader>(shaderCollection["skybox"]);
	skyBox->transform.setRotation(180, 0, 0);
	skyBox->texture = std::static_pointer_cast<SkyBoxTexture>(textureCollection["blueSky"]);;
	skyBox->transform.setScale(0.1, 0.1, 0.1);
	stage->addChild(skyBox);
	*/

	std::shared_ptr<GameObjectTexture> figure = std::shared_ptr<GameObjectTexture>(new GameObjectTexture);
	figure->transform.setPosition(-4, 0.3, -6.5);
	//figure->shader = std::static_pointer_cast<TextureShader>(shaderCollection["texture"]);
	//figure->texture = textureCollection["blankFigureTex"];
	//figure->mesh = meshCollection["figure"];
	//figure phsyics
	figure->transform.setRotation(glm::vec3(0, 90, 0));
	figure->friction = 0.0;// playerFriction;
	figure->elasticity = playerElasticity;
	figure->radius = playerRadius;
	figure->velocity = glm::vec3(0, 0, 0);
	figure->collisionReactEnabled = true;
	figure->neverDisable = true;
	figure->collider = std::shared_ptr<Cylinder>(new Cylinder(playerRadius, playerHeight));
	//stage->addChild(figure);
	std::shared_ptr<GameObjectTexture> figureMesh = std::shared_ptr<GameObjectTexture>(new GameObjectTexture);
	figureMesh->shader = std::static_pointer_cast<TextureShader>(shaderCollection["texture"]);
	figureMesh->texture = textureCollection["blankFigureTex"];
	figureMesh->mesh = meshCollection["figure"];
	stage->addChild(figureMesh);

	std::shared_ptr<Camera> camera(new Camera(fov, IO.aspectRatio, 0.1f, 100.0f));
	
	Toggle mouseLockToggle;
	Toggle shootToggle;
	Toggle physicsToggle;
	Toggle toggleA;
	Toggle noClip;
	float shootDelay = 1.0f / 10.0f;
	float reloadTime = 0.0f;
	
	float dt = 1.0 / 30.0f;

	std::shared_ptr<GameObjectTexture> player = std::shared_ptr<GameObjectTexture>(new GameObjectTexture);
	player->transform.setPosition(0, 6, -0.6);
	player->transform.setRotation(glm::vec3(0, 140, 0));
	//player->collider = std::shared_ptr<Sphere>(new Sphere(playerRadius));
	player->collider = std::shared_ptr<Cylinder>(new Cylinder(playerRadius, playerHeight));
	player->friction = playerFriction;
	player->elasticity = playerElasticity;
	player->radius = playerRadius;
	player->neverDisable = true;
	player->collisionReactEnabled = true;
	//stage->addChild(player);
	
	std::shared_ptr<GameObject> hand = std::shared_ptr<GameObject>(new GameObject);
	stage->addChild(hand);

	std::shared_ptr<GameObjectTexture> shotgun = std::shared_ptr<GameObjectTexture>(new GameObjectTexture);
	shotgun->transform.setScale(glm::vec3(0.4));
	shotgun->transform.setPosition(glm::vec3(-0.18, -0.18, 0.28));
	//shotgun->transform.setRotation(glm::vec3(0, 90, 0));
	shotgun->shader = std::static_pointer_cast<TextureShader>(shaderCollection["texture"]);
	shotgun->texture = textureCollection["shotgun"];
	shotgun->mesh = meshCollection["shotgun"];
	hand->addChild(shotgun);
	//stage->addChild(shotgun);

	//player mesh
	std::shared_ptr<GameObjectTexture> playerMesh = std::shared_ptr<GameObjectTexture>(new GameObjectTexture);
	playerMesh->shader = std::static_pointer_cast<TextureShader>(shaderCollection["texture"]);
	playerMesh->texture = textureCollection[playerTex];//blankFigureTex
	playerMesh->mesh = meshCollection["figure"];
	stage->addChild(playerMesh);
	   		
	std::vector<std::shared_ptr<GameObject>> physicsList;
	//physicsList.push_back(player);
	//physicsList.push_back(figure);

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
	
	std::shared_ptr<AnimationData> animRun = AnimationData::loadFromFilePLY("figure2_run", 16);
	std::shared_ptr<AnimationData> animStand = AnimationData::loadFromFilePLY("figure2_stand", 1);
	std::shared_ptr<AnimationData> animJump = AnimationData::loadFromFilePLY("figure2_jump", 1);
	std::map<std::string, std::shared_ptr<AnimationData>> animationTest;
	animationTest["run"] = animRun;
	animationTest["stand"] = animStand;
	animationTest["jump"] = animJump;
	figureMesh->animations = animationTest;
	playerMesh->animations = animationTest;
	std::string currentState;

	Toggle testJumpToggle;

	checkGLError("setup");

	std::shared_ptr<CollisionStructure> collider = bspTest;

	networkClient->updateNametex("big boy", playerTex);
	do
	{
		dt = IO.deltaTime;
		//std::cout << 1.0 / dt << "\n";

		if (mouseLockToggle.toggle(IO.isKeyPressed(GLFW_KEY_2)))
		{
			IO.toggleMouseState();
		}

		//if (shootToggle.toggle(IO.isMouseDown(GLFW_MOUSE_BUTTON_1)))
		shootToggle.toggle(IO.isKeyPressed(GLFW_KEY_B));
		if(shootToggle.getState() && IO.isMouseDown(GLFW_MOUSE_BUTTON_1) && shootDelay <= reloadTime)
		{
			reloadTime = 0.0;
			//spawn a fancy ball.

			int r = rand() % 255;
			int g = rand() % 255;
			int b = rand() % 255;

			float speed = 20.0f;


			std::shared_ptr<GameObjectColor> ball = Primitive::makeSphere(0.1, std::static_pointer_cast<ColorShader>(shaderCollection["color"]), r, g, b, 3.0);
			glm::mat4 rotMat = camera->getRotationMatrix();
			glm::vec3 vel = speed * rotMat * glm::vec4(0, 0, 1, 0);

			ball->friction = 0.1;
			ball->elasticity = 0.90f;
			ball->velocity = vel;
			ball->transform.setPosition(camera->getPosition());
			stage->addChild(ball);
			physicsList.push_back(ball);

			/*
			for (int i = 0; i < 3; i++)
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
			*/
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



		//move figure
		if (IO.isKeyPressed(GLFW_KEY_H))
		{
			glm::vec3 vel = playerSpeed * figure->transform.getTransformedZ();
			figure->velocity.x = vel.x;
			figure->velocity.z = vel.z;
		}
		else if (IO.isKeyPressed(GLFW_KEY_J))
		{
			glm::vec3 vel = -playerSpeed * figure->transform.getTransformedZ();
			figure->velocity.x = vel.x;
			figure->velocity.z = vel.z;
		}
		else
		{
			glm::vec3 vel(0);
			figure->velocity.x = vel.x;
			figure->velocity.z = vel.z;
		}
		if (testJumpToggle.toggle(IO.isKeyPressed(GLFW_KEY_G)))
		{
			figure->velocity.y = playerJump;
		}


		//now move character based on input.
		glm::vec3 controlVelocity = getControllerVelocity(IO, player);
		
		float vy = player->velocity.y;
		player->velocity = controlVelocity;
		camera->setRotation(cameraOrientation);
		if (movemodeToggle.getState())
		{
			//player->velocity -= gravity * dt;
		}
		else
		{
			player->velocity.y = vy;
		}

		//Network:
		if (toggleA.toggle(IO.isKeyPressed(GLFW_KEY_M)))
		{
			networkClient->updateNametex("big boy", playerTex);
		}

		while (networkClient->clientsToAdd.size())
		{
			int r = rand() % 155 + 100;
			int g = rand() % 155 + 100;
			int b = rand() % 155 + 100;
			std::shared_ptr<GameObjectTexture> nPlayer = std::shared_ptr<GameObjectTexture>(new GameObjectTexture);
			//Player
			nPlayer->transform.setPosition(0, 0, 0);
			//stage->addChild(nPlayer);
			nPlayer->friction = 0.0;// playerFriction;
			nPlayer->elasticity = 0.0;// playerElasticity;
			nPlayer->radius = playerRadius;
			nPlayer->neverDisable = true;
			nPlayer->collisionReactEnabled = true;
			nPlayer->gravityAffected = false; // ============= FALSE =============
			physicsList.push_back(nPlayer);
			networkClient->clientsToAdd.front()->clientObject = nPlayer;
			networkClient->clientsToAdd.pop();
			std::shared_ptr<GameObjectTexture> nPlayerMesh = std::shared_ptr<GameObjectTexture>(new GameObjectTexture);
			nPlayerMesh->shader = std::static_pointer_cast<TextureShader>(shaderCollection["texture"]);
			nPlayerMesh->texture = textureCollection["blankFigureTex"];
			nPlayerMesh->mesh = meshCollection["figure"];
			nPlayerMesh->animations = animationTest;
			stage->addChild(nPlayerMesh);
			networkClient->clientsToAdd.front()->clientDisplayObject = nPlayerMesh;
			std::cout << "NEW PLAYER!!!!!\n";
		}

		float slipFrac = slipPercentPerSecond * dt;
		float slipRotFrac = slipRotPercentPerSecond * dt;
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
					if (clientPair.second->velQue.size() > 0)
					{
						glm::vec3 velInt;
						while (clientPair.second->velQue.size() > 0)
						{
							velInt = clientPair.second->velQue.front();
							clientPair.second->velQue.pop();
						}
						clientPair.second->clientObject->velocity = velInt;
					}
					if (clientPair.second->posQue.size() > 0)
					{
						glm::vec3 posInt;
						while (clientPair.second->posQue.size() > 0)
						{
							posInt = clientPair.second->posQue.front();
							clientPair.second->posQue.pop();
						}
						//check the snap
						//float slip = glm::length(posInt - clientPair.second->clientObject->transform.getPosition());
						//if (slip > slipRange)
						//{
						//	clientPair.second->clientObject->transform.setPosition(posInt);
						//}
						clientPair.second->goalPos = posInt;
						//clientPair.second->clientObject->transform.setPosition(posInt);
					}
					if (glm::length(clientPair.second->clientObject->velocity) == 0.0)//<= dt * glm::length(gravity))
					{		
						clientPair.second->clientObject->friction = 20.0;
					}
					else
					{
						clientPair.second->clientObject->friction = 0.0;
					}
					glm::vec3 pos = clientPair.second->clientObject->transform.getPosition();
					glm::vec3 dif = clientPair.second->goalPos - pos;
					clientPair.second->clientObject->transform.setPosition(pos + slipFrac * (dif));
					if (glm::length(dif) >= teleportRange)
					{
						clientPair.second->clientObject->transform.setPosition(clientPair.second->goalPos);
					}
					if (clientPair.second->rotQue.size() > 0)
					{
						float rotInt;
						while (clientPair.second->rotQue.size() > 0)
						{
							rotInt = clientPair.second->rotQue.front();
							clientPair.second->rotQue.pop();							
						}
						//clientPair.second->clientObject->transform.setRotation(glm::vec3(0, rotInt, 0));
						clientPair.second->goalRot = rotInt;
					}
					float rot = clientPair.second->clientObject->transform.getRotation().y;
					float nRot = Geometry::lerpAngleDeg(rot, clientPair.second->goalRot, slipRotFrac);
					clientPair.second->clientObject->transform.setRotation(glm::vec3(0, nRot, 0));
					if (clientPair.second->stateQue.size() > 0)
					{
						std::string stateInt;
						while (clientPair.second->stateQue.size() > 0)
						{
							stateInt = clientPair.second->stateQue.front();
							clientPair.second->stateQue.pop();
						}
						//clientPair.second->clientObject->transform.setRotation(glm::vec3(0, rotInt, 0));
						clientPair.second->currentState = stateInt;
					}
					if (clientPair.second->updateName)
					{
						std::string name = clientPair.second->name;
						std::string clientTex = clientPair.second->texture;
						std::cout << "player"<<clientPair.first << " name is " << name << " : texture is " << clientTex << "\n";
						std::static_pointer_cast<GameObjectTexture>(clientPair.second->clientDisplayObject)->texture = textureCollection[clientTex];
						clientPair.second->updateName = false;
					}
					//give it a gravity push down to make sure animations work.. "will push up later..."
					//clientPair.second->clientObject->velocity += gravity * dt;
					setAnimation(clientPair.second->clientObject, clientPair.second->clientDisplayObject, dt, clientPair.second->currentState);
					clientPair.second->updateMutex = false;
				}
				//animate...
				//std::string state;
				//animatePlayer(clientPair.second->clientObject, clientPair.second->clientDisplayObject, dt, &state);
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
		
			tryMoveObject(object, dt * object->velocity, dt, collider);//tree

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
		
		float gravityMult = (IO.isKeyPressed(GLFW_KEY_SPACE) && player->velocity.y > 0.0 && movemodeToggle.getState()) ? 1.0 : 2.0;
		tryMovePlayer(player, dt* player->velocity, dt, collider, gravityMult * gravity);
		tryMovePlayer(figure, dt* figure->velocity, dt, collider, gravity);

		cameraOrientation = player->transform.getRotationQuat();
		camera->setRotation(cameraOrientation);
		//camera->setPosition(player->transform.getPosition() + glm::vec3(0, -playerHeight/2.0f + eyeHeight, 0));
		float eyes = (IO.isKeyPressed(GLFW_KEY_LEFT_CONTROL)) ? crouchEyeHeight : eyeHeight;
		camera->setPosition(player->transform.getPosition() + glm::vec3(0, eyes, 0));

		hand->transform.setPosition(camera->getPosition());
		hand->transform.setRotation(cameraOrientation);

		//test bsp
		/*
		if (bspTest->inside(camera->getPosition()))
			std::cout << "SOLID\n";
		else
			std::cout << "EMPTY\n";
		//*/

		//animate meshes
		std::string playerState, figureState;
		animatePlayer(player, playerMesh, dt, &playerState);
		animatePlayer(figure, figureMesh, dt, &figureState);		

		networkClient->updateNetworkPosVel(player->transform.getPosition(), player->velocity, player->transform.getRotation().y, playerState);
		playerMesh->visible = true;
		Portal::preRenderPortals(camera, 2);
		playerMesh->visible = false;
		std::static_pointer_cast<WindowShader>(shaderCollection["window"])->setViewMatrix(camera->getTransformMatrix());
		IO.display(camera, stage);
		checkGLError("in");
	} while (IO.isWindowOpen());
		
	return 0;
}
