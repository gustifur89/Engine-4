#include "Camera.h"



Camera::Camera()
{
}

Camera::Camera(float fov, float aspectRatio, float minZ, float maxZ)
{
	projectionMatrix = glm::perspective(
		glm::radians(fov),	// The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
		aspectRatio,		// Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
		minZ,				// Near clipping plane. Keep as big as possible, or you'll get precision issues.
		maxZ				// Far clipping plane. Keep as little as possible.
	);

	viewFrustum = Frustum(projectionMatrix);
	this->fov = fov;
	this->aspectRatio = aspectRatio;
	this->minZ = minZ;
	this->maxZ = maxZ;
	this->gamma = 2.2;
	this->exposure = 1.0;
}

Camera::~Camera()
{

}

glm::mat4 Camera::getProjectionMatrix()
{
	return projectionMatrix;
}

glm::mat4 Camera::getTransformMatrix()
{
	glm::vec4 lookDirection(0, 0, 1, 0);	//Looking direction in viewSpace
	glm::vec4 upDirection(0, 1, 0, 0);
	//rotate by the direction
	glm::mat4 rotationMatrix = this->getRotationMatrix();
	//roate the look direction by the rotation
	lookDirection = rotationMatrix * lookDirection;
	upDirection = rotationMatrix * upDirection;
	glm::vec3 lookAt = position + glm::vec3(lookDirection);

	return glm::lookAt(
		position,	// the position of your camera, in world space
		lookAt,		// where you want to look at, in world space
		upDirection.xyz()// glm::vec3(0, 1, 0)    // probably glm::vec3(0,1,0), but (0,-1,0) would make you looking upside-down, which can be great too
	 );
}

bool Camera::isSphereInView(glm::vec3 position, double radius, glm::mat4 modelMatrix)
{
	glm::vec4 pos = glm::vec4(position, 1);
	pos = getTransformMatrix() * modelMatrix * pos;
	return viewFrustum.viewSpaceSphereInFrustum(glm::vec3(pos), radius);
}

bool Camera::isBoxInView(Bounds bounds, glm::mat4 modelMatrix)
{
	//camera.getProjection() * camera.getTransform()
	//Box in world space
		//put vectors into view space.
//	bounds = bounds.applyMatrix(getProjection() * getTransform() * modelMatrix);	
//	if (bounds.low.x <= 1.0 && bounds.low.y <= 1.0 && bounds.low.z <= 1.0 && bounds.high.x >= -1.0 && bounds.high.y >= -1.0 && bounds.high.z >= -1.0)
//	{
//		return true;
//	}
//	return false;
			
	//bounds = bounds.applyMatrix(getTransform() * modelMatrix);
	return viewFrustum.viewSpaceBoxInFrustum(getTransformMatrix() * modelMatrix, bounds);
}

bool Camera::isScaleInView(glm::mat4 modelMatrix)
{
	//Get the 8 points on the corners.
	//project them into the modelViewPorjection space.
	// clamp all the edges to the boundary of the box...
	// get the average of the points x,y,z.
	//test if all of those averages are in the range (0,1), but not 0 or 1.
	//if the average of the clamped points in not on the boundary of the cube, the object must be visible in the box... 
	//maybe....   might produce false positives, but shouldn't produce false negatives...
	//Let's just check it's screen space bounding box... like the xy.

	///MVP
	glm::mat4 MMatrix = modelMatrix;
	glm::mat4 MVMatrix = this->getTransformMatrix() * MMatrix;
	glm::mat4 MVPmatrix = projectionMatrix * MVMatrix;

	//glm::vec4 vLLL = glm::clamp(MVPmatrix * glm::vec4(position + scale * glm::vec3(-1, -1, -1), 1)	, 0.0f, 1.0f);
	//glm::vec4 vLLH = glm::clamp(MVPmatrix * glm::vec4(position + scale * glm::vec3(-1, -1, 1), 1)	, 0.0f, 1.0f);
	//glm::vec4 vLHL = glm::clamp(MVPmatrix * glm::vec4(position + scale * glm::vec3(-1, 1, -1), 1)	, 0.0f, 1.0f);
	//glm::vec4 vLHH = glm::clamp(MVPmatrix * glm::vec4(position + scale * glm::vec3(-1, 1, 1), 1)	, 0.0f, 1.0f);
	//glm::vec4 vHLL = glm::clamp(MVPmatrix * glm::vec4(position + scale * glm::vec3(1, -1, -1), 1)	, 0.0f, 1.0f);
	//glm::vec4 vHLH = glm::clamp(MVPmatrix * glm::vec4(position + scale * glm::vec3(1, -1, 1), 1)	, 0.0f, 1.0f);
	//glm::vec4 vHHL = glm::clamp(MVPmatrix * glm::vec4(position + scale * glm::vec3(1, 1, -1), 1)	, 0.0f, 1.0f);
	//glm::vec4 vHHH = glm::clamp(MVPmatrix * glm::vec4(position + scale * glm::vec3(1, 1, 1), 1)		, 0.0f, 1.0f);

	glm::vec4 vLLL = MVPmatrix * glm::vec4(glm::vec3(-1, -1, -1), 1);
	glm::vec4 vLLH = MVPmatrix * glm::vec4(glm::vec3(-1, -1, 1), 1)	;
	glm::vec4 vLHL = MVPmatrix * glm::vec4(glm::vec3(-1, 1, -1), 1)	;
	glm::vec4 vLHH = MVPmatrix * glm::vec4(glm::vec3(-1, 1, 1), 1)	;
	glm::vec4 vHLL = MVPmatrix * glm::vec4(glm::vec3(1, -1, -1), 1)	;
	glm::vec4 vHLH = MVPmatrix * glm::vec4(glm::vec3(1, -1, 1), 1)	;
	glm::vec4 vHHL = MVPmatrix * glm::vec4(glm::vec3(1, 1, -1), 1)	;
	glm::vec4 vHHH = MVPmatrix * glm::vec4(glm::vec3(1, 1, 1), 1)	;

	vLLL = vLLL / vLLL.w;
	vLLH = vLLH / vLLH.w;
	vLHL = vLHL / vLHL.w;
	vLHH = vLHH / vLHH.w;
	vHLL = vHLL / vHLL.w;
	vHLH = vHLH / vHLH.w;
	vHHL = vHHL / vHHL.w;
	vHHH = vHHH / vHHH.w;

	glm::vec2 oMax = glm::max(vLLL, glm::max(vLLH, glm::max(vLHL, glm::max(vLHH, glm::max(vHLL, glm::max(vHLH, glm::max(vHHL, vHHH)))))));
	glm::vec2 oMin = glm::min(vLLL, glm::min(vLLH, glm::min(vLHL, glm::min(vLHH, glm::min(vHLL, glm::min(vHLH, glm::min(vHHL, vHHH)))))));

	glm::vec2 sMax = glm::vec2(1, 1);
	glm::vec2 sMin = glm::vec2(-1, -1);

	if (oMax.x >= sMin.x && oMin.x <= sMax.x && oMax.y >= sMin.y && oMin.y <= sMax.y)
	{
		return true;
	}
	else
	{
		return false;
	}


	/*

	vLLL = glm::clamp(vLLL, -1.0f, 1.0f);
	vLLH = glm::clamp(vLLH, -1.0f, 1.0f);
	vLHL = glm::clamp(vLHL, -1.0f, 1.0f);
	vLHH = glm::clamp(vLHH, -1.0f, 1.0f);
	vHLL = glm::clamp(vHLL, -1.0f, 1.0f);
	vHLH = glm::clamp(vHLH, -1.0f, 1.0f);
	vHHL = glm::clamp(vHHL, -1.0f, 1.0f);
	vHHH = glm::clamp(vHHH, -1.0f, 1.0f);

	glm::vec3 average = (1.0f / 8.f) * (vLLL + vLLH + vLHL + vLHH + vHLL + vHLH + vHHL);

	return true;
	if (average.x > -1.0f && average.x < 1.0f && average.y > -1.0f && average.y < 1.0f && average.z > -1.0f && average.z < 1.0f)
	{
		//visible.
		return true;
	}
	else
	{
		//not visible
		return false;
	}
	*/
}

glm::vec4 Camera::getViewSpaceBoundingBox(glm::mat4 modelMatrix)
{
	///MVP
	glm::mat4 MMatrix = modelMatrix;
	glm::mat4 MVMatrix = this->getTransformMatrix() * MMatrix;
	glm::mat4 MVPmatrix = projectionMatrix * MVMatrix;

	glm::vec4 vLLL = MVPmatrix * glm::vec4(glm::vec3(-1, -1, -1), 1);
	glm::vec4 vLLH = MVPmatrix * glm::vec4(glm::vec3(-1, -1, 1), 1);
	glm::vec4 vLHL = MVPmatrix * glm::vec4(glm::vec3(-1, 1, -1), 1);
	glm::vec4 vLHH = MVPmatrix * glm::vec4(glm::vec3(-1, 1, 1), 1);
	glm::vec4 vHLL = MVPmatrix * glm::vec4(glm::vec3(1, -1, -1), 1);
	glm::vec4 vHLH = MVPmatrix * glm::vec4(glm::vec3(1, -1, 1), 1);
	glm::vec4 vHHL = MVPmatrix * glm::vec4(glm::vec3(1, 1, -1), 1);
	glm::vec4 vHHH = MVPmatrix * glm::vec4(glm::vec3(1, 1, 1), 1);

	vLLL = vLLL / vLLL.w;
	vLLH = vLLH / vLLH.w;
	vLHL = vLHL / vLHL.w;
	vLHH = vLHH / vLHH.w;
	vHLL = vHLL / vHLL.w;
	vHLH = vHLH / vHLH.w;
	vHHL = vHHL / vHHL.w;
	vHHH = vHHH / vHHH.w;

	glm::vec2 oMax = glm::max(vLLL, glm::max(vLLH, glm::max(vLHL, glm::max(vLHH, glm::max(vHLL, glm::max(vHLH, glm::max(vHHL, vHHH)))))));
	glm::vec2 oMin = glm::min(vLLL, glm::min(vLLH, glm::min(vLHL, glm::min(vLHH, glm::min(vHLL, glm::min(vHLH, glm::min(vHHL, vHHH)))))));

	return glm::vec4(oMin, oMax);
}

bool Camera::inView(glm::vec3 p)
{
	glm::mat4 VPmatrix = getProjectionMatrix() * getTransformMatrix();
	glm::vec4 t_point = VPmatrix * glm::vec4(p, 1.0);
	return abs(t_point.x) < t_point.w &&
		abs(t_point.y) < t_point.w &&
		0.0 < t_point.z &&
		t_point.z < t_point.w;
}
