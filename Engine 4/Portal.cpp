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


	//solve for minZ
	//get distance from the viewer to the plane of the portal, perpedicular to the direction... so closest point.
	/*
	glm::vec3 portalVertex = this->transform.position;
	float minZ = distanceToPlane(camera.position, camera.getTransformedZ(), portalVertex);// fmax((getMinZ(camera) - radius), 0.1f);
//	float minZA = fmax((getMinZ(camera) - radius), 0.1f);

	minZ = fmax(0.0, minZ - 0.5);

	//to get the true minZ, we need to use the matrix projections...

	//camera.fov * (1.0 - (minZ / camera.maxZ))
	Camera portalCamera(camera.fov, camera.aspectRatio, minZ, camera.maxZ);

	portalCamera.position = otherPortal->transform.position + (camera.position - this->transform.position);

	portalCamera.setRotation(camera);
	*/
	/*
	glBindFramebuffer(GL_FRAMEBUFFER, renderTexture.frameBuffer);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	world->render(portalCamera, glm::mat4(1.0));
	updated = true;
	*/

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

	//get clip plane in camera space:
	//clip plane given by... point and normal..
	glm::vec4 clipNorm = glm::vec4(this->transform.getTransformedZ(), 0);
	glm::vec4 clipPos = glm::vec4(this->transform.getPosition(), 1);
	//Now these are in world space... convert to camera space...
	float side = sgn(glm::dot(clipNorm.xyz(), this->transform.getPosition() - camera->getPosition()));

	//offset clipPos by the thickness to avoid weird clipping
	//clipPos -= side * scale.z * glm::normalize(clipNorm);

	glm::mat4 worldToCamera = camera->getTransformMatrix();
	clipPos = worldToCamera * clipPos;
	clipNorm = side * worldToCamera * clipNorm;
	glm::vec4 clipPlane = glm::vec4(clipNorm.x, clipNorm.y, clipNorm.z, -glm::dot(clipPos, clipNorm) - glm::dot(clipNorm, glm::vec4(0,0,scale.z, 0)) );

	//do a check to make sure clipplane is infront of the camera
	if (clipPlane.w <= 0 && clipPlane.z <= 0)
	{
		glm::mat4 projection = portalCam->getProjectionMatrix();

		glm::vec4 q;
		q = glm::inverse(projection) * glm::vec4(
			sgn(clipPlane.x),
			sgn(clipPlane.y),
			1.0f,
			1.0f
		);
		glm::vec4 c = clipPlane * (2.0f / (glm::dot(clipPlane, q)));
		// third row = C - 4th row
		projection[0][2] = c.x;//- projection[0][3];
		projection[1][2] = c.y;//- projection[1][3]; 
		projection[2][2] = c.z + 1.0f;//- projection[2][3];
		projection[3][2] = c.w;//- projection[3][3];

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
