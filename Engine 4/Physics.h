#pragma once
#include "Header.h"
#include "GameObject.h"

struct collisionManifold
{
public:
	bool doesCollide;
	glm::vec3 contactPoint;
	glm::vec3 normal;
	float sepDepth;
};

class Physics
{
protected:
	std::vector<std::shared_ptr<GameObject>> objectList;

	glm::vec3 gravity;

	void bounceBoundary(std::shared_ptr<GameObject> object);
	bool killBoundary(std::shared_ptr<GameObject> object);

	glm::vec3 boundary;

	collisionManifold collideCapsuleCapsule(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB);
	collisionManifold collideAABBAABB(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB);
	collisionManifold collideSphereSphere(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB);
	collisionManifold collideEllipsoidEllipsoid(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB);

	collisionManifold collideSphereEllipsoid(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB);
	collisionManifold collideEllipsoidSphere(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB);

	collisionManifold collideEllipsoidCapsule(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB);
	collisionManifold collideCapsuleEllipsoid(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB);

	collisionManifold collideSphereCapsule(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB);
	collisionManifold collideCapsuleSphere(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB);

	collisionManifold collideAABBCapsule(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB);
	collisionManifold collideCapsuleAABB(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB);

	collisionManifold collideAABBSphere(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB);
	collisionManifold collideSphereAABB(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB);




	collisionManifold collide(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB);

	void collision(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB);
	void penetrationCorrection(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB, collisionManifold& colInfo); 

public:
	Physics();
	
	void addObject(std::shared_ptr<GameObject> object);
	void setGravity(glm::vec3 gravity);
	void setBounceBoundary(glm::vec3 boundary);

	void physicsStep(float dt, int iterations);
};

