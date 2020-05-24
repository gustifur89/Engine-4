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

std::shared_ptr<Shader> Portal::debugShader = std::shared_ptr<Shader>();
std::shared_ptr<PortalShader> Portal::portalInternalShader = std::shared_ptr<PortalShader>();
GLuint Portal::debugVBO = 0;
GLuint Portal::debugVAO = 0;
GLuint Portal::internalPortalVBO = 0;
GLuint Portal::internalPortalVAO = 0;
std::vector<std::shared_ptr<Portal>> Portal::portalList = std::vector<std::shared_ptr<Portal>>();

Portal::Portal()
{
	
}

Portal::Portal(int width, int height)
{
	portalTexture = std::shared_ptr<ScreenBufferRenderTexture>(new ScreenBufferRenderTexture(width, height));
	intermediateTexture = std::shared_ptr<ScreenBufferRenderTexture>(new ScreenBufferRenderTexture(width, height));
	completedTexture = std::shared_ptr<ScreenBufferRenderTexture>(new ScreenBufferRenderTexture(width, height));
	this->width = width;
	this->height = height;
	setUpPortalRect();
	setUpDebugRect();
}

Portal::~Portal()
{

}

void Portal::setDebugShader(std::shared_ptr<Shader> debugShader_)
{
	Portal::debugShader = debugShader_;
}

void Portal::setInternalShader(std::shared_ptr<PortalShader> portalInternalShader_)
{
	Portal::portalInternalShader = portalInternalShader_;
	//GLuint colTexLoc = Portal::portalInternalShader->getUniformLocation("colTex");
	//glUniform1i(colTexLoc, 0);
}

void Portal::setUpPortalRect()
{
	// The fullscreen quad's FBO
	glGenVertexArrays(1, &internalPortalVAO);
	glBindVertexArray(internalPortalVAO);

	static const GLfloat windowVertexBufferData[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f,  1.0f, 0.0f,
	};

	glGenBuffers(1, &internalPortalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, internalPortalVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(windowVertexBufferData), windowVertexBufferData, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,					// attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Portal::setUpDebugRect()
{
	// The fullscreen quad's FBO
	glGenVertexArrays(1, &debugVAO);
	glBindVertexArray(debugVAO);

	static const GLfloat windowVertexBufferData[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f,  1.0f, 0.0f,
	};

	glGenBuffers(1, &debugVBO);
	glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(windowVertexBufferData), windowVertexBufferData, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,					// attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Portal::updateDebugRect(glm::vec4 bounds)
{
	// The fullscreen quad's FBO
	glBindVertexArray(debugVAO);

	///*
	GLfloat windowVertexBufferData[] = {
	bounds.x, bounds.y, 0.0f,
	bounds.z, bounds.y, 0.0f,
	bounds.x, bounds.w, 0.0f,
	bounds.x, bounds.w, 0.0f,
	bounds.z, bounds.y, 0.0f,
	bounds.z, bounds.w, 0.0f,
	};//*/

	/*
	static const GLfloat windowVertexBufferData[] = {
	-bounds.x, -1.0, 0.0f,
	1.0, -1.0, 0.0f,
	-bounds.x,  bounds.w, 0.0f,
	-bounds.x,  bounds.w, 0.0f,
	bounds.z, -bounds.y, 0.0f,
	bounds.z,  bounds.w, 0.0f,
	};
	*/
//	std::cout << bounds.x << " : " << bounds.y << " -:- " << bounds.z << " : " << bounds.w << "\n";

	glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(windowVertexBufferData), windowVertexBufferData, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,					// attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Portal::drawDubugRect()
{
	Portal::debugShader->useShader();
	glBindVertexArray(debugVAO);
	glEnableVertexAttribArray(debugVBO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(debugVBO);
	glBindVertexArray(0);
	std::cout << "DRAWRECTTTTTT\n";
}

bool Portal::isCameraInBounds(std::shared_ptr<Camera> camera)
{
	glm::mat4 inv = glm::inverse(getAdjustedPortalMatrix(camera));
	glm::vec4 pos = glm::vec4(camera->getPosition(), 1);
	pos = inv * pos;
	bool isIn = (pos.x <= 1.0 && pos.x >= -1.0 && pos.y <= 1.0 && pos.y >= -1.0 && pos.z <= 1.0 && pos.z >= -1.0);
	return isIn;
}

void Portal::setWorld(std::shared_ptr<GameObject> world)
{
	this->world = world;
}

void Portal::renderFunc(std::shared_ptr<Camera> camera, glm::mat4 parentTransform)
{
	if (internalRender) return;

	/*
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
	*/
	//glm::mat4 MMatrix = parentTransform * transform.getTransformMatrix();
	glm::mat4 MMatrix = parentTransform * getAdjustedPortalMatrix(camera);
	glm::mat4 MVMatrix = camera->getTransformMatrix() * MMatrix;
	glm::mat4 MVPmatrix = camera->getProjectionMatrix() * MVMatrix;
	glm::mat4 depMVPmatrix = camera->getDepthProjectionMatrix() * MVMatrix;
	glm::mat4 NMmatrix = glm::transpose(glm::inverse(MMatrix));

	if (shader && mesh && portalTexture)
	{
		glDisable(GL_CULL_FACE);
		shader->useShader();
		shader->setTexture(portalTexture);
		shader->setMatrixes(MVPmatrix, MVMatrix, depMVPmatrix, colorMatrix);
		mesh->render();
		glEnable(GL_CULL_FACE);
	}

	//this->transform.setScale(oScale);
	//this->transform.setPosition(oPos);
}

void Portal::drawStencil(std::shared_ptr<Camera> camera)
{
	/*/
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
	*/
	glm::mat4 MMatrix = getAdjustedPortalMatrix(camera);
	glm::mat4 MVMatrix = camera->getTransformMatrix() * MMatrix;
	glm::mat4 MVPmatrix = camera->getProjectionMatrix() * MVMatrix;
	glm::mat4 depMVPmatrix = camera->getDepthProjectionMatrix() * MVMatrix;
	glm::mat4 NMmatrix = glm::transpose(glm::inverse(MMatrix));

	glDisable(GL_CULL_FACE);
	shader->useShader();
	shader->setTexture(portalTexture);
	shader->setMatrixes(MVPmatrix, MVMatrix, depMVPmatrix, colorMatrix);
	mesh->render();
	glEnable(GL_CULL_FACE);

	//this->transform.setScale(oScale);
	//this->transform.setPosition(oPos);

	/*
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

	glDisable(GL_CULL_FACE);
	shader->useShader();

	/*
	if(internalRender)
		shader->setTexture(intermediateTexture);
	else
		shader->setTexture(completedTexture);
	* /

	shader->setTexture(portalTexture);
	shader->setMatrixes(MVPmatrix, MVMatrix, colorMatrix);
	mesh->render();
	glEnable(GL_CULL_FACE);

	this->transform.setScale(oScale);
	this->transform.setPosition(oPos);

	/*
	glm::mat4 MMatrix = parentTransform * transform.getTransformMatrix();
	glm::mat4 MVMatrix = camera->getTransformMatrix() * MMatrix;
	glm::mat4 MVPmatrix = camera->getProjectionMatrix() * MVMatrix;
	glm::mat4 NMmatrix = glm::transpose(glm::inverse(MMatrix));
	
	//glEnable(GL_STENCIL_TEST);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
	//glStencilMask(0xFF); // Write to stencil buffer
	
	/// *
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	//* /

	glDisable(GL_CULL_FACE);
	shader->useShader();
	shader->setTexture(portalTexture);
	shader->setMatrixes(MVPmatrix, MVMatrix, colorMatrix);
	mesh->render();
	glEnable(GL_CULL_FACE);

	/// *
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	//* /
	*/
}

void Portal::clearDepth(std::shared_ptr<Camera> camera)
{
	glm::mat4 MMatrix = getAdjustedPortalMatrix(camera);
	glm::mat4 MVMatrix = camera->getTransformMatrix() * MMatrix;
	glm::mat4 MVPmatrix = camera->getProjectionMatrix() * MVMatrix;
	glm::mat4 depMVPmatrix = camera->getDepthProjectionMatrix() * MVMatrix;
	glm::mat4 NMmatrix = glm::transpose(glm::inverse(MMatrix));

	glDisable(GL_CULL_FACE);
	portalInternalShader->useShader();
	portalInternalShader->setTexture(portalTexture);
	portalInternalShader->setMatrixes(MVPmatrix, MVMatrix, depMVPmatrix, colorMatrix);
	mesh->render();
	glEnable(GL_CULL_FACE);
}

float Portal::getMinZ(Camera camera)
{
	return 0.0f;// return glm::length(camera.position - this->transform.position);
}

glm::mat4 Portal::getObliqueProjectionMatrix(std::shared_ptr<Camera> camera)
{
	//need to do an oblique clipplane...
	// http://aras-p.info/texts/obliqueortho.html
	// https://bitbucket.org/ThomasRinsma/opengl-game-test/src/77/src/sceneobject/portal/clippedProjMat.cc?at=default

	glm::mat4 newProjection = camera->projectionMatrix;

	glm::vec4 clipNorm = glm::vec4(this->transform.getTransformedZ(), 0);
	glm::vec4 clipPos = glm::vec4(this->transform.getPosition(), 1);
	//Now these are in world space... convert to camera space...
	float side = sgn(glm::dot(clipNorm.xyz(), this->transform.getPosition() - camera->getPosition()));
	
	clipNorm = side * clipNorm;

	float dist = -glm::dot(clipPos, clipNorm);
	glm::vec4 clipPlane = glm::vec4(clipNorm.xyz(), dist);
	glm::mat4 worldToCamera = camera->getTransformMatrix();// glm::inverse(camera->Transform::getTransformMatrix()); //camera->getTransformMatrix();
	clipPlane = glm::inverse(glm::transpose(worldToCamera)) * clipPlane;

	if (clipPlane.w <= 0.0f)
	{
		glm::vec4 q = glm::inverse(newProjection) * glm::vec4(
			glm::sign(clipPlane.x),
			glm::sign(clipPlane.y),
			1.0f,
			1.0f
		);

		glm::vec4 c = clipPlane * (2.0f / (glm::dot(clipPlane, q)));

		glm::mat4 newProj = camera->projectionMatrix;
		// third row = clip plane - fourth row
		newProj = glm::row(newProj, 2, c - glm::row(newProj, 3));
		newProjection = newProj;
	}

	return newProjection;
}

bool Portal::boundsOverlap(glm::vec2 aMin, glm::vec2 aMax, glm::vec2 bMin, glm::vec2 bMax, bool output)
{
	if (output)
	{
		std::cout << "a: " << aMin.x << " , " << aMax.x << "  :  " << aMin.y << " , " << aMax.y << "\n";
		std::cout << "b: " << bMin.x << " , " << bMax.x << "  :  " << bMin.y << " , " << bMax.y << "\n";
	}


	if (bMax.x >= aMin.x && bMin.x <= aMax.x && bMax.y >= aMin.y && bMin.y <= aMax.y)
	{
		//if (!(((aMin.x == aMax.x) && (bMin.x == bMax.x)) || ((aMin.x == aMax.x) && (bMin.x == bMax.x))))
		if (!(aMin.x == aMax.x || aMin.y == aMax.y || bMin.x == bMax.x || bMin.y == bMax.y))
		{
			return true;
		}
	}
	return false;
}

glm::mat4 Portal::getAdjustedPortalMatrix(std::shared_ptr<Camera> camera)
{
	glm::vec3 nScale = this->transform.getScale();
	glm::vec3 nPos = this->transform.getPosition();

	float halfHeight = camera->minZ * tanf(camera->fov * 0.5f * TO_RAD);
	float halfWidth = halfHeight * camera->aspectRatio;
	float distToNearClipPlaneCorner = glm::length(glm::vec3(halfWidth, halfHeight, camera->minZ));

	nScale.z = distToNearClipPlaneCorner;

	nPos += this->transform.getTransformedZ() * distToNearClipPlaneCorner * ((glm::dot(this->transform.getTransformedZ(), this->transform.getPosition() - camera->getPosition()) > 0) ? 0.5f : -0.5f);
	
	glm::mat4 translation = glm::translate(nPos);// glm::translate(glm::mat4(1.0), position);

	glm::mat4 rotationMatrix = transform.getRotationMatrix();

	glm::mat4 scaleMatrix = glm::scale(nScale);

	return translation * rotationMatrix * scaleMatrix;
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

void Portal::portalRender(std::shared_ptr<Camera> camera, int drawDepth, int maxDepth, std::vector<std::shared_ptr<Portal>> portals, bool primaryDraw)
{
	if (!otherPortal || !world) return;
	
	//glm::vec4 portalBox = camera->getViewSpaceBoundingBox(this->transform.getTransformMatrix());
	glm::vec4 portalBox = camera->getViewSpaceBoundingBox(getAdjustedPortalMatrix(camera));
	glm::vec2 pMin = portalBox.xy();
	glm::vec2 pMax = portalBox.zw();

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
	portalCam->projectionMatrix = camera->projectionMatrix;
	portalCam->depthProjectionMatrix = camera->depthProjectionMatrix;

	//loop through all portals and draw if visible. May need to do this before the oblique. Could result in performanc eissues but we'll see.
	if (primaryDraw)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, portalTexture->frameBuffer);
	//	glViewport(0, 0, portalTexture->width, portalTexture->height);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	//https://stackoverflow.com/questions/48246302/writing-to-the-opengl-stencil-buffer
	//glClear(GL_STENCIL_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Do not draw any pixels on the back buffer
	glDepthMask(GL_FALSE);
	glEnable(GL_STENCIL_TEST); // Enables testing AND writing functionalities
	glStencilFunc(GL_EQUAL, drawDepth, 0xFF); // Do not test the current value in the stencil buffer, always accept any value on there for drawing
	glStencilMask(0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR); // Make every test succeed

	drawStencil(camera);

	glEnable(GL_STENCIL_TEST);

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // Make sure you will no longer (over)write stencil values, even if any test succeeds
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Make sure we draw on the backbuffer again.
	glDepthMask(GL_TRUE);
	glStencilFunc(GL_EQUAL, drawDepth + 1, 0xFF); // Now we will only draw pixels where the corresponding stencil buffer value equals 1
	glStencilMask(0x00);

	//clear the depth behind the portal. That way there is no overdraw from geometry 
	glDepthFunc(GL_ALWAYS);
	clearDepth(camera);
	glDepthFunc(GL_LEQUAL);

	// ... here you render your image on the computer screen (or whatever) that should be limited by the previous geometry ...
	portalCam->projectionMatrix = getObliqueProjectionMatrix(camera);

	this->world->visible = false;
	otherPortal->world->visible = true;

	//otherPortal->visible = false;
	world->render(portalCam, glm::mat4(1.0));
	//otherPortal->visible = true;

	otherPortal->world->visible = false;
	this->world->visible = true;

	if (drawDepth < maxDepth)
	{
		for (int i = 0; i < portalList.size(); i++)
		{
			if (otherPortal != portalList[i])
			{
				//glm::vec4 otherBox = portalCam->getViewSpaceBoundingBox(portalList[i]->transform.getTransformMatrix());
				glm::vec4 otherBox = portalCam->getViewSpaceBoundingBox(portalList[i]->getAdjustedPortalMatrix(camera));
				glm::vec2 oMin = glm::vec2(otherBox.xy());
				glm::vec2 oMax = glm::vec2(otherBox.zw());

				if (boundsOverlap(oMin, oMax, pMin, pMax, false))
				{
					portalList[i]->portalRender(portalCam, drawDepth + 1, maxDepth, portalList, false);
				}
			}
		}
	}

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Do not draw any pixels on the back buffer
	drawStencil(camera);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Make sure we draw on the backbuffer again.

	glDisable(GL_STENCIL_TEST);

	//glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Do not draw any pixels on the back buffer
	//drawStencil(camera);
	//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Make sure we draw on the backbuffer again.

	//after you draw the world, redraw the depth
//	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Do not draw any pixels on the back buffer
///	glDepthMask(GL_FALSE);

	//drawStencil(camera);

	//	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Make sure we draw on the backbuffer again.
//	glDepthMask(GL_TRUE);

	/*
	glm::vec4 portalBox = camera->getViewSpaceBoundingBox(this->transform.getTransformMatrix());
	//glm::vec4 portalBox = camera->getViewSpaceBoundingBox(getAdjustedPortalMatrix(camera));
	glm::vec2 pMin = portalBox.xy();
	glm::vec2 pMax = portalBox.zw();

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
	portalCam->projectionMatrix = camera->projectionMatrix;
	portalCam->depthProjectionMatrix = camera->depthProjectionMatrix;

	//loop through all portals and draw if visible. May need to do this before the oblique. Could result in performanc eissues but we'll see.
	if (primaryDraw)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, portalTexture->frameBuffer);
		glViewport(0, 0, portalTexture->width, portalTexture->height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	if (drawDepth > 0)
	{
		for (int i = 0; i < portalList.size(); i++)
		{
			if (otherPortal != portalList[i])
			{
				//glm::vec4 otherBox = portalCam->getViewSpaceBoundingBox(portalList[i]->transform.getTransformMatrix());
				glm::vec4 otherBox = portalCam->getViewSpaceBoundingBox(portalList[i]->getAdjustedPortalMatrix(camera));
				glm::vec2 oMin = glm::vec2(otherBox.xy());
				glm::vec2 oMax = glm::vec2(otherBox.zw());

				if (boundsOverlap(oMin, oMax, pMin, pMax))
				{
					portalList[i]->portalRender(portalCam, drawDepth - 1, portalList, false);
				}
			}
		}
	}

	//https://stackoverflow.com/questions/48246302/writing-to-the-opengl-stencil-buffer

	glClearStencil(0);
	glStencilMask(0xFF);
	glClear(GL_STENCIL_BUFFER_BIT);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Do not draw any pixels on the back buffer
	glDepthMask(GL_FALSE);
	glEnable(GL_STENCIL_TEST); // Enables testing AND writing functionalities
	glStencilFunc(GL_ALWAYS, 1, 0xFF); // Do not test the current value in the stencil buffer, always accept any value on there for drawing
	glStencilMask(0xFF);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE); // Make every test succeed

	drawStencil(camera);

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // Make sure you will no longer (over)write stencil values, even if any test succeeds
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Make sure we draw on the backbuffer again.
	glDepthMask(GL_TRUE);

	glStencilFunc(GL_EQUAL, 1, 0xFF); // Now we will only draw pixels where the corresponding stencil buffer value equals 1
	// ... here you render your image on the computer screen (or whatever) that should be limited by the previous geometry ...
	portalCam->projectionMatrix = getObliqueProjectionMatrix(camera);

	this->world->visible = false;
	otherPortal->world->visible = true;

	otherPortal->visible = false;
	world->render(portalCam, glm::mat4(1.0));
	otherPortal->visible = true;
	
	otherPortal->world->visible = false;
	this->world->visible = true;

	glDisable(GL_STENCIL_TEST);

	*/
}

void Portal::preRenderPortals(std::shared_ptr<Camera> camera, int depth)
{
	//find first to render
	//we need to use recursion to find them..
	//need to find if one is in the view frustum
	for (int i = 0; i < portalList.size(); i++)
	{
		portalList[i]->internalRender = true;

		glBindFramebuffer(GL_FRAMEBUFFER, portalList[i]->portalTexture->frameBuffer);
		glViewport(0, 0, portalList[i]->portalTexture->width, portalList[i]->portalTexture->height);
		//glClearStencil(0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		/*
		portalList[i]->properSize = portalList[i]->transform.getScale();
		portalList[i]->properPos = portalList[i]->transform.getPosition();

		glm::vec3 nScale = portalList[i]->properSize;
		glm::vec3 nPos = portalList[i]->properPos;

		float halfHeight = camera->minZ * tanf(camera->fov * 0.5f * TO_RAD);
		float halfWidth = halfHeight * camera->aspectRatio;
		float distToNearClipPlaneCorner = glm::length(glm::vec3(halfWidth, halfHeight, camera->minZ));

		nScale.z = distToNearClipPlaneCorner;

		nPos += portalList[i]->transform.getTransformedZ() * distToNearClipPlaneCorner * ((glm::dot(this->transform.getTransformedZ(), this->transform.getPosition() - camera->getPosition()) > 0) ? 0.5f : -0.5f);
		portalList[i]->transform.setScale(nScale);
		portalList[i]->transform.setPosition(nPos);
		*/
	}

	for (int i = 0; i < portalList.size(); i++)
	{
		glm::vec4 portalBox = camera->getViewSpaceBoundingBox(portalList[i]->getAdjustedPortalMatrix(camera));
		//glm::vec4 portalBox = camera->getViewSpaceBoundingBox(portalList[i]->transform.getTransformMatrix());
		glm::vec2 oMin = glm::vec2(portalBox.x, portalBox.y);
		glm::vec2 oMax = glm::vec2(portalBox.z, portalBox.w);
		glm::vec2 sMax = glm::vec2( 1,  1);
		glm::vec2 sMin = glm::vec2(-1, -1);

		if (boundsOverlap(oMin, oMax, sMin, sMax))
		{
			portalList[i]->portalRender(camera, 0, depth, portalList, true);
		}
	}

	for (int i = 0; i < portalList.size(); i++)
	{
		portalList[i]->internalRender = false;

		//portalList[i]->transform.setScale(portalList[i]->properSize);
		//portalList[i]->transform.setPosition(portalList[i]->properPos);
	}
}

void Portal::linkPortals(std::shared_ptr<Portal> portal1, std::shared_ptr<Portal> portal2)
{
	portal1->otherPortal = portal2;
	portal2->otherPortal = portal1;
}

void Portal::loadPortalList(std::shared_ptr<GameObject> world, int WIDTH, int HEIGHT, std::shared_ptr<PortalShader> portalShader, std::shared_ptr<ColorMesh> portalMesh, std::string fileName)
{
	fileName = std::string("src/data/") + fileName;
	std::ifstream file;
	file = std::ifstream(fileName);

	if (file.fail())
	{
		fprintf(stderr, "failed to load file at : ");
		fprintf(stderr, fileName.c_str());
		fprintf(stderr, "\n");
		return;
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
			port->transform.setScale(sx, sy, 0.0001);
			port->transform.setRotation(0, rot, 0);
			port->mesh = portalMesh;
			port->shader = portalShader;
			port->setWorld(world);
			portalList.push_back(port);
			world->addChild(port);
		}
		if (line == "link")
		{
			int A, B;
			file >> A >> B;
			Portal::linkPortals(portalList[A], portalList[B]);
		}
	}
}
