#include "GrapplingHook.h"

GrapplingHook::GrapplingHook(float maxLength, float idealLength, float springConstant)
{
	maxGrappleLength = maxLength;
	this->idealLength = idealLength;
	this->springConstant = springConstant;
}

glm::vec3 GrapplingHook::action(bool doAction, std::shared_ptr<CollisionStructure> collider, glm::vec3 pos, glm::vec3 dir)
{
	if (startToggle.toggle(doAction))
	{
		//This runs when the player first toggles
		startPt = pos;
		startDir = dir;
		state = STATE::LAUNCH;
		missed = false;
		attached = false;
	}

	if (state == STATE::LAUNCH && !missed)
	{
		//project grapple through geometry
		//grappling hook is a line from startPt in the direction of dir at maxGrappleLength
		glm::vec3 endPt = startPt + maxGrappleLength * dir;

		if (collider->collide(0.0, startPt, endPt, &attachPt, &tempFace, 0))
		{
			//we have a valid attach
			attached = true;
			//we have the attached pt...
			state = STATE::GRAPPLE;
		}
		else
		{
			state = STATE::RELEASE;
			missed = true;
		}
	
	}
	else if (state == STATE::GRAPPLE)
	{
		//this is the grapply part...
		//let go of it:
		if (!doAction)
		{
			//player releases...
			attached = false;
			state = STATE::RELEASE;
			visible = false;
		}

		//Return a force as if the grappling line is a spring
		glm::vec3 force;
		
		//this is the vector from player to attached pt.
		glm::vec3 dif = attachPt - pos;

		float curLength = glm::length(dif);
		glm::vec3 forceDir = glm::normalize(dif);

		//update location and appearance:
		visible = true;
		transform.setPosition(pos);
		transform.setScale(0.5, 0.5, curLength);
		transform.setDirection(forceDir);

		//hooke's law:
		// f = -k(length - ideal Stretch)

		float forceMagnitude = springConstant;// *(curLength - idealLength);

		force = forceMagnitude * forceDir;

		return force;
	}
	else if (state == STATE::RELEASE)
	{
		visible = false;
	}


	/*
	if (doAction)
	{
		visible = true;
		transform.setPosition(glm::vec3(0));
		transform.setScale(1, 1, maxGrappleLength);
		transform.setDirection(dir);
	}
	else
	{
		visible = false;
	}*/


	return glm::vec3(0);
}
