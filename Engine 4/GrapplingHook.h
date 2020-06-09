#pragma once
#include "Header.h"
#include "GameObject.h"
#include "Toggle.h"
#include "Octree.h"

class GrapplingHook : public GameObjectColor
{
private:
	enum STATE
	{
		LAUNCH,
		GRAPPLE,
		RELEASE
	};

	Toggle startToggle;
	STATE state;

	float maxGrappleLength;
	float idealLength;
	float springConstant;

public:
	glm::vec3 direction;
	glm::vec3 startPt;
	glm::vec3 startDir;
	glm::vec3 attachPt;
	std::vector<glm::vec3> tempFace;
	bool missed;
	bool attached;

	GrapplingHook(float maxLength, float idealLength, float springConstant);

	glm::vec3 action(bool doAction, std::shared_ptr<CollisionStructure> collider, glm::vec3 pos, glm::vec3 dir);


};

