#pragma once
#include "Header.h"
#include "GameObject.h"
#include "Camera.h"
#include "RenderTexture.h"
#include "TempGeometry.h"
#include "WindowShader.h"

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

	int genCounter = 0;

	
public:
	Portal();
	Portal(int width, int height);
	~Portal();

	static void setDebugShader(std::shared_ptr<Shader> debugShader);
	static void setInternalShader(std::shared_ptr<PortalShader> portalInternalShader);
	static GLuint debugVBO;
	static GLuint debugVAO;
	static GLuint internalPortalVBO;
	static GLuint internalPortalVAO;
	static std::vector<std::shared_ptr<Portal>> portalList;


	void setUpPortalRect();
	void setUpDebugRect();
	void updateDebugRect(glm::vec4 bounds);
	void drawDubugRect();
	static std::shared_ptr<PortalShader> portalInternalShader;
	static std::shared_ptr<Shader> debugShader;

	bool isCameraInBounds(std::shared_ptr<Camera> camera);
	void setWorld(std::shared_ptr<GameObject> world);
	void renderFunc(std::shared_ptr<Camera> camera, glm::mat4 parentTransform);
	void portalRender(std::shared_ptr<Camera> camera, int drawDepth, int maxDepth, std::vector<std::shared_ptr<Portal>> portalList, bool primaryDraw = true);
	void drawStencil(std::shared_ptr<Camera> camera);
	void clearDepth(std::shared_ptr<Camera> camera);
	float getMinZ(Camera camera);
	glm::mat4 getObliqueProjectionMatrix(std::shared_ptr<Camera> camera);
	//static bool boundsOverlap(glm::vec2 aMin, glm::vec2 aMax, glm::vec2 bMin, glm::vec2 bMax, bool output = false);
	glm::mat4 getAdjustedPortalMatrix(std::shared_ptr<Camera> camera);

	bool action(std::shared_ptr<GameObject> object, glm::vec3 difference, bool* didTeleport, glm::vec3* teleThisPt, glm::vec3* teleNextPt, glm::quat* newRot);

	static void preRenderPortals(std::shared_ptr<Camera> camera, int depth = 6);
	static void linkPortals(std::shared_ptr<Portal> portal1, std::shared_ptr<Portal> portal2);
	static void loadPortalList(std::shared_ptr<GameObject> world, int WIDTH, int HEIGHT, std::shared_ptr<PortalShader> portalShader, std::shared_ptr<ColorMesh> portalMesh, std::string fileName);

	//parameters
	bool drawComplete;
	std::shared_ptr<Portal> otherPortal;
	glm::mat4 colorMatrix;
	std::shared_ptr<GameObject> world;
	std::shared_ptr<PortalShader> shader;
	int width, height;
	std::shared_ptr<ScreenBufferRenderTexture> completedTexture;
	std::shared_ptr<ScreenBufferRenderTexture> portalTexture;
	std::shared_ptr<ScreenBufferRenderTexture> intermediateTexture;
	bool internalRender;
};

