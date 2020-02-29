#pragma once
#include "Headers.h"
#include "Mesh.h"
#include "Transform.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Collision.h"

class GameObject
{
public:
	GameObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader);
	GameObject() : GameObject(NULL, NULL) {}

	void addChild(std::shared_ptr<GameObject> gameObject);

	void render(std::shared_ptr<Camera> camera, glm::mat4 parentTransform);
	virtual void renderFunc(std::shared_ptr<Camera> camera, glm::mat4 parentTransform);

	static void cleanUp(std::shared_ptr<GameObject> object, bool removeChildren = false);

	std::vector<std::shared_ptr<GameObject>> children;
	//Render stuff
	bool visible;
	bool persistentVisible;
	Transform transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Shader> shader;
	bool toRemove;

	//Physics stuff:
	std::shared_ptr<Collider> collider;
	float mass;
	float invMass;
	float elasticity;
	float staticFriction;
	float dynamicFriction;

	glm::vec3 velocity;
	
	glm::vec3 force;
	glm::vec3 impulse;

	bool isColliding;

	void setMass(float mass);
	void setElasticity(float elasticity);

	void applyImpulse(glm::vec3 impulse, glm::vec3 position);
	void applyForce(glm::vec3 force, glm::vec3 position);

};

class GameObjectColor : public GameObject
{
public:
	GameObjectColor(std::shared_ptr<ColorMesh> mesh, std::shared_ptr<ColorShader> shader);
	GameObjectColor() : GameObjectColor(NULL, NULL) {}
	glm::mat4 colorMatrix;
	float shininess;
	std::shared_ptr<ColorShader> shader;

	void setShininess(float shininess);
	void setFillColor(int r, int g, int b);
	void renderFunc(std::shared_ptr<Camera> camera, glm::mat4 parentTransform);
	
};
