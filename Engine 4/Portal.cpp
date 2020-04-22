#include "Portal.h"

// ============================ PortalCamera =======================

PortalCamera::PortalCamera(std::shared_ptr<Camera> camera)
{
	this->projectionMatrix = camera->projectionMatrix;
	this->scale = camera->getScale();
	this->rotation = camera->getRotation();
	this->position = camera->getPosition();
}

void PortalCamera::setProjection(std::shared_ptr<Camera> camera)
{
	this->projectionMatrix = camera->projectionMatrix;
}

glm::mat4 PortalCamera::getTransformMatrix()
{
	//std::cout << "yeet\n";
	return transformMatrix;
}

void PortalCamera::setTransformMatrix(glm::mat4 mat)
{
	transformMatrix = mat;
}

// ============================ Portal ===============================

Portal::Portal()
{
	
}

Portal::Portal(int width, int height)
{
	portalTexture = std::shared_ptr<ScreenBufferRenderTexture>(new ScreenBufferRenderTexture(width, height));
	this->width = width;
	this->height = height;
}

Portal::~Portal()
{

}

void Portal::setWorld(std::shared_ptr<GameObject> world)
{
	this->world = world;
}

void Portal::renderFunc(std::shared_ptr<Camera> camera, glm::mat4 parentTransform)
{
	glm::mat4 MMatrix = parentTransform * transform.getTransformMatrix();
	glm::mat4 MVMatrix = camera->getTransformMatrix() * MMatrix;
	glm::mat4 MVPmatrix = camera->getProjectionMatrix() * MVMatrix;
	glm::mat4 NMmatrix = glm::transpose(glm::inverse(MMatrix));

	if (shader && mesh && portalTexture)
	{
		glDisable(GL_CULL_FACE);
		shader->useShader();
		shader->setTexture(portalTexture);
		shader->setMatrixes(MVPmatrix, MVMatrix, colorMatrix);
		mesh->render();
		glEnable(GL_CULL_FACE);
	}
}

float Portal::getMinZ(Camera camera)
{
	return 0.0f;// return glm::length(camera.position - this->transform.position);
}

bool Portal::action(std::shared_ptr<GameObject> object, glm::vec3 difference, bool* didTeleport, glm::vec3* teleThisPt, glm::vec3* teleNextPt, glm::quat* newRot)
{
	//this is naive....
	//check to see if the line of the object's trajectory intersects the plane... If it does... Move the player?
	
	if (*didTeleport) return false;

	glm::vec3 thisP = object->transform.getPosition();
	glm::vec3 nextP = thisP + difference;

	glm::vec3 faceNorm;
	glm::vec3 nP = nextP;

	//glm::vec4 v1(-1, -1, 0, 1);
	//glm::vec4 v2(1, -1, 0, 1);
	//glm::vec4 v3(-1, 1, 0, 1);
	//glm::vec4 v4(1, 1, 0, 1);
	glm::vec4 v1(-0.5, -0.5, 0, 1);
	glm::vec4 v2(0.5, -0.5, 0, 1);
	glm::vec4 v3(-0.5, 0.5, 0, 1);
	glm::vec4 v4(0.5, 0.5, 0, 1);

	v1 = this->transform.getTransformMatrix() * v1;
	v2 = this->transform.getTransformMatrix() * v2;
	v3 = this->transform.getTransformMatrix() * v3;
	v4 = this->transform.getTransformMatrix() * v4;

	glm::vec3 norm = this->transform.getTransformedZ();

	//make 2 new triangles based on the 

	MeshTriangle tri0(v1, v2, v3, norm, norm, norm);
	MeshTriangle tri1(v2, v3, v4, norm, norm, norm);

	glm::vec3 ptOnSurface;
	glm::vec3 remainder;

	if (tri0.rayIntersectsTriangle(thisP, nextP, &ptOnSurface, &remainder))
	{
		//update position...
		glm::mat4 m = otherPortal->transform.getTransformMatrix() * glm::inverse(this->transform.getTransformMatrix()) * object->transform.getTransformMatrix();

		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m, scale, rotation, translation, skew, perspective);

		*newRot = rotation;

		*teleThisPt = otherPortal->transform.getTransformMatrix() * glm::inverse(this->transform.getTransformMatrix()) * glm::vec4(thisP, 1);
		*teleNextPt = otherPortal->transform.getTransformMatrix() * glm::inverse(this->transform.getTransformMatrix()) * glm::vec4(nextP, 1);
		object->transform.setRotation(rotation);
		//object->transform.setPosition(translation);
		*didTeleport = true;
		return true;
	}
	if (tri1.rayIntersectsTriangle(thisP, nextP, &ptOnSurface, &remainder))
	{
		glm::mat4 m = otherPortal->transform.getTransformMatrix() * glm::inverse(this->transform.getTransformMatrix()) * object->transform.getTransformMatrix();

		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m, scale, rotation, translation, skew, perspective);

		object->transform.setRotation(rotation);
		*newRot = rotation;

		*teleThisPt = otherPortal->transform.getTransformMatrix() * glm::inverse(this->transform.getTransformMatrix()) * glm::vec4(thisP, 1);
		*teleNextPt = otherPortal->transform.getTransformMatrix() * glm::inverse(this->transform.getTransformMatrix()) * glm::vec4(nextP, 1);
		*didTeleport = true;
		return true;
	}
	return false;
}

double Portal::distanceToPlane(glm::vec3 entPos, glm::vec3 dir, glm::vec3 planePoint)
{
	dir = glm::normalize(dir);
	dir = -dir;
	return (glm::dot(entPos, dir) - glm::dot(dir, planePoint)) / glm::length(dir);
//	glm::vec4 n = glm::vec4(dir, glm::dot(dir, planePoint));
//	return glm::dot(entPos, glm::vec3(n)) + n.w;
	//return glm::dot(entPos, dir) - glm::dot(dir, planePoint);
}

void Portal::portalRender(std::shared_ptr<Camera> camera)
{
	if (!otherPortal || !world) return;

	this->visible = false;
	otherPortal->visible = false;
	
	std::shared_ptr<Camera> portalCam = std::shared_ptr<Camera>(new Camera(camera->fov, camera->aspectRatio, camera->minZ, camera->maxZ));

	glm::mat4 m = otherPortal->transform.getTransformMatrix() * glm::inverse(this->transform.getTransformMatrix()) * camera->Transform::getTransformMatrix();
	
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(m, scale, rotation, translation, skew, perspective);

	portalCam->setRotation(rotation);
	portalCam->setPosition(translation);
	
	//need to do an oblique clipplane...
	// http://aras-p.info/texts/obliqueortho.html
	// https://bitbucket.org/ThomasRinsma/opengl-game-test/src/77/src/sceneobject/portal/clippedProjMat.cc?at=default

	/*
	glm::mat4 projection = portalCam->getProjectionMatrix();
	//glm::vec3 clipNorm = (this->transform.getTransformMatrix() * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f)
	float dist = glm::length(this->transform.getPosition());


	glm::vec4 clipPlane((this->transform.getTransformMatrix() * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)).xyz(), dist);
	float side =- sgn(glm::dot(clipPlane.xyz(), this->transform.getPosition() - camera->getPosition()));
	

	clipPlane = glm::inverse(glm::transpose(camera->getTransformMatrix())) * clipPlane;

	//clipPlane.x *= side;
	//clipPlane.y *= side;
	//clipPlane.z *= side;

	if (clipPlane.w <= 0.0f)
	{
		std::cout << "yem\n";

		glm::vec4 q = glm::inverse(projection) * glm::vec4(
			glm::sign(clipPlane.x),
			glm::sign(clipPlane.y),
			1.0f,
			1.0f
		);

		glm::vec4 c = clipPlane * (2.0f / (glm::dot(clipPlane, q)));

		glm::mat4 newProj = projection;
		// third row = clip plane - fourth row
		//newProj = glm::row(newProj, 2, c - glm::row(newProj, 3));
		projection[0][2] = c.x - projection[0][3];
		projection[1][2] = c.y - projection[1][3]; 
		projection[2][2] = c.z - projection[2][3];
		projection[3][2] = c.w - projection[3][3];
		portalCam->projectionMatrix = projection;
	}*/

	glm::vec4 clipNorm = glm::vec4(this->transform.getTransformedZ(), 0);
	glm::vec4 clipPos = glm::vec4(this->transform.getPosition(), 1);
	//Now these are in world space... convert to camera space...
	float side = sgn(glm::dot(clipNorm.xyz(), this->transform.getPosition() - camera->getPosition()));
	clipNorm = side * clipNorm;
	//offset clipPos by the thickness to avoid weird clipping
	//clipPos -= side * scale.z * glm::normalize(clipNorm);

	float dist = glm::dot(clipPos, clipNorm);
	glm::vec4 clipPlane = glm::vec4(clipNorm.xyz(), dist);
	glm::mat4 worldToCamera = camera->getTransformMatrix();
	clipPlane = glm::inverse(glm::transpose(worldToCamera)) * clipPlane;


	//clipPos = worldToCamera * clipPos;
	//clipNorm = side * worldToCamera * clipNorm;
	//glm::vec4 clipPlane = glm::vec4(clipNorm.x, clipNorm.y, clipNorm.z, -glm::dot(clipPos, clipNorm));

	//do a check to make sure clipplane is infront of the camera
	if (clipPlane.w <= 0);//&& clipPlane.z <= 0)
	{
		//std::cout << genCounter++ << " | yem\n";

		glm::mat4 projection = portalCam->getProjectionMatrix();

		glm::vec4 q = glm::inverse(projection) * glm::vec4(
			glm::sign(clipPlane.x),
			glm::sign(clipPlane.y),
			1.0f,
			1.0f
		);
		glm::vec4 c = clipPlane * (2.0f / (glm::dot(clipPlane, q)));
		// third row = C - 4th row
		projection = glm::row(projection, 2, c - glm::row(projection, 3));
		portalCam->projectionMatrix = projection;
	}


	glBindFramebuffer(GL_FRAMEBUFFER, portalTexture->frameBuffer);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	world->render(portalCam, glm::mat4(1.0));

	this->visible = true;
	otherPortal->visible = true;
}

void Portal::preRenderPortals(std::vector<std::shared_ptr<Portal>> portals, Camera& camera, int width, int height, int depth)
{
	//find first to render
	//we need to use recursion to find them..
	//need to find if one is in the view frustum
	for (int i = 0; i < portals.size(); i++)
	{
		//portals[i]->updated = true;
	//	portals[i]->portalRender(camera, width, height);
	}

}

void Portal::linkPortals(std::shared_ptr<Portal> portal1, std::shared_ptr<Portal> portal2)
{
	portal1->otherPortal = portal2;
	portal2->otherPortal = portal1;
}
