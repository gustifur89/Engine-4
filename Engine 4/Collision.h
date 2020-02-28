#pragma once
#include "Header.h"

class Collider
{
public:
	enum TYPE
	{
		SPHERE,
		CAPSULE,
		AABB,
		ELLIPSOID,
		ARBIT
	};

	TYPE type;

	virtual void temp();
};

class Capsule : public Collider
{
public:
	Capsule(float radius, float height);

	glm::vec4 A;
	glm::vec4 B;

	float radius;
	float height;

	void temp();
};

class Sphere : public Collider
{
public:
	Sphere(float radius);

	float radius;

	void temp();
};

class Ellipsoid : public Collider
{
public:
	Ellipsoid(glm::vec3 axis);

	glm::vec3 axis;

	void temp();
};

class AABB : public Collider
{
public:
	AABB(glm::vec3 size);
	glm::vec3 size;
	void temp();
};

class CollisionFunctions
{
public:
	static void getClosestPoints(glm::mat4 aToWorld, std::shared_ptr<Capsule> capA, glm::mat4 bToWorld, std::shared_ptr<Capsule> capB, glm::vec3* ptA, glm::vec3* ptB);
	static void getClosestPointToSphere(glm::mat4 aToWorld, std::shared_ptr<Capsule> capA, glm::mat4 bToWorld, std::shared_ptr<Sphere> sphB, glm::vec3* pt);
	static void getClosestPointToPoint(glm::mat4 aToWorld, std::shared_ptr<Capsule> capA, glm::vec3 pt, glm::vec3* outPt);

};

