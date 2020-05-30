#pragma once
#include "Header.h"

class MeshTriangle
{
private:
	glm::vec3 calculateNormal(glm::vec3 vert0, glm::vec3 vert1, glm::vec3 vert2)
	{
		return glm::normalize(glm::cross(vert1 - vert0, vert2 - vert0));
	}

	const float EPSILON = 0.001;

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
		this->d = -glm::dot(vert0, normal);
		this->min = glm::min(vert0, glm::min(vert1, vert2));
		this->max = glm::max(vert0, glm::max(vert1, vert2));
		this->center = (1.0f / 3.0f) * (vert0 + vert1 + vert2);
	}

	float d;
	glm::vec3 vert0, vert1, vert2;
	glm::vec3 n0, n1, n2;
	glm::vec3 normal;
	glm::vec3 min, max;
	glm::vec3 center;

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

	bool collide(float radius, glm::vec3 v0, glm::vec3 v1, float * alongVec, glm::vec3* ptOnSurface, glm::vec3* goal)
	{
		glm::vec3 dif = v1 - v0;

		float dirFactor = glm::dot(dif, normal);
		if (dirFactor >= 0.0) return false; //Parallel OR moving away OR v1 - v0 == 0

		glm::vec3 dir = glm::normalize(dif);
		
		v0 -= EPSILON * dir;
		dif += EPSILON * dir;



		glm::vec4 intersection = PointinTriangleOffset(v0, dif, radius);


		//intersection = collideFace(v0, dif, 0.0);

		if (intersection.w >= 0.0 && intersection.w <= 1.0)
		{
			//collisions
			glm::vec3 norm = glm::normalize(intersection.xyz());
			glm::vec3 offset = glm::vec3(0.0);// -EPSILON * glm::normalize(dif);   //EPSILON * intersection.xyz();
			glm::vec3 intPt = v0 + intersection.w * dif + offset;

			glm::vec3 remainder = v1 - intPt - EPSILON * dir;// -expandCorrection;

			//glm::vec3 perp = glm::dot(remainder, norm) * norm;
			glm::vec3 perp = glm::dot(remainder, norm) * norm;

			glm::vec3 par = remainder - perp;

			*ptOnSurface = intPt;
			*goal = intPt + par;
			*alongVec = intersection.w;
			return true;
		}
		else
		{
			//nothing...
			return false;
		}


		/* THIS WORKS FOR NON OFFSET SURFACE
		glm::vec3 dif = v1 - v0;
		
		float dirFactor = glm::dot(dif, normal);
		if (dirFactor >= 0.0) return false; //Parallel OR moving away.
		glm::vec3 nPt = vert0;// RADIUS DISABLED +radius * normal;

		float nd = -glm::dot(nPt, normal);

		float d0 = glm::dot(v0, normal) + nd;// d;
		float d1 = glm::dot(v1, normal) + nd;// d;
		float t = -d0 / (d1 - d0);

		if (t < 0.0 || t > 1.0) return false;

		glm::vec3 intPt = v0 + t * dif;
		
		// RADIUS DISABLED if(ptInTriangle(intPt, radius))
		if (ptInTriangle(intPt))
		{
			//collisions
			
			glm::vec3 remainder = v1 - intPt;

			glm::vec3 perp = glm::dot(remainder, normal) * normal;

			glm::vec3 par = remainder - perp;

			*ptOnSurface = intPt + EPSILON * normal;
			*goal = intPt + par + EPSILON * normal;
			*alongVec = t;
			return true;
		}
		else
		{
			//nothing...
			return false;
		}

		*/
		
		
		/*
		glm::mat4 transform(1.0f);

		glm::vec3 dir01 = v1 - v0;
		float denom = glm::dot(dir01, normal);
		if (denom >= 0.0) return false; //Parallel OR moving away.

		float d0 = glm::dot(v0, normal) + d;
		float d1 = glm::dot(v1, normal) + d;
		float t = -d0 / (d1 - d0);

		//float t = glm::dot(vert0 - v0, normal) / denom;
		glm::vec3 intPt = v0 + t * dir01;

		//Need another quick culling check.... Or maybe it will be fine because the Octree will cull...

		//now get it's barycentric coords... clamped to the edge if need be.
		glm::vec3 baryPt = getClampedBarycentricPt(intPt);

		glm::vec3 trianglePt = baryPt.x * vert0 + baryPt.y * vert1 + baryPt.z * vert2;

		//Do a check:
		glm::vec3 testPt = getBarycentricPt(trianglePt);

		//now we have the closest point on the triangle to the line...
		//now we get the closest point on the line to the triangle...
		t = glm::clamp(t, 0.0f, 1.0f);
		glm::vec3 linePt = v0 + t * dir01;


		glm::vec3 dif = linePt - trianglePt;


		//We need to send the point back if it get's to close... Basically so the distance is at least radius away
		float distance = glm::length(dif);
		glm::vec3 dir = glm::normalize(dif);


		if (radius - distance >= 0.0f)
		{

			//need to get projected radius...

			//gets how much the direction from the collision point is the same as the direction of movement...
			float dirFactor = glm::dot(glm::normalize(dir), glm::normalize(dir01)) + 1;

			
			//so.. we use...

			//std::cout << "yeet\n";
			//std::cout << baryPt.x << " : " << baryPt.y << " : " << baryPt.z << " \n";
			//std::cout << testPt.x << " : " << testPt.y << " : " << testPt.z << " \n";
			//std::cout << intPt.x << " : " << intPt.y << " : " << intPt.z << " \n";
			//std::cout << closestPt.x << " : " << closestPt.y << " : " << closestPt.z << " \n";
			//std::cout << "y : " << vert0.y << "\n";


			float distanceToMoveBack = fmax(radius - distance, 0.0);

			//go to the end and work back
			//glm::vec3 newPt = v1 - distanceToMoveBack * (1.0f / glm::length(dir01)) * dir01;



			//not dir01,,,
			// dir closet -> line

			//glm::vec3 newPt = linePt - radius * dir01;// glm::normalize(linePt - closestPt);
			//glm::vec3 newPt = intPt + (radius - distance) * dir;//closestPt + (radius - distance) * dir;
			glm::vec3 newPt = linePt - dirFactor * radius * dir01;


			*out = newPt;

			return true;// distanceToMoveBack > 0.0;
		}
		else
		{
			return false;
		}
		*/
		/*

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

			//float denom = glm::dot(dir01, normal);

			if (denom == 0.0) // parallel Not sure this can happen.... ???
			{
				//*out = v0;
				return false;
			}
			else
			{
				//float t = glm::dot(vert0 + radius * normal - v0, normal) / denom; // I'm like 90% sure this will always be valid here due to above checks ( valid :=  t<=1.0 && t >+ 0.0 )
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
		*/
	}

	bool rayIntersectsTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3* ptOnSurface, glm::vec3* remainder)
	{
		glm::vec3 dif = v1 - v0;

		float dirFactor = glm::dot(dif, normal);
		
		glm::vec3 dir = glm::normalize(dif);

		glm::vec4 intersection = intersectFace(v0, dif);

		if (intersection.w >= 0.0 && intersection.w <= 1.0)
		{
			//collisions
			glm::vec3 intPt = v0 + intersection.w * dif;
			*remainder = v1 - intPt;
			*ptOnSurface = intPt;
			return true;
		}
		else
		{
			//nothing...
			return false;
		}
	}

	glm::vec4 intersectFace(glm::vec3 pt, glm::vec3 dif)
	{
		//This should work... Maybe not though
		float d0 = glm::dot(pt, normal) + d;
		float d1 = glm::dot(pt + dif, normal) + d;
		//Returns when it crosses the plane...
		float t = -d0 / (d1 - d0);
		//test if it does collie the face... not just the plane.
		//maybe let's just...
		//bool ptIn = ptInTriangle(pt + t * dif);//ptInTriangleProject(pt + t * dif);
		//std::cout << ptIn << "\n";
		pt += t * dif;

		glm::vec3 e01 = vert1 - vert0, e12 = vert2 - vert1, e20 = vert0 - vert2;
		glm::vec3 d0p = pt - vert0, d1p = pt - vert1, d2p = pt - vert2;
		glm::vec3 sideEdge01 = glm::cross(e01, d0p);
		glm::vec3 sideEdge12 = glm::cross(e12, d1p);
		glm::vec3 sideEdge20 = glm::cross(e20, d2p);

		float side0 = glm::dot(normal, sideEdge01);
		float side1 = glm::dot(normal, sideEdge12);
		float side2 = glm::dot(normal, sideEdge20);

		if (t >= 0.0 && t <= 1.0 && side0 >= 0.0 && side1 >= 0.0 && side2 >= 0.0)
		{
			return glm::vec4(normal, t);
		}
		else
		{
			return glm::vec4(0, 0, 0, 100.0); // return a large number because we will use a min fucntion later
		}
	}

	bool ptInTriangle(glm::vec3 pt)
	{
		glm::vec3 e01 = vert1 - vert0,	e12 = vert2 - vert1,	e20 = vert0 - vert2;
		glm::vec3 d0p = pt - vert0,		d1p = pt - vert1,		d2p = pt - vert2;
		glm::vec3 sideEdge01 = glm::cross(e01, d0p);
		glm::vec3 sideEdge12 = glm::cross(e12, d1p);
		glm::vec3 sideEdge20 = glm::cross(e20, d2p);
		
		float side0 = glm::dot(normal, sideEdge01);
		float side1 = glm::dot(normal, sideEdge12);
		float side2 = glm::dot(normal, sideEdge20);

		return side0 >= 0.0 && side1 >= 0.0 && side2 >= 0.0;
		
		//float side01 = glm::dot(sideEdge01, e20);
		//float side12 = glm::dot(sideEdge12, e01);
		//float side20 = glm::dot(sideEdge20, e12);
		/*
		float side01 = glm::dot(d0p, e20);
		float side12 = glm::dot(d1p, e01);
		float side20 = glm::dot(d2p, e12);

		/*
		if (side01 <= 0.0 && side12 <= 0.0 && side20 <= 0.0)
			return true;
		else
			return false;
		* /

		glm::vec3 v01 = vert1 - vert0;
		glm::vec3 v02 = vert2 - vert0;
		glm::vec3 p0 = vert0 - pt;
		glm::vec3 p1 = vert1 - pt;
		glm::vec3 p2 = vert2 - pt;

		float AREAABC = glm::length(glm::cross(v01, v02)) / 2.0;
		float alpha = glm::length(glm::cross(p1, p2)) / (2.0 * AREAABC);
		float beta = glm::length(glm::cross(p2, p0)) / (2.0 * AREAABC);
		float gamma = 1 - alpha - beta;
		return alpha >= 0.0 && alpha <= 1.0 && beta >= 0.0 && beta <= 1.0 && gamma >= 0.0 && gamma <= 1.0;
		*/

		

	}

	bool ptInTriangleProject(glm::vec3 pt)
	{
		pt = pt - glm::dot(pt - vert0, normal) * normal;

		glm::vec3 e01 = vert1 - vert0, e12 = vert2 - vert1, e20 = vert0 - vert2;
		glm::vec3 d0p = pt - vert0, d1p = pt - vert1, d2p = pt - vert2;
		glm::vec3 sideEdge01 = glm::cross(e01, d0p);
		glm::vec3 sideEdge12 = glm::cross(e12, d1p);
		glm::vec3 sideEdge20 = glm::cross(e20, d2p);

		float side0 = glm::dot(normal, sideEdge01);
		float side1 = glm::dot(normal, sideEdge12);
		float side2 = glm::dot(normal, sideEdge20);

	//	std::cout << side0 << " : " << side1 << " : " << side2 << "\n";

		return side0 >= 0.0 && side1 >= 0.0 && side2 >= 0.0;
	}

	bool ptInTriangle(glm::vec3 pt, float radius)
	{
		glm::vec3 v0Offset = (radius / glm::dot(n0, normal)) * n0;
		glm::vec3 v1Offset = (radius / glm::dot(n1, normal)) * n1;
		glm::vec3 v2Offset = (radius / glm::dot(n2, normal)) * n2;

		glm::vec3 e01 = (vert1 + v1Offset) - (vert0 + v0Offset), e12 = (vert2 + v2Offset) - (vert1 + v1Offset), e20 = (vert0 + v0Offset) - (vert2 + v2Offset);
		glm::vec3 d0p = pt - (vert0 + v0Offset), d1p = pt - (vert1 + v1Offset), d2p = pt - (vert2 + v2Offset);
		glm::vec3 sideEdge01 = glm::cross(e01, d0p);
		glm::vec3 sideEdge12 = glm::cross(e12, d1p);
		glm::vec3 sideEdge20 = glm::cross(e20, d2p);

		float side0 = glm::dot(normal, sideEdge01);
		float side1 = glm::dot(normal, sideEdge12);
		float side2 = glm::dot(normal, sideEdge20);

		return side0 >= 0.0 && side1 >= 0.0 && side2 >= 0.0;
	}

	bool ptInOffsetTriangle(glm::vec3 pt, float radius)
	{
		glm::vec3 v0Offset = (radius / glm::dot(n0, normal)) * n0;
		glm::vec3 v1Offset = (radius / glm::dot(n1, normal)) * n1;
		glm::vec3 v2Offset = (radius / glm::dot(n2, normal)) * n2;

		glm::vec3 e01 = (vert1 + v1Offset) - (vert0 + v0Offset), e12 = (vert2 + v2Offset) - (vert1 + v1Offset), e20 = (vert0 + v0Offset) - (vert2 + v2Offset);
		glm::vec3 d0p = pt - (vert0 + v0Offset), d1p = pt - (vert1 + v1Offset), d2p = pt - (vert2 + v2Offset);
		glm::vec3 sideEdge01 = glm::cross(e01, d0p);
		glm::vec3 sideEdge12 = glm::cross(e12, d1p);
		glm::vec3 sideEdge20 = glm::cross(e20, d2p);

		float side0 = glm::dot(normal, sideEdge01);
		float side1 = glm::dot(normal, sideEdge12);
		float side2 = glm::dot(normal, sideEdge20);

		return side0 >= 0.0 && side1 >= 0.0 && side2 >= 0.0;
	}

	glm::vec4 collideFace(glm::vec3 pt, glm::vec3 dif, float radius)
	{
		//This should work... Maybe not though
		float d0 = glm::dot(pt, normal) + d - glm::dot(normal * radius, normal);
		float d1 = glm::dot(pt + dif, normal) + d - glm::dot(normal * radius, normal);
		//Returns when it crosses the plane...
		float t = -d0 / (d1 - d0);
		//test if it does collie the face... not just the plane.
		//maybe let's just...
		//bool ptIn = ptInTriangle(pt + t * dif);//ptInTriangleProject(pt + t * dif);
		//std::cout << ptIn << "\n";
		if (t >= 0.0 && t <= 1.0 && ptInTriangleProject(pt + t * dif))
		{
			return glm::vec4(normal, t);
		}
		else
		{
			return glm::vec4(0,0,0, 100.0); // return a large number because we will use a min fucntion later
		}
		//return -d0 / (d1 - d0);
	}

	glm::vec4 collideSphere(glm::vec3 center, glm::vec3 pt, glm::vec3 dif, float radius)
	{
		glm::vec3 PA = pt - center;
		glm::vec3 PB = PA + dif;

		float a = glm::dot(dif, dif);
		float b = 2 * glm::dot(PA, dif);
		float c = glm::dot(PA, PA) - radius * radius;

		float discriminant = b * b - 4 * a * c;
		if (discriminant < 0)
		{
			return glm::vec4(0,0,0,100.0);
		}
		else if (discriminant > 0)
		{
			//two answers
			float aa = -b / (2 * a);
			float ab = sqrt(discriminant) / (2 * a);

			float t0 = aa + ab;
			float t1 = aa - ab;

			//need them to be greater than 0... 
			if (t0 >= 0 && t1 >= 0)
			{
				float t = fmin(t0, t1);

				//need to get normal..
				glm::vec3 norm =(pt + t * dif) - center;

				return glm::vec4(norm, t);
			}
			else if(t0 >= 0 || t1 >= 0)
			{
				float t = fmax(t0, t1);

				//need to get normal..
				
				if (t <= 1.0)
				{
					glm::vec3 norm = (pt + t * dif) - center;

					return glm::vec4(norm, t);
				}
				else
				{
					return glm::vec4(0, 0, 0, 100);
				}
			}
			else
			{
				return glm::vec4(0, 0, 0, 100);
			}
		}
		else
		{
			//one answer
			float t = -b / (2 * a);

			//need to get normal..
			glm::vec3 norm = (pt + t * dif) - center;
			return glm::vec4(norm, t);
		}
	}

	glm::vec4 collideEdge(glm::vec3 A, glm::vec3 B, glm::vec3 pt, glm::vec3 dif, float radius)
	{
		//get closest point to line from line...
		glm::vec3 u = A - B;
		glm::vec3 v = dif;
		glm::vec3 w = B - pt;

		//http://geomalgorithms.com/a07-_distance.html#dist3D_Segment_to_Segment()

		const float SMALL_NUMBER = 1.0E-10;

		float    a = dot(u, u);         // always >= 0
		float    b = dot(u, v);
		float    c = dot(v, v);         // always >= 0
		float    d = dot(u, w);
		float    e = dot(v, w);
		float    D = a * c - b * b;        // always >= 0
		float    sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
		float    tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

		// compute the line parameters of the two closest points
		if (D < SMALL_NUMBER) { // the lines are almost parallel
			sN = 0.0;         // force using point P0 on segment S1
			sD = 1.0;         // to prevent possible division by 0.0 later
			tN = e;
			tD = c;
		}
		else {                 // get the closest points on the infinite lines
			sN = (b * e - c * d);
			tN = (a * e - b * d);
			if (sN < 0.0) {        // sc < 0 => the s=0 edge is visible
				sN = 0.0;
				tN = e;
				tD = c;
			}
			else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
				sN = sD;
				tN = e + b;
				tD = c;
			}
		}

		if (tN < 0.0) {            // tc < 0 => the t=0 edge is visible
			tN = 0.0;
			// recompute sc for this edge
			if (-d < 0.0)
				sN = 0.0;
			else if (-d > a)
				sN = sD;
			else {
				sN = -d;
				sD = a;
			}
		}
		else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
			tN = tD;
			// recompute sc for this edge
			if ((-d + b) < 0.0)
				sN = 0;
			else if ((-d + b) > a)
				sN = sD;
			else {
				sN = (-d + b);
				sD = a;
			}
		}
		// finally do the division to get sc and tc
		sc = (abs(sN) < SMALL_NUMBER ? 0.0 : sN / sD);
		tc = (abs(tN) < SMALL_NUMBER ? 0.0 : tN / tD);

		sc = glm::clamp(sc, 0.0f, 1.0f);

		//sc is out t... the closest Pt...
		glm::vec3 ptOnLine = B + sc * u;

		return collideSphere(ptOnLine, pt, dif, radius);
	}

	//returns vec4 = (nx, ny, nz, t);
	glm::vec4 PointinTriangleOffset(glm::vec3 pt, glm::vec3 dif, float radius)
	{
		//first an offset from the plane
		//Both ways... Or just one face...  Just onece face. Only collide with forawrd geometry...Hopefully
		glm::vec4 faceT = collideFace(pt, dif, radius);
		glm::vec4 edge01T = collideEdge(vert0, vert1, pt, dif, radius);
		glm::vec4 edge12T = collideEdge(vert1, vert2, pt, dif, radius);
		glm::vec4 edge20T = collideEdge(vert2, vert0, pt, dif, radius);

		if (faceT.w <= edge01T.w && faceT.w <= edge12T.w && faceT.w <= edge20T.w)
		{
			return faceT;
		}
		else if (edge01T.w <= edge12T.w && edge01T.w <= edge20T.w)
		{
			return edge01T;
		}
		else if (edge12T.w <= edge20T.w)
		{
			return edge12T;
		}
		else
		{
			return edge20T;
		}
		
	}

	glm::vec3 getClampedBarycentricPt(glm::vec3 pt)
	{
		//https://stackoverflow.com/questions/14467296/barycentric-coordinate-clamping-on-3d-triangle
		//Sus but we'll try it...
		glm::vec3 bcPt = getBarycentricPt(pt);

		if (bcPt.x < 0)
		{
			float t = glm::dot(pt - vert1, vert2 - vert1) / glm::dot(vert2 - vert1, vert2 - vert1);
			t = glm::clamp(t, 0.0f, 1.0f);
			return glm::vec3(0.0f, 1.0f - t, t);
		}
		else if (bcPt.y < 0)
		{
			float t = glm::dot(pt - vert2, vert0 - vert2) / glm::dot(vert0 - vert2, vert0 - vert2);
			t = glm::clamp(t, 0.0f, 1.0f);
			return glm::vec3(t, 0.0f, 1.0f - t);
		}
		else if (bcPt.z < 0)
		{
			float t = glm::dot(pt - vert0, vert1 - vert0) / glm::dot(vert1 - vert0, vert1 - vert0);
			t = glm::clamp(t, 0.0f, 1.0f);
			return glm::vec3(1.0f - t, t, 0.0f);
		}
		else
		{
			return bcPt;
		}
	}

	glm::vec3 getBarycentricPt(glm::vec3 pt)
	{
		//glm::vec3 l0 = 
		//((y(D) - y(C)) (x(B) - x(C)) + (x(D) - x(C)) (y(C) - y(B))) / ((y(A) - y(C)) (x(B) - x(C)) + (x(A) - x(C)) (y(C) - y(B)))
		//https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
			// Compute barycentric coordinates (u, v, w) for

		glm::vec3 v0 = vert1 - vert0, v1 = vert2 - vert0, v2 = pt - vert0;
		float d00 = glm::dot(v0, v0);
		float d01 = glm::dot(v0, v1);
		float d11 = glm::dot(v1, v1);
		float d20 = glm::dot(v2, v0);
		float d21 = glm::dot(v2, v1);
		float invDenom = 1.0 / (d00 * d11 - d01 * d01);
		float v = (d11 * d20 - d01 * d21) * invDenom;
		float w = (d00 * d21 - d01 * d20) * invDenom;
		float u = 1.0f - v - w;

		return glm::vec3(u, v, w);


		/*
		glm::vec3 v0 = vert1 - vert0, v1 = vert2 - vert0, v2 = pt - vert0;
		float d00 = glm::dot(v0, v0);
		float d01 = glm::dot(v0, v1);
		float d11 = glm::dot(v1, v1);
		float d20 = glm::dot(v2, v0);
		float d21 = glm::dot(v2, v1);
		float denom = d00 * d11 - d01 * d01;
		float v = (d11 * d20 - d01 * d21) / denom;
		float w = (d00 * d21 - d01 * d20) / denom;
		float u = 1.0f - v - w;
		return glm::vec3(u,v,w);
		*/
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

		//vert0 += v0Offset * n0;
		//vert1 += v1Offset * n1;
		//vert2 += v2Offset * n2;

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

class Poly
{
public:
	Poly(std::vector<glm::vec3> pts, glm::vec3 norm, glm::vec4 plane)
	{
		this->pts = pts;
		this->norm = norm;
		this->plane = plane;
	}

	void applyMatrixSelf(glm::mat4 matrix, glm::mat4 normalMatrix)
	{
		for (int i = 0; i < pts.size(); i++)
		{
			pts[i] = matrix * glm::vec4(pts[i], 1.0);
		}
		norm = normalMatrix * glm::vec4(norm, 0.0);
		plane = glm::vec4(norm, -glm::dot(norm, pts[0]));
	}
	
	static glm::vec3 getIntersection(glm::vec3 ptA, glm::vec3 ptB, glm::vec4 plane)
	{
		//https://www.cs.princeton.edu/courses/archive/fall00/cs426/lectures/raycast/sld017.htm
		glm::vec3 v = ptB - ptA;
		float t = -(glm::dot(ptA, plane.xyz()) + plane.w) / glm::dot(plane.xyz(), v);
		return ptA + t * (v);
	}

	glm::vec3 getIntersection(glm::vec3 ptA, glm::vec3 ptB)
	{
		return getIntersection(ptA, ptB, plane);
		/*
		//https://www.cs.princeton.edu/courses/archive/fall00/cs426/lectures/raycast/sld017.htm
		glm::vec3 v = ptB - ptA;
		float t = -(glm::dot(ptA, plane.xyz()) + plane.w) / glm::dot(plane.xyz(), v);
		return ptA + t * (v);
		*/
	}

	std::vector<glm::vec3> pts;
	glm::vec3 norm;
	glm::vec4 plane;
};
