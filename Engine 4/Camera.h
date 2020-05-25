#pragma once
#include "Headers.h"
#include "Transform.h"
#include "Light.h"
#include "Geometry.h"

class Camera : public Transform
{
private:
	glm::vec4 plane0;
	glm::vec4 plane1;
	glm::vec4 plane2;
	glm::vec4 plane3;
	glm::vec4 plane4;
	glm::vec4 plane5;
	std::vector<glm::vec4> planes;
public:
	Camera();
	Camera(float fov, float aspectRatio, float minZ, float maxZ);
	~Camera();

	void createPlanes();

	virtual glm::mat4 getTransformMatrix();
	glm::mat4 getProjectionMatrix();
	glm::mat4 getDepthProjectionMatrix();
	
	bool isSphereInView(glm::vec3 position, double radius, glm::mat4 modelMatrix);
	bool isBoxInView(Bounds bounds, glm::mat4 modelMatrix);
	bool isScaleInView(glm::mat4 modelMatrix);
	bool isBoxInViewSpace(Bounds bounds, glm::mat4 modelMatrix);
	glm::vec4 getViewSpaceBoundingBox(glm::mat4 modelMatrix, glm::vec3 low = glm::vec3(-0.5), glm::vec3 high = glm::vec3(0.5));
	static bool boundsOverlap(glm::vec2 aMin, glm::vec2 aMax, glm::vec2 bMin, glm::vec2 bMax);

	bool isLightInView(std::shared_ptr<Light> light);

	bool inView(glm::vec3 p);
	//parameters
	glm::mat4 projectionMatrix;
	glm::mat4 depthProjectionMatrix; // to make oblique clipping play fiarly.
	GLfloat fov;
	GLfloat aspectRatio;
	GLfloat minZ;
	GLfloat maxZ;
	GLfloat gamma;
	GLfloat exposure;
	Frustum viewFrustum;

	std::string debugValue;
};

