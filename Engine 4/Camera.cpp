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
	depthProjectionMatrix = projectionMatrix;
	viewFrustum = Frustum(projectionMatrix);
	this->fov = fov;
	this->aspectRatio = aspectRatio;
	this->minZ = minZ;
	this->maxZ = maxZ;
	this->gamma = 2.2;
	this->exposure = 1.0;
	createPlanes();
}

Camera::~Camera()
{

}

void Camera::createPlanes()
{
	glm::mat4 inv_proj = glm::inverse(projectionMatrix);

	glm::vec4 vLLL = inv_proj * glm::vec4(-1, -1, -1, 1);
	glm::vec4 vLLH = inv_proj * glm::vec4(-1, -1,  1, 1);
	glm::vec4 vLHL = inv_proj * glm::vec4(-1,  1, -1, 1);
	glm::vec4 vLHH = inv_proj * glm::vec4(-1,  1,  1, 1);
	glm::vec4 vHLL = inv_proj * glm::vec4( 1, -1, -1, 1);
	glm::vec4 vHLH = inv_proj * glm::vec4( 1, -1,  1, 1);
	glm::vec4 vHHL = inv_proj * glm::vec4( 1,  1, -1, 1);
	glm::vec4 vHHH = inv_proj * glm::vec4( 1,  1,  1, 1);

	vLLL = vLLL / vLLL.w;
	vLLH = vLLH / vLLH.w;
	vLHL = vLHL / vLHL.w;
	vLHH = vLHH / vLHH.w;
	vHLL = vHLL / vHLL.w;
	vHLH = vHLH / vHLH.w;
	vHHL = vHHL / vHHL.w;
	vHHH = vHHH / vHHH.w;

	//Triangle LZ(vLLL, vLHL, vHLL);
	//plane0 vLLL, vLHL, vHLL  near clipping plane
	plane0 = glm::vec4(glm::normalize(glm::cross(glm::vec3(vLHL - vLLL), glm::vec3(vHLL - vLLL))), 0);
	plane0.w = -glm::dot(plane0, vLLL);

	//Triangle HZ(vHLH, vLHH, vLLH);
	plane1 = glm::vec4(glm::normalize(glm::cross(glm::vec3(vLHH - vHLH), glm::vec3(vLLH - vHLH))), 0);
	plane1.w = -glm::dot(plane1, vHLH);

	//Triangle LX(vLLH, vLHL, vLLL);
	plane2 = glm::vec4(glm::normalize(glm::cross(glm::vec3(vLHL - vLLH), glm::vec3(vLLL - vLLH))), 0);
	plane2.w = -glm::dot(plane2, vLLH);

	//Triangle HX(vHHL, vHLH, vHLL);
	plane3 = glm::vec4(glm::normalize(glm::cross(glm::vec3(vHLH - vHHL), glm::vec3(vHLL - vHHL))), 0);
	plane3.w = -glm::dot(plane3, vHHL);

	//Triangle LY(vHLH, vLLH, vHLL);
	plane4 = glm::vec4(glm::normalize(glm::cross(glm::vec3(vLLH - vHLH), glm::vec3(vHLL - vHLH))), 0);
	plane4.w = -glm::dot(plane4, vHLH);

	//Triangle HY(vHHH, vHHL, vLHH);
	plane5 = glm::vec4(glm::normalize(glm::cross(glm::vec3(vHHL - vHHH), glm::vec3(vLHH - vHHH))), 0);
	plane5.w = -glm::dot(plane5, vHHH);


	//glm::vec3 testP(0, 0, 6);
	//std::cout << plane0.x << " : " << plane0.y << " : " << plane0.z << "\n";
	//std::cout << "0: " << (glm::dot(testP, plane0.xyz())) << "\n";
	//std::cout << "1: " << (glm::dot(testP, plane1.xyz())) << "\n";
	//std::cout << "2: " << (glm::dot(testP, plane2.xyz())) << "\n";
	//std::cout << "3: " << (glm::dot(testP, plane3.xyz())) << "\n";
	//std::cout << "4: " << (glm::dot(testP, plane4.xyz())) << "\n";
	//std::cout << "5: " << (glm::dot(testP, plane5.xyz())) << "\n";
	//std::cout << "0: " << (plane0.w + glm::dot(testP, plane0.xyz())) << "\n";
	//std::cout << "1: " << (plane1.w + glm::dot(testP, plane1.xyz())) << "\n";
	//std::cout << "2: " << (plane2.w + glm::dot(testP, plane2.xyz())) << "\n";
	//std::cout << "3: " << (plane3.w + glm::dot(testP, plane3.xyz())) << "\n";
	//std::cout << "4: " << (plane4.w + glm::dot(testP, plane4.xyz())) << "\n";
	//std::cout << "5: " << (plane5.w + glm::dot(testP, plane5.xyz())) << "\n";

	planes.push_back(plane0);
	planes.push_back(plane1);
	planes.push_back(plane2);
	planes.push_back(plane3);
	planes.push_back(plane4);
	planes.push_back(plane5);
	
	//glm::vec3 testP(-100, 0, -0.5);
	
	/*for (int i = 0; i < 6; i++)
	{
		/*
		glm::vec4 plane = planes[i];
		float dst = -(glm::dot(plane.xyz(), testP.xyz()) + plane.w);
		if (dst > 0)
		{
			testP += (dst) * plane.xyz();
		}
		* /
		//above is plane projection... But we don't want to do that....

	}*/

	//in View space...
	//clamp the z... then... clamp to the crosssectional rectangle orthogonally.
	
	/*
	glm::vec3 testP(16, 12, -30);

	testP.z = glm::clamp(testP.z, -maxZ, -minZ);
	//find out the half height and half width...
	float halfHeight = fabs(testP.z * tanf(fov * 0.5f * TO_RAD));
//	std::cout << halfHeight << "\n";
	float halfWidth = halfHeight * aspectRatio;
	testP.x = glm::clamp(testP.x, -halfWidth, halfWidth);
	testP.y = glm::clamp(testP.y, -halfHeight, halfHeight);
	
	glm::vec4 temp = projectionMatrix * glm::vec4(testP, 1);
	
//	std::cout << testP.x << " : " << testP.y << " : " << testP.z << " \n";
	
	testP = temp / temp.w;
	*/
//	std::cout << testP.x << " : " << testP.y << " : " << testP.z << " \n";
	

	//std::cout << "0: " << (plane0.w + glm::dot(testP, plane0.xyz())) << "\n";
	//std::cout << "1: " << (plane1.w + glm::dot(testP, plane1.xyz())) << "\n";
	//std::cout << "2: " << (plane2.w + glm::dot(testP, plane2.xyz())) << "\n";
	//std::cout << "3: " << (plane3.w + glm::dot(testP, plane3.xyz())) << "\n";
	//std::cout << "4: " << (plane4.w + glm::dot(testP, plane4.xyz())) << "\n";
	//std::cout << "5: " << (plane5.w + glm::dot(testP, plane5.xyz())) << "\n";
	//std::cout << "\n";
	
} 

glm::mat4 Camera::getProjectionMatrix()
{
	return projectionMatrix;
}

glm::mat4 Camera::getDepthProjectionMatrix()
{
	return depthProjectionMatrix;
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
	glm::mat4 MVmatrix = this->getTransformMatrix() * MMatrix;
	glm::mat4 MVPmatrix = this->getProjectionMatrix() * MVmatrix;

	std::vector<glm::vec4> points;

	//get all points og the bounding box in viewspace
	points.push_back(MVmatrix * glm::vec4(-0.5, -0.5, -0.5, 1));
	points.push_back(MVmatrix * glm::vec4(-0.5, -0.5,  0.5, 1));
	points.push_back(MVmatrix * glm::vec4(-0.5,  0.5, -0.5, 1));
	points.push_back(MVmatrix * glm::vec4(-0.5,  0.5,  0.5, 1));
	points.push_back(MVmatrix * glm::vec4( 0.5, -0.5, -0.5, 1));
	points.push_back(MVmatrix * glm::vec4( 0.5, -0.5,  0.5, 1));
	points.push_back(MVmatrix * glm::vec4( 0.5,  0.5, -0.5, 1));
	points.push_back(MVmatrix * glm::vec4( 0.5,  0.5,  0.5, 1));
	//points.push_back(MVmatrix * glm::vec4(-0.5, -0.5, 0.0, 1));
	//points.push_back(MVmatrix * glm::vec4( 0.5, -0.5, 0.0, 1));
	//points.push_back(MVmatrix * glm::vec4(-0.5,  0.5, 0.0, 1));
	//points.push_back(MVmatrix * glm::vec4( 0.5,  0.5, 0.0, 1));
	//clamp them against the view frustum..

	
	/*
	for (glm::vec4 plane : planes)
	{
		for (int i = 0; i < points.size(); i++)
		{
			/*
			//clamp it...
			float dst = (plane.w + glm::dot(points[0].xyz(), plane.xyz()));
			if (dst < 0)
			{
				//other side.. project to surface.
				//float val = (glm::dot(diff, plane.xyz()) + plane.w) / (glm::length2(plane.xyz()));
				//points[i] -= glm::vec4(val * plane.xyz(), 0);
				//float d0 = -glm::dot(points[i].xyz(), plane.xyz());
				//float dist = plane.w - d0;
				points[i] -= glm::vec4(dst * plane.xyz(), 0);//glm::vec4(dist * plane.xyz(), 0);

				//points[i] -= glm::vec4(glm::dot(points[i].xyz(), plane.xyz()) * plane.xyz(), 0);

			}* /

			float dst = -(glm::dot(plane.xyz(), points[i].xyz()) + plane.w);
			if (dst > 0)
			{
				points[i] += glm::vec4((dst)*plane.xyz(), 0);
				//if (fabs(points[i].x) < 0.001) points[i].x = 0.0;
				//if (fabs(points[i].y) < 0.001) points[i].y = 0.0;
				//if (fabs(points[i].z) < 0.001) points[i].z = 0.0;
			}
		}
	}*/
	
	int numOfBehindProper = 0;

	for (int i = 0; i < points.size(); i++)
	{
		///*
		points[i].z = glm::clamp(points[i].z, -maxZ, -minZ);
		
		if(points[i].z == -minZ)
			numOfBehindProper++;
		
		//find out the half height and half width...
		float halfHeight = fabs(points[i].z * tanf(fov * 0.5f * TO_RAD));
		float halfWidth = halfHeight * aspectRatio;
		points[i].x = glm::clamp(points[i].x, -halfWidth, halfWidth);
		points[i].y = glm::clamp(points[i].y, -halfHeight, halfHeight);
		//*/
	}

	int numberOfBehind = 0;
	std::vector<glm::vec4> pts;
	for (int i = 0; i < points.size(); i++)
	{
		glm::vec4 temp = this->getProjectionMatrix() * points[i];
		temp = temp / temp.w;
		if (temp.z == -1.0f)
		{
			numberOfBehind++;
		}
		pts.push_back(temp);
		/*if (temp.z < -1.0 || temp.z > 1.0 || temp.y < -1.0 || temp.y > 1.0 || temp.z < -1.0 || temp.z > 1.0)
		{
			//std::cout << temp.x << " : " << temp.y << " : " << temp.z << "\n";
		}
		else
		{
			pts.push_back(temp);
		}*/
		//pts.push_back(temp);
		//pts.push_back(temp);
		//std::cout << temp.x << " : " << temp.y << " : " << temp.z << "\n";
		/*if (temp.x < -1.0 || temp.x > 1.0 || temp.y < -1.0 || temp.y > 1.0 || temp.z < -1.0 || temp.z > 1.0)
		{
			//std::cout << temp.x << " : " << temp.y << " : " << temp.z << "\n";
		}
		else
		{
			pts.push_back(temp);
		}*/
	}

	/*
	std::vector<glm::vec4> pts;
	if (vLLL.z >= minZ && vLLL.z <= maxZ)
		pts.push_back(vLLL);
	if (vLLH.z >= minZ && vLLH.z <= maxZ)
		pts.push_back(vLLH);
	if (vLHL.z >= minZ && vLHL.z <= maxZ)
		pts.push_back(vLHL);
	if (vLHH.z >= minZ && vLHH.z <= maxZ)
		pts.push_back(vLHH);
	if (vHLL.z >= minZ && vHLL.z <= maxZ)
		pts.push_back(vHLL);
	if (vHLH.z >= minZ && vHLH.z <= maxZ)
		pts.push_back(vHLH);
	if (vHHL.z >= minZ && vHHL.z <= maxZ)
		pts.push_back(vHHL);
	if (vHHH.z >= minZ && vHHH.z <= maxZ)
		pts.push_back(vHHH);


	if (pts.size() == 0)
	{
		return glm::vec4(-3, -3, -3, -3);
	}
	*/
	

	
	if (numberOfBehind == pts.size() || numOfBehindProper == pts.size())
	{
		return glm::vec4(-3, -3, -3, -3);
	}

	glm::vec2 oMax = pts[0];
	glm::vec2 oMin = pts[0];

	for (int i = 1; i < pts.size(); i++)
	{
		oMax = glm::max(oMax, pts[i].xy());
		oMin = glm::min(oMin, pts[i].xy());
	}

	return glm::vec4(oMin.x, oMin.y, oMax.x, oMax.y);
	
	//*/

	/*
	///MVP
	glm::mat4 MMatrix = modelMatrix;
	glm::mat4 MVMatrix = this->getTransformMatrix() * MMatrix;
	glm::mat4 MVPmatrix = this->getProjectionMatrix() * MVMatrix;

	///*
	glm::vec4 vLLL = MVPmatrix * glm::vec4(-0.5, -0.5, -0.5, 1);
	glm::vec4 vLLH = MVPmatrix * glm::vec4(-0.5, -0.5,  0.5, 1);
	glm::vec4 vLHL = MVPmatrix * glm::vec4(-0.5,  0.5, -0.5, 1);
	glm::vec4 vLHH = MVPmatrix * glm::vec4(-0.5,  0.5,  0.5, 1);
	glm::vec4 vHLL = MVPmatrix * glm::vec4( 0.5, -0.5, -0.5, 1);
	glm::vec4 vHLH = MVPmatrix * glm::vec4( 0.5, -0.5,  0.5, 1);
	glm::vec4 vHHL = MVPmatrix * glm::vec4( 0.5,  0.5, -0.5, 1);
	glm::vec4 vHHH = MVPmatrix * glm::vec4( 0.5,  0.5,  0.5, 1);
	


	float zLLL = vLLL.z;
	float zLLH = vLLH.z;
	float zLHL = vLHL.z;
	float zLHH = vLHH.z;
	float zHLL = vHLL.z;
	float zHLH = vHLH.z;
	float zHHL = vHHL.z;
	float zHHH = vHHH.z;

	vLLL = glm::clamp(vLLL / vLLL.w, -1.0f, 1.0f);
	vLLH = glm::clamp(vLLH / vLLH.w, -1.0f, 1.0f);
	vLHL = glm::clamp(vLHL / vLHL.w, -1.0f, 1.0f);
	vLHH = glm::clamp(vLHH / vLHH.w, -1.0f, 1.0f);
	vHLL = glm::clamp(vHLL / vHLL.w, -1.0f, 1.0f);
	vHLH = glm::clamp(vHLH / vHLH.w, -1.0f, 1.0f);
	vHHL = glm::clamp(vHHL / vHHL.w, -1.0f, 1.0f);
	vHHH = glm::clamp(vHHH / vHHH.w, -1.0f, 1.0f);

	std::vector<glm::vec4> pts;
	if (vLLL.z >= minZ && vLLL.z <= maxZ)
		pts.push_back(vLLL);
	if (vLLH.z >= minZ && vLLH.z <= maxZ)
		pts.push_back(vLLH);
	if (vLHL.z >= minZ && vLHL.z <= maxZ)
		pts.push_back(vLHL);
	if (vLHH.z >= minZ && vLHH.z <= maxZ)
		pts.push_back(vLHH);
	if (vHLL.z >= minZ && vHLL.z <= maxZ)
		pts.push_back(vHLL);
	if (vHLH.z >= minZ && vHLH.z <= maxZ)
		pts.push_back(vHLH);
	if (vHHL.z >= minZ && vHHL.z <= maxZ)
		pts.push_back(vHHL);
	if (vHHH.z >= minZ && vHHH.z <= maxZ)
		pts.push_back(vHHH);

	/*
	std::vector<glm::vec4> pts;
	if (zLLL >= minZ && zLLL <= maxZ)
		pts.push_back(vLLL);
	if (zLLH >= minZ && zLLH <= maxZ)
		pts.push_back(vLLH);
	if (zLHL >= minZ && zLHL <= maxZ)
		pts.push_back(vLHL);
	if (zLHH >= minZ && zLHH <= maxZ)
		pts.push_back(vLHH);
	if (zHLL >= minZ && zHLL <= maxZ)
		pts.push_back(vHLL);
	if (zHLH >= minZ && zHLH <= maxZ)
		pts.push_back(vHLH);
	if (zHHL >= minZ && zHHL <= maxZ)
		pts.push_back(vHHL);
	if (zHHH >= minZ && zHHH <= maxZ)
		pts.push_back(vHHH);
	//*/
	
	/*
	if (wLLL > 0.0 && vLLL.z >= minZ && vLLL.z <= maxZ)
		pts.push_back(vLLL);
	if (wLLH > 0.0 && vLLH.z >= minZ && vLLH.z <= maxZ)
		pts.push_back(vLLH);
	if (wLHL > 0.0 && vLHL.z >= minZ && vLHL.z <= maxZ)
		pts.push_back(vLHL);
	if (wLHH > 0.0 && vLHH.z >= minZ && vLHH.z <= maxZ)
		pts.push_back(vLHH);
	if (wHLL > 0.0 && vHLL.z >= minZ && vHLL.z <= maxZ)
		pts.push_back(vHLL);
	if (wHLH > 0.0 && vHLH.z >= minZ && vHLH.z <= maxZ)
		pts.push_back(vHLH);
	if (wHHL > 0.0 && vHHL.z >= minZ && vHHL.z <= maxZ)
		pts.push_back(vHHL);
	if (wHHH > 0.0 && vHHH.z >= minZ && vHHH.z <= maxZ)
		pts.push_back(vHHH);
	//*/

	/*
	if (pts.size() == 0)
	{
		return glm::vec4(-3, -3, -3, -3);
	}

	glm::vec2 oMax = pts[0];
	glm::vec2 oMin = pts[0];

	for (int i = 1; i < pts.size(); i++)
	{
		oMax = glm::max(oMax, pts[i].xy());
		oMin = glm::min(oMin, pts[i].xy());
	}
	*/
	/*
	glm::vec2 oMax = glm::max(vLLL, glm::max(vLLH, glm::max(vLHL, glm::max(vLHH, glm::max(vHLL, glm::max(vHLH, glm::max(vHHL, vHHH)))))));
	glm::vec2 oMin = glm::min(vLLL, glm::min(vLLH, glm::min(vLHL, glm::min(vLHH, glm::min(vHLL, glm::min(vHLH, glm::min(vHHL, vHHH)))))));
	*/


	//*
	//glm::vec4 vLL = MVPmatrix * glm::vec4(-0.5, -0.5, 0, 1);
	//glm::vec4 vLH = MVPmatrix * glm::vec4(-0.5,  0.5, 0, 1);
	//glm::vec4 vHL = MVPmatrix * glm::vec4( 0.5, -0.5, 0, 1);
	//glm::vec4 vHH = MVPmatrix * glm::vec4( 0.5,  0.5, 0, 1);
	/*
	//vLL = glm::vec4(glm::clamp(vLL.xyz(), -1.0f, 1.0f), vLL.w);
	//vLH = glm::vec4(glm::clamp(vLH.xyz(), -1.0f, 1.0f), vLH.w);
	//vHL = glm::vec4(glm::clamp(vHL.xyz(), -1.0f, 1.0f), vHL.w);
	//vHH = glm::vec4(glm::clamp(vHH.xyz(), -1.0f, 1.0f), vHH.w);

	vLL = vLL / vLL.w;
	vLH = vLH / vLH.w;
	vHL = vHL / vHL.w;
	vHH = vHH / vHH.w;


	vLL = glm::clamp(vLL, -1.0f, 1.0f);
	vLH = glm::clamp(vLH, -1.0f, 1.0f);
	vHL = glm::clamp(vHL, -1.0f, 1.0f);
	vHH = glm::clamp(vHH, -1.0f, 1.0f);


	glm::vec2 oMax = glm::max(vLL, glm::max(vLH, glm::max(vHL, vHH)));
	glm::vec2 oMin = glm::min(vLL, glm::min(vLH, glm::min(vHL, vHH)));


	*/
	/*

	vLL = vLL / vLL.w;
	vLH = vLH / vLH.w;
	vHL = vHL / vHL.w;
	vHH = vHH / vHH.w;

	std::vector<glm::vec4> pts;
	if (vLL.z >= minZ || vLL.z <= maxZ)
		pts.push_back(vLL);
	if (vLH.z >= minZ || vLH.z <= maxZ)
		pts.push_back(vLH);
	if (vHL.z >= minZ || vHL.z <= maxZ)
		pts.push_back(vHL);
	if (vHH.z >= minZ || vHH.z <= maxZ)
		pts.push_back(vHH);

	if (pts.size() == 0)
	{
		return glm::vec4(-3, -3, -3, -3);
	}

	glm::vec2 oMax = pts[0];
	glm::vec2 oMin = pts[0];

	for (int i = 1; i < pts.size(); i++) 
	{
		oMax = glm::max(oMax, pts[i].xy());
		oMin = glm::min(oMin, pts[i].xy());
	}//*/

	//return glm::vec4(oMin.x, oMin.y, oMax.x, oMax.y);
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
