#pragma once
#include "Headers.h"

class MeshTriangle
{
private:
	glm::vec3 calculateNormal(glm::vec3 vert0, glm::vec3 vert1, glm::vec3 vert2)
	{
		return glm::normalize(glm::cross(vert1 - vert0, vert2 - vert0));
	}

	const float EPSILON = 0.1;

public:
	MeshTriangle(glm::vec3 vert0, glm::vec3 vert1, glm::vec3 vert2, glm::vec3 n0, glm::vec3 n1, glm::vec3 n2)
	{
		this->vert0 = vert0;
		this->vert1 = vert1;
		this->vert2 = vert2;
		this->n0 = n0;
		this->n1 = n1;
		this->n2 = n2;
		this->normal = calculateNormal(vert0, vert1, vert2);
	//	std::cout << normal.x << " : " << normal.y << " " << normal.z << "\n";
	}

	glm::vec3 vert0, vert1, vert2;
	glm::vec3 n0, n1, n2;
	glm::vec3 normal;

	bool rayUnder(float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3* w0, glm::vec3* w1)
	{
		//need to expand the plane by the radius;
		// plane equation: dot( (p - v0), normal) = 0
		// extruded equation: dot( (p - v0 + radius * normal), normal) = 0

		//TODO: rework to be more efficient. A lot can be shortened. Lots of redundant checks.

		//side:
		float side0 = glm::dot(v0 - vert0, normal);
		float side1 = glm::dot(v1 - vert0, normal);

		if (side0 >= 0 && side1 >= 0)
		{
			//both on the colliding side:
			*w0 = v0;
			*w1 = v1;
			return true;
		}
		else if (side0 < 0 && side1 < 0)
		{
			//not colliding. ignore
			return false;
		}
		else
		{
			//interesting case. It transects the plane
			glm::vec3 dir01 = v1 - v0;

			float denom = glm::dot(dir01, normal);

			if (denom == 0.0) // parallel
			{
				return false;
			}
			else
			{
				float t = glm::dot(vert0 + radius * normal - v0, normal) / denom;
				glm::vec3 intP = v0 + t * dir01;

				if (t < 0 || t > 1.0) // segment doesn't intersect the plane... (this may be unneeded because of the above checks... maybe fix);
				{
					return false;
				}
				else if (isPointInTriangle(v0 + t * dir01, radius))
				{
					//w0, w1
					//see which side is on the top:

					//check to see if moving towards the triangle?
					*w0 = v0;
					*w1 = v0 + t * dir01;

					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}

	bool collide(float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3* out)
	{
		//need to expand the plane by the radius;
		// plane equation: dot( (p - v0), normal) = 0
		// extruded equation: dot( (p - v0 + radius * normal), normal) = 0

		//TODO: rework to be more efficient. A lot can be shortened. Lots of redundant checks.

		//side:
		float side0 = glm::dot(v0 - (vert0 + radius * normal), normal);
		float side1 = glm::dot(v1 - (vert0 + radius * normal), normal);

		glm::vec3 dir01 = v1 - v0;

	//	if (glm::dot(dir01, normal) > 0.0)
	//		return false;

		if (side0 > 0 && side1 > 0)
		{
			//both on the colliding side: So no collision
			//*out = v0;
			return false;
		}
		else if (side0 < 0 && side1 < 0)
		{
			//not colliding. ignore
			//*out = v0;
			return false;
		}
		else
		{
			//interesting case. It transects the plane
			
			float denom = glm::dot(dir01, normal);

			if (denom == 0.0) // parallel Not sure this can happen.... ??? 
			{
				//*out = v0;
				return false;
			}
			else
			{
				float t = glm::dot(vert0 + radius * normal - v0, normal) / denom; // I'm like 90% sure this will always be valid here due to above checks ( valid :=  t<=1.0 && t >+ 0.0 )
				//std::cout << t << "\n";
																	  
				//glm::vec3 intP = v0 + t * dir01;
				if(t <= 0.0 && t >= 1.0f)
				{
					return false;
				}
				else if (isPointInTriangle(v0 + t * dir01, radius))
				{
					//w0, w1
					//see which side is on the top:

					//check to see if moving towards the triangle?
					*out = v0 + t * dir01;// +EPSILON * normal;
					return true;
				}
				else
				{
					//*out = v1;
					return false;
				}
			}
		}
	}

	bool isPointInTriangle(glm::vec3 p, float radius)
	{
		//need to offset the geometry properly...
		float v0Offset = radius / glm::dot(n0, normal);
		float v1Offset = radius / glm::dot(n1, normal);
		float v2Offset = radius / glm::dot(n2, normal);

		glm::vec3 m0 = (vert1 + v1Offset * n1) - (vert0 + v0Offset * n0), m1 = (vert2 + v2Offset * n2) - (vert0 + v0Offset * n0), m2 = p - (vert0 + v0Offset * n0);
		float d00 = glm::dot(m0, m0);
		float d01 = glm::dot(m0, m1);
		float d11 = glm::dot(m1, m1);
		float d20 = glm::dot(m2, m0);
		float d21 = glm::dot(m2, m1);
		float denom = d00 * d11 - d01 * d01;
		float v = (d11 * d20 - d01 * d21) / denom;
		float w = (d00 * d21 - d01 * d20) / denom;
		float u = 1.0f - v - w;
		return (v >= 0.0 && v <= 1.0 && w >= 0.0 && w <= 1.0 && u >= 0.0 && u <= 1.0);
	}

	MeshTriangle applyMatrix(glm::mat4 matrix, glm::mat4 normalMatrix)
	{
		glm::vec4 vert0_ = matrix * glm::vec4(vert0, 1);
		glm::vec4 vert1_ = matrix * glm::vec4(vert1, 1);
		glm::vec4 vert2_ = matrix * glm::vec4(vert2, 1);

		glm::vec4 n0_ = normalMatrix * glm::vec4(n0, 0);
		glm::vec4 n1_ = normalMatrix * glm::vec4(n1, 0);
		glm::vec4 n2_ = normalMatrix * glm::vec4(n2, 0);
		
		return MeshTriangle(vert0_, vert1_, vert2_, n0_, n1_, n2_);
	}

	void thicken(float radius)
	{
		//need to offset the geometry properly...
		float v0Offset = radius / glm::dot(n0, normal);
		float v1Offset = radius / glm::dot(n1, normal);
		float v2Offset = radius / glm::dot(n2, normal);

		vert0 += v0Offset * n0;
		vert1 += v1Offset * n1;
		vert2 += v2Offset * n2;
	}

	MeshTriangle getThicker(float radius)
	{
		//need to offset the geometry properly...
		float v0Offset = radius / glm::dot(n0, normal);
		float v1Offset = radius / glm::dot(n1, normal);
		float v2Offset = radius / glm::dot(n2, normal);

		//std::cout << v0Offset << " : " << v1Offset << " : " << v2Offset << "\n";

		vert0 += v0Offset * n0;
		vert1 += v1Offset * n1;
		vert2 += v2Offset * n2;

		return MeshTriangle(vert0 + v0Offset * n0, vert1 + v1Offset * n1, vert2 + v2Offset * n2, n0, n1, n2);
	}
	
	void applyMatrixSelf(glm::mat4 matrix, glm::mat4 normalMatrix)
	{
		vert0 = matrix * glm::vec4(vert0, 1);
		vert1 = matrix * glm::vec4(vert1, 1);
		vert2 = matrix * glm::vec4(vert2, 1);

		n0 = normalMatrix * glm::vec4(n0, 0);
		n1 = normalMatrix * glm::vec4(n1, 0);
		n2 = normalMatrix * glm::vec4(n2, 0);
		normal = normalMatrix * glm::vec4(normal, 0);
	}
};

class Triangle
{
private:
	glm::vec3 calculateNormal(glm::vec3 vert0, glm::vec3 vert1, glm::vec3 vert2)
	{
		return glm::normalize(glm::cross(vert1 - vert0, vert2 - vert0));
	}

public:
	Triangle(glm::vec3 vert0, glm::vec3 vert1, glm::vec3 vert2)
	{
		this->vert0 = vert0;
		this->vert1 = vert1;
		this->vert2 = vert2;
		pos = glm::vec3((vert0.x + vert1.x + vert2.x) / 3, (vert0.y + vert1.y + vert2.y) / 3, (vert0.z + vert1.z + vert2.z) / 3);
		this->normal = calculateNormal(vert0, vert1, vert2);
		d = -glm::dot(normal, pos);
	}

	Triangle(glm::vec3 &vert0, glm::vec3 &vert1, glm::vec3 &vert2, glm::vec3 &n)
	{
		this->vert0 = vert0;
		this->vert1 = vert1;
		this->vert2 = vert2;
		pos = glm::vec3((vert0.x + vert1.x + vert2.x) / 3, (vert0.y + vert1.y + vert2.y) / 3, (vert0.z + vert1.z + vert2.z) / 3);
		this->normal = n;
		d = -glm::dot(n, pos);
	}

	/** /
	inline Triangle operator+(const glm::vec3& offSet)
	{
		vert0 += offSet;
		vert1 += offSet;
		vert2 += offSet;
		pos += offSet;
		return this;
	}*/

	Triangle& operator+(const glm::vec3& offSet)
	{
		vert0 += offSet;
		vert1 += offSet;
		vert2 += offSet;
		pos += offSet;
		return *this;
	}
	
	Triangle applyMatrix(glm::mat4 matrix)
	{
		glm::vec4 vert0_ = glm::vec4(vert0, 1);
		glm::vec4 vert1_ = glm::vec4(vert1, 1);
		glm::vec4 vert2_ = glm::vec4(vert2, 1);
		vert0_ = matrix * vert0_;
		vert1_ = matrix * vert1_;
		vert2_ = matrix * vert2_;
		Triangle nTri(vert0_ / vert0_.w, vert1_ / vert1_.w, vert2_ / vert2_.w);
		return nTri;
	}

	double getD()
	{
		//pos.x * normal.x + pos.y * normal.y + pos.z * normal.z + d = 0


		return -glm::dot(normal, pos);
	}

	float distanceFromPlan(glm::vec3 p)
	{
		return glm::dot(normal, p) + d;
	}

	//parameters
	glm::vec3 vert0;
	glm::vec3 vert1;
	glm::vec3 vert2;
	glm::vec3 pos;
	glm::vec3 normal;
	float d;
};

class Bounds
{
private:
	bool boundingBoxCheck(std::shared_ptr<Triangle> triangle)
	{
		double maxX = fmax(triangle->vert0.x, fmax(triangle->vert1.x, triangle->vert2.x));
		double minX = fmin(triangle->vert0.x, fmin(triangle->vert1.x, triangle->vert2.x));
		double maxY = fmax(triangle->vert0.y, fmax(triangle->vert1.y, triangle->vert2.y));
		double minY = fmin(triangle->vert0.y, fmin(triangle->vert1.y, triangle->vert2.y));
		double maxZ = fmax(triangle->vert0.z, fmax(triangle->vert1.z, triangle->vert2.z));
		double minZ = fmin(triangle->vert0.z, fmin(triangle->vert1.z, triangle->vert2.z));

		if (maxX >= low.x && minX <= high.x && maxY >= low.y && minY <= high.y && maxZ >= low.z && minZ <= high.z)
			return true;
		return false;
	}

	bool pointInRect(float px, float py, float lowX, float highX, float lowY, float highY)
	{
		return px >= lowX && px <= highX && py >= lowY && py <= highY;
	}

	bool lineIntersectsBoundaries(glm::vec3 vert0, glm::vec3 vert1)
	{
		//check all the planes...

		//Z:
		double zSlope = vert1.z - vert0.z;
		if (zSlope != 0.0)
		{
			//TOP
			double t1 = (high.z - vert0.z) / zSlope;
			if (t1 >= 0.0 && t1 <= 1.0)
			{
				double x = vert0.x + t1 * (vert1.x - vert0.x);
				double y = vert0.y + t1 * (vert1.y - vert0.y);
				if (x >= low.x && x <= high.x && y >= low.y && y <= high.y) return true;
			}
			//BOTTOM
			double t2 = (low.z - vert0.z) / zSlope;
			if (t2 >= 0.0 && t2 <= 1.0)
			{
				double x = vert0.x + t2 * (vert1.x - vert0.x);
				double y = vert0.y + t2 * (vert1.y - vert0.y);
				if (x >= low.x && x <= high.x && y >= low.y && y <= high.y) return true;
			}
		}

		//X:
		double xSlope = vert1.x - vert0.x;
		if (xSlope != 0.0)
		{
			//TOP
			double t1 = (high.x - vert0.x) / xSlope;
			if (t1 >= 0.0 && t1 <= 1.0)
			{
				double z = vert0.z + t1 * (vert1.z - vert0.z);
				double y = vert0.y + t1 * (vert1.y - vert0.y);
				if (z >= low.z && z <= high.z && y >= low.y && y <= high.y) return true;
			}
			//BOTTOM
			double t2 = (low.x - vert0.x) / xSlope;
			if (t2 >= 0.0 && t2 <= 1.0)
			{
				double z = vert0.z + t2 * (vert1.z - vert0.z);
				double y = vert0.y + t2 * (vert1.y - vert0.y);
				if (z >= low.z && z <= high.z && y >= low.y && y <= high.y) return true;
			}
		}

		//Y:
		double ySlope = vert1.y - vert0.y;
		if (ySlope != 0.0)
		{
			//TOP
			double t1 = (high.y - vert0.y) / ySlope;
			if (t1 >= 0.0 && t1 <= 1.0)
			{
				double z = vert0.z + t1 * (vert1.z - vert0.z);
				double x = vert0.x + t1 * (vert1.x - vert0.x);
				if (z >= low.z && z <= high.z && x >= low.x && x <= high.x) return true;
			}
			//BOTTOM
			double t2 = (low.y - vert0.y) / ySlope;
			if (t2 >= 0.0 && t2 <= 1.0)
			{
				double z = vert0.z + t2 * (vert1.z - vert0.z);
				double x = vert0.x + t2 * (vert1.x - vert0.x);
				if (z >= low.z && z <= high.z && x >= low.x && x <= high.x) return true;
			}
		}
		return false;
	}

	bool triangleIntersectsBoundaries(std::shared_ptr<Triangle> triangle)
	{
		if (lineIntersectsBoundaries(triangle->vert0, triangle->vert1) ||
			lineIntersectsBoundaries(triangle->vert1, triangle->vert2) ||
			lineIntersectsBoundaries(triangle->vert2, triangle->vert0)) return true;
		return false;
	}

	/*
	bool pointInTriangle(float px, float py, float p0x, float p0y, float vert0x, float vert0y, float vert1x, float vert1y)
	{
	float Area = (float)(0.5 *(-vert0y * vert1x + p0y * (-vert0x + vert1x) + p0x * (vert0y - vert1y) + vert0x * vert1y));
	float s = 1 / (2 * Area)*(p0y*vert1x - p0x * vert1y + (vert1y - p0y)*px + (p0x - vert1x)*py);
	float t = 1 / (2 * Area)*(p0x*vert0y - p0y * vert0x + (p0y - vert0y)*px + (vert0x - p0x)*py);
	return s >= 0 && t >= 0 && 1.0f - s - t >= 0;
	}
	*/

	void solveSystem(double a, double b, double c, double d, double e, double f, double *x, double *y)
	{
		*x = (c - ((b * (f - ((d*c) / a))) / (e - ((d*b) / a)))) / a;
		*y = (f - ((d*c) / a)) / (e - ((d*b) / a));
	}

	bool pointInTriangle(float x, float y, float p0x, float p0y, float vert0x, float vert0y, float vert1x, float vert1y)
	{
		double t = 0.0;
		double s = 0.0;
		solveSystem(vert0x - p0x, vert1x - p0x, x - p0x, vert0y - p0y, vert1y - p0y, y - p0y, &t, &s);
		if (t >= 0.0 && s >= 0.0 && t + s <= 1.0) return true;
		return false;
	}

	bool lineInteresectsLine(float A1x, float A1y, float A2x, float A2y, float B1x, float B1y, float B2x, float B2y)
	{
		if (B2x * A2y - B2y * A2x == 0.0) return false; // parallel
		float u = (A2x * (B1y - A1y) + A2y * (A1x - B1x)) / (B2x * A2y - B2y * A2x);
		float t = (B2x * (A1y - B1y) + B2y * (B1x - A1x)) / (B2x * A2y - B2y * A2x);
		if (u >= 0.0 && u <= 1.0 && t >= 0.0 && t <= 0.0) return true;
		return false;
	}

	bool triangleIntersectsLine(float p0x, float p0y, float vert0x, float vert0y, float vert1x, float vert1y, float A1x, float A1y, float A2x, float A2y)
	{
		if (lineInteresectsLine(p0x, p0y, vert0x, vert0y, A1x, A1y, A2x, A2y) ||
			lineInteresectsLine(vert0x, vert0y, vert1x, vert1y, A1x, A1y, A2x, A2y) ||
			lineInteresectsLine(vert1x, vert1y, p0x, p0y, A1x, A1y, A2x, A2y)) return true;
		return false;
	}

	bool triangleIntersectsRect(float p0x, float p0y, float vert0x, float vert0y, float vert1x, float vert1y, float lowX, float highX, float lowY, float highY)
	{
		if (triangleIntersectsLine(p0x, p0y, vert0x, vert0y, vert1x, vert1y, lowX, lowY, highX, lowY) ||
			triangleIntersectsLine(p0x, p0y, vert0x, vert0y, vert1x, vert1y, highX, lowY, highX, highY) ||
			triangleIntersectsLine(p0x, p0y, vert0x, vert0y, vert1x, vert1y, highX, highY, lowX, highY) ||
			triangleIntersectsLine(p0x, p0y, vert0x, vert0y, vert1x, vert1y, lowX, highY, lowX, lowY)) return true;
		return false;
	}

	bool triangleInRect(float p0x, float p0y, float vert0x, float vert0y, float vert1x, float vert1y, float lowX, float highX, float lowY, float highY)
	{
		//check all triangle verts in rect
		if (pointInRect(p0x, p0y, lowX, highX, lowY, highY)) return true;
		if (pointInRect(vert0x, vert0y, lowX, highX, lowY, highY)) return true;
		if (pointInRect(vert1x, vert1y, lowX, highX, lowY, highY)) return true;
		//check all rect verts in triangle
		if (pointInTriangle(lowX, lowY, p0x, p0y, vert0x, vert0y, vert1x, vert1y)) return true;
		if (pointInTriangle(lowX, highY, p0x, p0y, vert0x, vert0y, vert1x, vert1y)) return true;
		if (pointInTriangle(highX, lowY, p0x, p0y, vert0x, vert0y, vert1x, vert1y)) return true;
		if (pointInTriangle(highX, highY, p0x, p0y, vert0x, vert0y, vert1x, vert1y)) return true;
		//check if the triangle intersects the boundary of the rectangle
		if (triangleIntersectsRect(p0x, p0y, vert0x, vert0y, vert1x, vert1y, lowX, highX, lowY, highY)) return true;
		return false;
	}

	bool rectInTriangle(float p0x, float p0y, float vert0x, float vert0y, float vert1x, float vert1y, float lowX, float highX, float lowY, float highY)
	{
		//check all rect verts in triangle
		if (pointInTriangle(lowX, lowY, p0x, p0y, vert0x, vert0y, vert1x, vert1y)) return true;
		if (pointInTriangle(lowX, highY, p0x, p0y, vert0x, vert0y, vert1x, vert1y)) return true;
		if (pointInTriangle(highX, lowY, p0x, p0y, vert0x, vert0y, vert1x, vert1y)) return true;
		if (pointInTriangle(highX, highY, p0x, p0y, vert0x, vert0y, vert1x, vert1y)) return true;
		return false;
	}

	bool boundsInTriangle(std::shared_ptr<Triangle> triangle)
	{
		if (rectInTriangle(triangle->vert0.x, triangle->vert0.y, triangle->vert1.x, triangle->vert1.y, triangle->vert2.x, triangle->vert2.y, low.x, high.x, low.y, high.y) &&
			rectInTriangle(triangle->vert0.y, triangle->vert0.z, triangle->vert1.y, triangle->vert1.z, triangle->vert2.y, triangle->vert2.z, low.y, high.y, low.z, high.z) &&
			rectInTriangle(triangle->vert0.z, triangle->vert0.x, triangle->vert1.z, triangle->vert1.x, triangle->vert2.z, triangle->vert2.x, low.z, high.z, low.x, high.x)) return true;
		return false;
	}

	bool triangleInBounds(std::shared_ptr<Triangle> triangle)
	{
		//check if bounding box intersects
		//	--HAVE--
		if (boundingBoxCheck(triangle))
		{
			//better one...
	//		if (pointInBounds(triangle->vert0) ||
	//			pointInBounds(triangle->vert1) ||
	//			pointInBounds(triangle->vert2)) return true;

	//		if (boundsInTriangle(triangle)) return true;

	//		if (triangleIntersectsBoundaries(triangle)) return true;
			//check projections: (true for all)
			//	See if the vertexes of triangle are in the rectangle
			//	if (triangleInRect(triangle.vert0.x, triangle.vert0.y, triangle.vert1.x, triangle.vert1.y, triangle.vert2.x, triangle.vert2.y, low.x, high.x, low.y, high.y) &&
			//		triangleInRect(triangle.vert0.y, triangle.vert0.z, triangle.vert1.y, triangle.vert1.z, triangle.vert2.y, triangle.vert2.z, low.y, high.y, low.z, high.z) &&
			//		triangleInRect(triangle.vert0.z, triangle.vert0.x, triangle.vert1.z, triangle.vert1.x, triangle.vert2.z, triangle.vert2.x, low.z, high.z, low.x, high.x)) return true;
			return true;
			//maybe good enough
		}
		return false;
	}

public:
	glm::vec3 low;
	glm::vec3 high;
	Bounds() {};
	Bounds(glm::vec3 low, glm::vec3 high)
	{
		this->low = low;
		this->high = high;
	}

	void copy(Bounds bounds) 
	{
		this->low.x = bounds.low.x;
		this->low.y = bounds.low.y;
		this->low.z = bounds.low.z;
		this->high.x = bounds.high.x;
		this->high.y = bounds.high.y;
		this->high.z = bounds.high.z;
	}

	bool pointInBounds(glm::vec3 p)
	{
		return p.x >= low.x && p.x <= high.x && p.y >= low.y && p.y <= high.y && p.z >= low.z && p.z <= high.z;
	}

	std::vector<std::shared_ptr<Triangle>> trianglesInBounds(std::vector<std::shared_ptr<Triangle>> triangles)
	{
		std::vector<std::shared_ptr<Triangle>> out;
		for (int i = 0; i < triangles.size(); i++)
		{
			if (triangleInBounds(triangles[i]))
			{
				out.push_back(triangles[i]);
			}
		}
		return out;
	//	return triangles;
	}

	Bounds applyMatrix(glm::mat4 matrix)
	{
		glm::vec4 vLLL = glm::vec4(low.x,	low.y,	low.z,	1);
		glm::vec4 vLLH = glm::vec4(low.x,	low.y,	high.z,	1);
		glm::vec4 vLHL = glm::vec4(low.x, high.y,	low.z,	1);
		glm::vec4 vLHH = glm::vec4(low.x, high.y, high.z,	1);
		glm::vec4 vHLL = glm::vec4(high.x,	low.y,	low.z,	1);
		glm::vec4 vHLH = glm::vec4(high.x,	low.y, high.z,	1);
		glm::vec4 vHHL = glm::vec4(high.x, high.y,	low.z,	1);
		glm::vec4 vHHH = glm::vec4(high.x, high.y, high.z,	1);


		vLLL = matrix * vLLL;
		vLLH = matrix * vLLH;
		vLHL = matrix * vLHL;
		vLHH = matrix * vLHH;
		vHLL = matrix * vHLL;
		vHLH = matrix * vHLH;
		vHHL = matrix * vHHL;
		vHHH = matrix * vHHH;

		vLLL /= vLLL.w;
		vLLH /= vLLH.w;
		vLHL /= vLHL.w;
		vLHH /= vLHH.w;
		vHLL /= vHLL.w;
		vHLH /= vHLH.w;
		vHHL /= vHHL.w;
		vHHH /= vHHH.w;	
		
		glm::vec4 low_ = glm::min(glm::min(glm::min(glm::min(glm::min(glm::min(glm::min(vLLL, vLLH), vLHL), vLHH), vHLL), vHLH), vHHL), vHHH);
		glm::vec4 high_ = glm::max(glm::max(glm::max(glm::max(glm::max(glm::max(glm::max(vLLL, vLLH), vLHL), vLHH), vHLL), vHLH), vHHL), vHHH);
		return Bounds(low_, high_);
	}

	static Bounds boundsFromTriangles(std::vector<std::shared_ptr<Triangle>> triangles)
	{
		Bounds bounds;
		
		if (triangles.size() == 0) return bounds;

		bounds.low.x =	triangles[0]->vert0.x;
		bounds.high.x = triangles[0]->vert0.x;
		bounds.low.y =	triangles[0]->vert0.y;
		bounds.high.y = triangles[0]->vert0.y;
		bounds.low.z =	triangles[0]->vert0.z;
		bounds.high.z = triangles[0]->vert0.z;

		for (int i = 0; i < triangles.size(); i++)
		{
			if (triangles[i]->vert0.x < bounds.low.x) bounds.low.x =	triangles[i]->vert0.x;
			if (triangles[i]->vert0.x > bounds.high.x) bounds.high.x = triangles[i]->vert0.x;
			if (triangles[i]->vert0.y < bounds.low.y) bounds.low.y =	triangles[i]->vert0.y;
			if (triangles[i]->vert0.y > bounds.high.y) bounds.high.y = triangles[i]->vert0.y;
			if (triangles[i]->vert0.z < bounds.low.z) bounds.low.z =	triangles[i]->vert0.z;
			if (triangles[i]->vert0.z > bounds.high.z) bounds.high.z = triangles[i]->vert0.z;

			if (triangles[i]->vert1.x < bounds.low.x) bounds.low.x =	triangles[i]->vert1.x;
			if (triangles[i]->vert1.x > bounds.high.x) bounds.high.x = triangles[i]->vert1.x;
			if (triangles[i]->vert1.y < bounds.low.y) bounds.low.y =	triangles[i]->vert1.y;
			if (triangles[i]->vert1.y > bounds.high.y) bounds.high.y = triangles[i]->vert1.y;
			if (triangles[i]->vert1.z < bounds.low.z) bounds.low.z =	triangles[i]->vert1.z;
			if (triangles[i]->vert1.z > bounds.high.z) bounds.high.z = triangles[i]->vert1.z;

			if (triangles[i]->vert2.x < bounds.low.x) bounds.low.x =	triangles[i]->vert2.x;
			if (triangles[i]->vert2.x > bounds.high.x) bounds.high.x = triangles[i]->vert2.x;
			if (triangles[i]->vert2.y < bounds.low.y) bounds.low.y =	triangles[i]->vert2.y;
			if (triangles[i]->vert2.y > bounds.high.y) bounds.high.y = triangles[i]->vert2.y;
			if (triangles[i]->vert2.z < bounds.low.z) bounds.low.z =	triangles[i]->vert2.z;
			if (triangles[i]->vert2.z > bounds.high.z) bounds.high.z = triangles[i]->vert2.z;
		}
		return bounds;
	}
};

class Geometry
{
public:
	static double distanceFromQuad(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d)
	{
		glm::vec3 ba = b - a; glm::vec3 pa = p - a;
		glm::vec3 cb = c - b; glm::vec3 pb = p - b;
		glm::vec3 dc = d - c; glm::vec3 pc = p - c;
		glm::vec3 ad = a - d; glm::vec3 pd = p - d;
		glm::vec3 nor = cross(ba, ad);

		return sqrt(
			(glm::sign(glm::dot(cross(ba, nor), pa)) +
				glm::sign(glm::dot(cross(cb, nor), pb)) +
				glm::sign(glm::dot(cross(dc, nor), pc)) +
				glm::sign(glm::dot(cross(ad, nor), pd)) < 3.0)
			?
			glm::min(glm::min(glm::min(
				glm::length(ba*glm::clamp(glm::dot(ba, pa) / glm::length(ba), 0.0f, 1.0f) - pa),
				glm::length(cb*glm::clamp(glm::dot(cb, pb) / glm::length(cb), 0.0f, 1.0f) - pb)),
				glm::length(dc*glm::clamp(glm::dot(dc, pc) / glm::length(dc), 0.0f, 1.0f) - pc)),
				glm::length(ad*glm::clamp(glm::dot(ad, pd) / glm::length(ad), 0.0f, 1.0f) - pd))
			:
			glm::dot(nor, pa)*glm::dot(nor, pa) / glm::length(nor));
	}
	
	static double distanceFromCuboid(glm::vec3 p, glm::vec3 ftr, glm::vec3 ftl, glm::vec3 fbr, glm::vec3 fbl, glm::vec3 btr, glm::vec3 btl, glm::vec3 bbr, glm::vec3 bbl)
	{
		double front = distanceFromQuad(p, ftr, ftl, fbr, fbl);
		double back = distanceFromQuad(p, btr, btl, bbr, bbl);
		double left = distanceFromQuad(p, ftl, fbl, btl, bbl);
	
	}

	static double distanceFromTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
	{
		glm::vec3 ba = b - a; glm::vec3 pa = p - a;
		glm::vec3 cb = c - b; glm::vec3 pb = p - b;
		glm::vec3 ac = a - c; glm::vec3 pc = p - c;
		glm::vec3 nor = cross(ba, ac);

		return sqrt(
			(glm::sign(glm::dot(glm::cross(ba, nor), pa)) +
				glm::sign(glm::dot(glm::cross(cb, nor), pb)) +
				glm::sign(glm::dot(glm::cross(ac, nor), pc)) < 2.0)
			?
			glm::min(glm::min(
				glm::length(ba*glm::clamp(glm::dot(ba, pa) / glm::length(ba), 0.0f, 1.0f) - pa),
				glm::length(cb*glm::clamp(glm::dot(cb, pb) / glm::length(cb), 0.0f, 1.0f) - pb)),
				glm::length(ac*glm::clamp(glm::dot(ac, pc) / glm::length(ac), 0.0f, 1.0f) - pc))
			:
			glm::dot(nor, pa)*glm::dot(nor, pa) / glm::length(nor));
	}

	static double distanceFromCapsule()
	{

	}

	static float lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}

	static glm::vec3 calculateNormal(glm::vec3 vert0, glm::vec3 vert1, glm::vec3 vert2)
	{
		return glm::normalize(glm::cross(vert1 - vert0, vert2 - vert0));
	}

};

class Frustum
{
public:
	Frustum()
	{
	}

	Frustum(glm::mat4 frustumProjection)
	{
		frustum = frustumProjection;
		plane.resize(6);	
		points.resize(8);
		glm::inverse(frustumProjection);
		//frustum into view space
		//we get the vertexes of a regular cube, and then project it into the frustm
		glm::vec3 vLLL = glm::vec3(-1, -1, -1);
		glm::vec3 vLLH = glm::vec3(-1, -1,	1);
		glm::vec3 vLHL = glm::vec3(-1,	1, -1);
		glm::vec3 vLHH = glm::vec3(-1,	1,	1);
		glm::vec3 vHLL = glm::vec3( 1, -1, -1);
		glm::vec3 vHLH = glm::vec3( 1, -1,	1);
		glm::vec3 vHHL = glm::vec3( 1,	1, -1);
		glm::vec3 vHHH = glm::vec3( 1,	1,	1);
		
	//	Triangle LY(vLLL, vLLH, vHLL);
	//	LY = LY.applyMatrix(frustum);
	
	//	Triangle HY(vLHL, vHHL, vLHH);
	//	HY = HY.applyMatrix(frustum);
	
		Triangle LZ(vLLL, vLHL, vHLL);
		LZ = LZ.applyMatrix(frustum);
		plane[0] = glm::vec4(LZ.normal, LZ.d);

		Triangle HZ(vLLH, vLHH, vHLH);
		HZ = HZ.applyMatrix(frustum);
		plane[1] = glm::vec4(HZ.normal, HZ.d);

		Triangle LX(vLLL, vLHL, vLLH);
		LX = LX.applyMatrix(frustum);
		plane[2] = glm::vec4(LX.normal, LX.d);

		Triangle HX(vHLL, vHLH, vHHL);
		HX = HX.applyMatrix(frustum);
		plane[3] = glm::vec4(HX.normal, HX.d);

		Triangle LY(vHLH, vLLH, vHLL);
		LY = LY.applyMatrix(frustum);
		plane[4] = glm::vec4(LY.normal, LY.d);

		Triangle HY(vHHH, vHHL, vLHH);
		HY = HY.applyMatrix(frustum);
		plane[5] = glm::vec4(HY.normal, HY.d);


		glm::vec4 vLLL_ = frustum * glm::vec4(vLLL, 1);
		glm::vec4 vLLH_ = frustum * glm::vec4(vLLH, 1);
		glm::vec4 vLHL_ = frustum * glm::vec4(vLHL, 1);
		glm::vec4 vLHH_ = frustum * glm::vec4(vLHH, 1);
		glm::vec4 vHLL_ = frustum * glm::vec4(vHLL, 1);
		glm::vec4 vHLH_ = frustum * glm::vec4(vHLH, 1);
		glm::vec4 vHHL_ = frustum * glm::vec4(vHHL, 1);
		glm::vec4 vHHH_ = frustum * glm::vec4(vHHH, 1);
	
		vLLL = glm::vec3(vLLL_) / vLLL_.w;
		vLLH = glm::vec3(vLLH_) / vLLH_.w;
		vLHL = glm::vec3(vLHL_) / vLHL_.w;
		vLHH = glm::vec3(vLHH_) / vLHH_.w;
		vHLL = glm::vec3(vHLL_) / vHLL_.w;
		vHLH = glm::vec3(vHLH_) / vHLH_.w;
		vHHL = glm::vec3(vHHL_) / vHHL_.w;
		vHHH = glm::vec3(vHHH_) / vHHH_.w;


		points[0] = vLLL;
		points[1] = vLLH;
		points[2] = vLHL;
		points[3] = vLHH;
		points[4] = vHLL;
		points[5] = vHLH;
		points[6] = vHHL;
		points[7] = vHHH;

		//glm::vec3 tempNorm;


		//tempNorm = glm::normalize(glm::cross(vLLL - vHLL, vHHL - vHLL));
		//plane[0] = glm::vec4(tempNorm, );


		//front
	//	tempNorm = glm::normalize(glm::cross(vHHH - vLHH, vLLH - vLHH));
	//	plane[0] = glm::vec4(tempNorm, glm::dot(tempNorm, vLHH));
	//	
	//	tempNorm = glm::normalize(glm::cross(vLHH - vLLH, vLLL - vLLH));
	//	plane[1] = glm::vec4(tempNorm, glm::dot(tempNorm, vLLH));
	//	
	//	tempNorm = glm::normalize(glm::cross(vHHL - vHLL, vLLL - vHLL));
	//	plane[2] = glm::vec4(tempNorm, glm::dot(tempNorm, vHLL));
	//
	//	tempNorm = glm::normalize(glm::cross(vHLH - vHHL, vHHH - vHHL));
	//	plane[3] = glm::vec4(tempNorm, glm::dot(tempNorm, vHHL));
	//
	//	tempNorm = glm::normalize(glm::cross(vLHH - vHHH, vHHL - vHHH));
	//	plane[0] = glm::vec4(tempNorm, glm::dot(tempNorm, vHHH));
	//
	//	tempNorm = glm::normalize(glm::cross(vLLH - vLLL, vHLL - vLLL));
	//	plane[5] = glm::vec4(tempNorm, glm::dot(tempNorm, vLLL));
	}

	float distanceFromPlane(glm::vec4 face, glm::vec4 p)
	{
		return glm::dot(face, p);// +face.w;
	}

	bool viewSpaceBoxInFrustum(glm::mat4 MVMatrix, Bounds bounds)
	{
		
		//int i = 0;
		//int out = 0;
		//out += ((distanceFromPlane(plane[i], glm::vec4(bounds.low.x, bounds.low.y, bounds.low.z,		1.0f)) < 0.0) ? 1 : 0);
		//out += ((distanceFromPlane(plane[i], glm::vec4(bounds.high.x, bounds.low.y, bounds.low.z,		1.0f)) < 0.0) ? 1 : 0);
		//out += ((distanceFromPlane(plane[i], glm::vec4(bounds.low.x, bounds.high.y, bounds.low.z,		1.0f)) < 0.0) ? 1 : 0);
		//out += ((distanceFromPlane(plane[i], glm::vec4(bounds.high.x, bounds.high.y, bounds.low.z,		1.0f)) < 0.0) ? 1 : 0);
		//out += ((distanceFromPlane(plane[i], glm::vec4(bounds.low.x, bounds.low.y, bounds.high.z,		1.0f)) < 0.0) ? 1 : 0);
		//out += ((distanceFromPlane(plane[i], glm::vec4(bounds.high.x, bounds.low.y, bounds.high.z,		1.0f)) < 0.0) ? 1 : 0);
		//out += ((distanceFromPlane(plane[i], glm::vec4(bounds.low.x, bounds.high.y, bounds.high.z,		1.0f)) < 0.0) ? 1 : 0);
		//out += ((distanceFromPlane(plane[i], glm::vec4(bounds.high.x, bounds.high.y, bounds.high.z,		1.0f)) < 0.0) ? 1 : 0);
		//if (out == 8) return false;
		//return true;

	//	return false;

		bounds = bounds.applyMatrix(frustum * MVMatrix);
	//	std::cout << "low : " << (bounds.low.z) << "\thigh : " << (bounds.high.z) << "\n";
		if (bounds.low.x <= 1 && bounds.low.y <= 1 && bounds.low.z <= 1 && bounds.high.x >= -1 && bounds.high.y >= -1 && bounds.high.z >= -1)
		//if (bounds.low.z <= 1 && bounds.high.z >= -1)
		{
			return true;
		}
		return false;


		//return true;

		///*

	//	bounds = bounds.applyMatrix(MVMatrix);
	//
	//	int frustumPlanes = 6;
	//	int pointsNum = 0;
	//	bool do_second = false;
	//
	//	// check box outside/inside of frustum
	//	for (int i = 0; i < frustumPlanes; i++)
	//	{
	//		int out = 0;
	//		out += ((glm::dot(plane[i], glm::vec4(bounds.low.x, bounds.low.y,	bounds.low.z	, 1.0f)) < 0.0) ? 1 : 0);
	//		out += ((glm::dot(plane[i], glm::vec4(bounds.high.x, bounds.low.y,	bounds.low.z	, 1.0f)) < 0.0) ? 1 : 0);
	//		out += ((glm::dot(plane[i], glm::vec4(bounds.low.x, bounds.high.y,	bounds.low.z	, 1.0f)) < 0.0) ? 1 : 0);
	//		out += ((glm::dot(plane[i], glm::vec4(bounds.high.x, bounds.high.y,	bounds.low.z	, 1.0f)) < 0.0) ? 1 : 0);
	//		out += ((glm::dot(plane[i], glm::vec4(bounds.low.x, bounds.low.y,	bounds.high.z	, 1.0f)) < 0.0) ? 1 : 0);
	//		out += ((glm::dot(plane[i], glm::vec4(bounds.high.x, bounds.low.y,	bounds.high.z	, 1.0f)) < 0.0) ? 1 : 0);
	//		out += ((glm::dot(plane[i], glm::vec4(bounds.low.x, bounds.high.y,	bounds.high.z	, 1.0f)) < 0.0) ? 1 : 0);
	//		out += ((glm::dot(plane[i], glm::vec4(bounds.high.x, bounds.high.y,	bounds.high.z	, 1.0f)) < 0.0) ? 1 : 0);
	//		if (out == 8) return false;
	//	}
	//
	//	// check frustum outside/inside box
	//	if (do_second)
	//	{
	//		int out;
	//		out = 0; for (int i = 0; i < pointsNum; i++) out += ((points[i].x > bounds.high.x) ? 1 : 0); if (out == 8) return false;
	//		out = 0; for (int i = 0; i < pointsNum; i++) out += ((points[i].x < bounds.low.x) ? 1 : 0); if (out == 8) return false;
	//		out = 0; for (int i = 0; i < pointsNum; i++) out += ((points[i].y > bounds.high.y) ? 1 : 0); if (out == 8) return false;
	//		out = 0; for (int i = 0; i < pointsNum; i++) out += ((points[i].y < bounds.low.y) ? 1 : 0); if (out == 8) return false;
	//		out = 0; for (int i = 0; i < pointsNum; i++) out += ((points[i].z > bounds.high.z) ? 1 : 0); if (out == 8) return false;
	//		out = 0; for (int i = 0; i < pointsNum; i++) out += ((points[i].z < bounds.low.z) ? 1 : 0); if (out == 8) return false;
	//	}
	//	
	//	return true;

		//*/
		//return true;



		/*
		glm::vec3 mins = bounds.low;
		glm::vec3 maxs = bounds.high;
		glm::vec3 vmin, vmax;
		glm::vec3 normal;

		for (int i = 0; i < 1; ++i) {//6
			// X axis 
			normal = glm::vec3(plane[i]);
			if (normal.x > 0) {
				vmin.x = mins.x;
				vmax.x = maxs.x;
			}
			else {
				vmin.x = maxs.x;
				vmax.x = mins.x;
			}
			// Y axis 
			if (normal.y > 0) {
				vmin.y = mins.y;
				vmax.y = maxs.y;
			}
			else {
				vmin.y = maxs.y;
				vmax.y = mins.y;
			}
			// Z axis 
			if (normal.z > 0) {
				vmin.z = mins.z;
				vmax.z = maxs.z;
			}
			else {
				vmin.z = maxs.z;
				vmax.z = mins.z;
			}
			if (glm::dot(normal, vmin) + plane[i].w > 0)
				return false;
			//if (Vector::DotProduct(planes[i].normal, vmax) + planes[i].d >= 0)
			//	ret = INTERSECT;
		}
		return true;
	//	*/
	}

	bool viewSpaceSphereInFrustum(glm::vec3 center, float radius)
	{
		for (int i = 0; i < 6; ++i) {
			if (glm::dot(glm::vec3(plane[i]), center) + plane[i].w - radius > 0)
  				return false;
		}
		return true;
	}

	std::vector<glm::vec4> plane;
	std::vector<glm::vec3> points;
	glm::mat4 frustum;

};
