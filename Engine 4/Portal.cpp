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
	intermediateTexture = std::shared_ptr<ScreenBufferRenderTexture>(new ScreenBufferRenderTexture(width, height));
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

	glm::vec3 oScale = this->transform.getScale();
	glm::vec3 oPos = this->transform.getPosition();
	glm::vec3 nScale = oScale;
	glm::vec3 nPos = oPos;

	float halfHeight = camera->minZ * tanf(camera->fov * 0.5f * TO_RAD);
	float halfWidth = halfHeight * camera->aspectRatio;
	float distToNearClipPlaneCorner = glm::length(glm::vec3(halfWidth, halfHeight, camera->minZ));

	nScale.z = distToNearClipPlaneCorner;

	nPos += this->transform.getTransformedZ() * distToNearClipPlaneCorner * ((glm::dot(this->transform.getTransformedZ(), this->transform.getPosition() - camera->getPosition()) > 0) ? 0.5f : -0.5f);
	this->transform.setScale(nScale);
	this->transform.setPosition(nPos);


	glm::mat4 MMatrix = parentTransform * transform.getTransformMatrix();
	glm::mat4 MVMatrix = camera->getTransformMatrix() * MMatrix;
	glm::mat4 MVPmatrix = camera->getProjectionMatrix() * MVMatrix;
	glm::mat4 NMmatrix = glm::transpose(glm::inverse(MMatrix));

	if (shader && mesh && portalTexture)
	{
		glDisable(GL_CULL_FACE);
		
		shader->useShader();
		if (internalRender)
			shader->setTexture(intermediateTexture);
		else
			shader->setTexture(portalTexture);
		
		shader->setMatrixes(MVPmatrix, MVMatrix, colorMatrix);
		mesh->render();
		glEnable(GL_CULL_FACE);
	}

	if (!internalRender)
	{
		this->transform.setScale(oScale);
		this->transform.setPosition(oPos);
	}
}

float Portal::getMinZ(Camera camera)
{
	return 0.0f;// return glm::length(camera.position - this->transform.position);
}

bool Portal::action(std::shared_ptr<GameObject> object, glm::vec3 difference, bool* didTeleport, glm::vec3* teleThisPt, glm::vec3* teleNextPt, glm::quat* newRot)
{
	if (!otherPortal || !world) return false;
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

void Portal::portalRender(std::shared_ptr<Camera> camera, int drawDepth, std::vector<std::shared_ptr<Portal>> portalList, bool primaryDraw)
{
	if (!otherPortal || !world) return;
	//if (primaryDraw)
	//	drawComplete = false;
	//check if visible:
	//if (!camera->isScaleInView(this->transform.getPosition(), this->transform.getScale(), this->transform.getTransformMatrix()))
	//{
		//This doesn't work, but we'll get to it....
	//	return;
	//}
	glm::vec4 portalBox = camera->getViewSpaceBoundingBox(this->transform.getTransformMatrix());
	glm::vec2 pMin = glm::vec2(portalBox.xy());
	glm::vec2 pMax = glm::vec2(portalBox.zw());
	//if (camera->isScaleInView(portals[i]->transform.getTransformMatrix()))
	std::shared_ptr<Camera> portalCam = std::shared_ptr<Camera>(new Camera(camera->fov, camera->aspectRatio, camera->minZ, camera->maxZ));

	//glm::mat4 m = otherPortal->transform.getTransformMatrix() * glm::inverse(this->transform.getTransformMatrix()) * camera->Transform::getTransformMatrix();
	
	glm::mat4 m = otherPortal->transform.getTransformMatrix() * glm::inverse(this->transform.getTransformMatrix()) * camera->Transform::getTransformMatrix();

	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(m, scale, rotation, translation, skew, perspective);

	portalCam->setRotation(rotation);
	portalCam->setPosition(translation);
	portalCam->projectionMatrix = camera->projectionMatrix;

	//loop through all portals and draw if visible. May need to do this before the oblique. Could result in performanc eissues but we'll see.
	if (drawDepth > 0)
	{
		for (int i = 0; i < portalList.size(); i++)
		{
			if (otherPortal != portalList[i])
			{
				glm::vec4 otherBox = portalCam->getViewSpaceBoundingBox(portalList[i]->transform.getTransformMatrix());
				glm::vec2 oMin = glm::vec2(otherBox.xy());
				glm::vec2 oMax = glm::vec2(otherBox.zw());
				if (oMax.x >= pMin.x && oMin.x <= pMax.x && oMax.y >= pMin.y && oMin.y <= pMax.y)
				{
					portalList[i]->portalRender(portalCam, drawDepth - 1, portalList, false);
				}
			}

			//	if (portalCam->isScaleInView(portalList[i]->transform.getTransformMatrix()))
			//		portalList[i]->portalRender(portalCam, drawDepth - 1, portalList, false);
		}
	}

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

	float dist = -glm::dot(clipPos, clipNorm);
	glm::vec4 clipPlane = glm::vec4(clipNorm.xyz(), dist);
	glm::mat4 worldToCamera = camera->getTransformMatrix();// glm::inverse(camera->Transform::getTransformMatrix()); //camera->getTransformMatrix();
	clipPlane = glm::inverse(glm::transpose(worldToCamera)) * clipPlane;
	
	if (clipPlane.w <= 0.0f)
	{
		glm::vec4 q = glm::inverse(camera->projectionMatrix) * glm::vec4(
			glm::sign(clipPlane.x),
			glm::sign(clipPlane.y),
			1.0f,
			1.0f
		);

		glm::vec4 c = clipPlane * (2.0f / (glm::dot(clipPlane, q)));

		glm::mat4 newProj = camera->projectionMatrix;
		// third row = clip plane - fourth row
		newProj = glm::row(newProj, 2, c - glm::row(newProj, 3));
		portalCam->projectionMatrix = newProj;
	}
	
	if (primaryDraw)
		glBindFramebuffer(GL_FRAMEBUFFER, portalTexture->frameBuffer);
	else
		glBindFramebuffer(GL_FRAMEBUFFER, intermediateTexture->frameBuffer);

	//this->visible = false;
	otherPortal->visible = false;

	//glBindFramebuffer(GL_FRAMEBUFFER, portalTexture->frameBuffer);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	world->render(portalCam, glm::mat4(1.0));
	//if(primaryDraw)
	//	drawComplete = true;
	//this->visible = true;
	otherPortal->visible = true;
}

void Portal::preRenderPortals(std::vector<std::shared_ptr<Portal>> portals, std::shared_ptr<Camera> camera, int depth)
{
	//find first to render
	//we need to use recursion to find them..
	//need to find if one is in the view frustum
	for (int i = 0; i < portals.size(); i++)
	{
		portals[i]->internalRender = true;
	}
	
	for (int i = 0; i < portals.size(); i++)
	{
	//	portals[i]->portalRender(camera, depth, portals, true);
		glm::vec4 portalBox = camera->getViewSpaceBoundingBox(portals[i]->transform.getTransformMatrix());
		glm::vec2 oMin = glm::vec2(portalBox.xy());
		glm::vec2 oMax = glm::vec2(portalBox.zw());
		glm::vec2 sMax = glm::vec2(1, 1);
		glm::vec2 sMin = glm::vec2(-1, -1);
		//if (camera->isScaleInView(portals[i]->transform.getTransformMatrix()))
		if (oMax.x >= sMin.x && oMin.x <= sMax.x && oMax.y >= sMin.y && oMin.y <= sMax.y)
		{
			portals[i]->portalRender(camera, depth, portals, true);
		}
	}
	//reset all textures
	for (int i = 0; i < portals.size(); i++)
	{
		portals[i]->internalRender = false;
	}
}

void Portal::linkPortals(std::shared_ptr<Portal> portal1, std::shared_ptr<Portal> portal2)
{
	portal1->otherPortal = portal2;
	portal2->otherPortal = portal1;
}

std::vector<std::shared_ptr<Portal>> Portal::loadPortalList(std::shared_ptr<GameObject> world, int WIDTH, int HEIGHT, std::shared_ptr<PortalShader> portalShader, std::shared_ptr<ColorMesh> portalMesh, std::string fileName)
{
	std::vector<std::shared_ptr<Portal>> portals;

	fileName = std::string("src/data/") + fileName;
	std::ifstream file;
	file = std::ifstream(fileName);

	if (file.fail())
	{
		fprintf(stderr, "failed to load file at : ");
		fprintf(stderr, fileName.c_str());
		fprintf(stderr, "\n");
		return portals;
	}

	std::string line;

	while (!file.eof())
	{
		//assumes you get 9 in.
		file >> line;

		if (line == "portal")
		{
			float x, y, z, sx, sy, rot;
			int r, g, b;
			file >> x >> z >> y >> sx >> sy >> rot;	//y and z switch
			std::shared_ptr<Portal> port = std::shared_ptr<Portal>(new Portal(WIDTH, HEIGHT));
			port->transform.setPosition(-x, y, z);
			port->transform.setScale(sx, sy, 0.06);
			port->transform.setRotation(0, rot, 0);
			port->mesh = portalMesh;
			port->shader = portalShader;
			port->setWorld(world);
			portals.push_back(port);
			world->addChild(port);
		}
		if (line == "link")
		{
			int A, B;
			file >> A >> B;
			Portal::linkPortals(portals[A], portals[B]);
		}
	}
	return portals;
}
