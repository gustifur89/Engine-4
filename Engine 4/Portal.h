#pragma once
#include "Header.h"
#include "GameObject.h"
#include "Camera.h"
#include "RenderTexture.h"

class PortalCamera : public Camera
{

public:
	PortalCamera(std::shared_ptr<Camera> camera);

	void setProjection(std::shared_ptr<Camera> camera);

	glm::mat4 getTransformMatrix();
	void setTransformMatrix(glm::mat4 mat);

	glm::mat4 transformMatrix;
};




class Portal : public GameObject
{
private:
	double distanceToPlane(glm::vec3 entPos, glm::vec3 dir, glm::vec3 planePoint);

	static float sgn(float a)
	{
		if (a > 0.0f) return 1.0f;
		if (a < 0.0f) return -1.0f;
		return 0.0f;
	}
public:
	Portal();
	Portal(int width, int height);
	~Portal();

	void setWorld(std::shared_ptr<GameObject> world);
	void renderFunc(std::shared_ptr<Camera> camera, glm::mat4 parentTransform);
	void portalRender(std::shared_ptr<Camera> camera);
	float getMinZ(Camera camera);


	static void preRenderPortals(std::vector<std::shared_ptr<Portal>> portals, Camera& camera, int width, int height, int depth = 6);
	static void linkPortals(std::shared_ptr<Portal> portal1, std::shared_ptr<Portal> portal2);
	
	//parameters
	std::shared_ptr<Portal> otherPortal;
	glm::mat4 colorMatrix;
	std::shared_ptr<GameObject> world;
	std::shared_ptr<PortalShader> shader;
	int width, height;
	std::shared_ptr<ScreenBufferRenderTexture> portalTexture;
};

