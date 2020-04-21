#include "Octree.h"

// ======================== Node =====================

int Octree::Node::getRegion(glm::vec3 pt)
{
	bool x = pt.x >= center.x;
	bool y = pt.y >= center.y;
	bool z = pt.z >= center.z;
	return 1 * x + 2 * y + 4 * z;
}

std::vector<int> Octree::Node::getRegions(glm::vec3 v0, glm::vec3 v1)
{
	glm::vec3 dir01 = v1 - v0;

	std::vector<int> regionList;

	int region0 = getRegion(v0);
	int region1 = getRegion(v1);

	regionList.push_back(region0);

	if (region0 != region1)
	{
		float denomX = glm::dot(dir01, glm::vec3(1, 0, 0));
		float tX = 100.0;

		if (denomX != 0.0)
		{
			float t = glm::dot(center - v0, glm::vec3(1, 0, 0)) / denomX;
			if (t >= 0 && t <= 1.0)
			{
				tX = t;
			}
		}

		float denomY = glm::dot(dir01, glm::vec3(0, 1, 0));
		float tY = 100.0;

		if (denomY != 0.0)
		{
			float t = glm::dot(center - v0, glm::vec3(0, 1, 0)) / denomY;
			if (t >= 0 && t <= 1.0)
			{
				tY = t;
			}
		}

		float denomZ = glm::dot(dir01, glm::vec3(0, 0, 1));
		float tZ = 100.0;

		if (denomZ != 0.0)
		{
			float t = glm::dot(center - v0, glm::vec3(0, 0, 1)) / denomZ;
			if (t >= 0 && t <= 1.0)
			{
				tZ = t;
			}
		}

		//flip 1
		if (tX <= 1.0 && tX < tY && tX < tZ)
		{
			tX = 100.0;
			region0 ^= 1UL << 0;
			regionList.push_back(region0);
		}
		else if (tY <= 1.0 && tY < tX && tY < tZ)
		{
			tY = 100.0;
			region0 ^= 1UL << 1;
			regionList.push_back(region0);
		} 
		else if (tZ <= 1.0 && tZ < tX && tZ < tY)
		{
			tZ = 100.0;
			region0 ^= 1UL << 2;
			regionList.push_back(region0);
		}

		//flip 2
		if (tX <= 1.0 && tX < tY && tX < tZ)
		{
			tX = 100.0;
			region0 ^= 1UL << 0;
			regionList.push_back(region0);
		}
		else if (tY <= 1.0 && tY < tX && tY < tZ)
		{
			tY = 100.0;
			region0 ^= 1UL << 1;
			regionList.push_back(region0);
		}
		else if (tZ <= 1.0 && tZ < tX && tZ < tY)
		{
			tZ = 100.0;
			region0 ^= 1UL << 2;
			regionList.push_back(region0);
		}

		//flip 3
		if (tX <= 1.0 && tX < tY && tX < tZ)
		{
			tX = 100.0;
			region0 ^= 1UL << 0;
			regionList.push_back(region0);
		}
		else if (tY <= 1.0 && tY < tX && tY < tZ)
		{
			tY = 100.0;
			region0 ^= 1UL << 1;
			regionList.push_back(region0);
		}
		else if (tZ <= 1.0 && tZ < tX && tZ < tY)
		{
			tZ = 100.0;
			region0 ^= 1UL << 2;
			regionList.push_back(region0);
		}

	}

	return regionList;
}

Octree::Node::Node(glm::vec3 min, glm::vec3 max)
{
	this->min = min;
	this->max = max;
	this->center = 0.5f * (min + max);
	this->leaf = false;
}

bool Octree::Node::isTriangleInRegion(std::shared_ptr<MeshTriangle> triangle)
{
	//bounding box:
	glm::vec3 n = center - triangle->center;

	// Calculate half extents along x axis for each object
	glm::vec3 a_extent = 0.5f * (max - min);
	glm::vec3 b_extent = 0.5f * (triangle->max - triangle->min);

	// Calculate overlap on x axis
	glm::vec3 overlap = a_extent + b_extent - glm::abs(n);

	// SAT test for bounding boxes
	if (overlap.x > 0 && overlap.y > 0 && overlap.z > 0)
	{
		//we are overlapping...
		//lets just go from here...
		return true;
	}
	return false;
}

bool Octree::Node::collide(float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, glm::vec3* faceNorm, int level)
{
	if (leaf)
	{
		//collide with all of the triangles...
	}
	else
	{
		//collide with all of the subvolumes...



	}
	return false;
}

void Octree::Node::getExtentsFromRegion(int region, glm::vec3* min_, glm::vec3* max_)
{
	//bool x = pt.x >= center.x;
	//bool y = pt.y >= center.y;
	//bool z = pt.z >= center.z;
	if (region & 1)
	{
		//we are above the x axis
		min_->x = center.x;
		max_->x = max.x;
	}
	else
	{
		//we are below the x axis
		min_->x = min.x;
		max_->x = center.x;
	}

	if ((region >> 1 )& 1)
	{
		//we are above the y axis
		min_->y = center.y;
		max_->y = max.y;
	}
	else
	{
		//we are below the y axis
		min_->y = min.y;
		max_->y = center.y;
	}

	if ((region >> 2) & 1)
	{
		//we are above the z axis
		min_->z = center.z;
		max_->z = max.z;
	}
	else
	{
		//we are below the z axis
		min_->z = min.z;
		max_->z = center.z;
	}
}

void Octree::Node::create(int level, int minTris, std::vector<std::shared_ptr<MeshTriangle>>& trianglesIn)
{
	if (level == 0 || trianglesIn.size() <= minTris)
	{
		triangles = trianglesIn;
		leaf = true;
	}
	else
	{
		for (int reg = 0; reg < 8; reg++)
		{
			glm::vec3 min_, max_;
			getExtentsFromRegion(reg, &min_, &max_);
			nodes[reg] = new Node(min_, max_);
		
			std::vector<std::shared_ptr<MeshTriangle>> trisInReg;

			for (int i = 0; i < trianglesIn.size(); i++)
			{
				if (nodes[reg]->isTriangleInRegion(trianglesIn[i]))
				{
					trisInReg.push_back(trianglesIn[i]);
				}
			}

			nodes[reg]->create(level - 1, minTris, trianglesIn);
		}
	}
}

// ========================== Octree ===================

Octree::Octree()
{
	root = nullptr;
}

bool Octree::collide(float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, glm::vec3* faceNorm, int level)
{
	return root->collide(radius, v0, v1, out, faceNorm, level);

	//return hitCheck(root, radius, v0, v1, out);
}

bool Octree::hitCheck(Node* node, float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3* out)
{
	/*
	Psuedo code:
		Traverse Octree...  how? dammit.  How do I line travers an octree....
		Maybe there is a way to find which trees to slide down based on the line...
		Could get intersections of the line with the Axis planes..
		[Traverse]

		At leaf node:
		for all triangles in node :
			get intersection of ray with offset geometry



	*/
	if (!node)
	{
		//Empty leaf...
		*out = v1;
		return false;
	}
	else
	{
		//https://graphicsinterface.org/wp-content/uploads/gi2000-28.pdf
		// modified because they use BSP. I'm using Octree because I can't be bothered with BSP cosntruction tonight. :p
		bool hit = false;
		glm::vec3 w0, w1;




		return hit;
	}
}

void Octree::construct(std::shared_ptr<GameObject> worldObject)
{
	//Step 1: put all of the object and children's transformed mesh geometry into a list of triangles...


	//step 2: construct Octree from the triangle lsit


}

void Octree::create(std::shared_ptr<GameObject> worldObject, glm::mat4 parentTransform)
{
	std::cout << "make\n";

	if (worldObject->mesh)
	{
		std::vector<std::shared_ptr<MeshTriangle>> meshTris = worldObject->mesh->getTrianglesFromMesh(parentTransform * worldObject->transform.getTransformMatrix());

		triangles.insert(std::end(triangles), std::begin(meshTris), std::end(meshTris));
	}

	for (std::shared_ptr<GameObject> child : worldObject->children)
	{
		create(child, parentTransform * worldObject->transform.getTransformMatrix());
	}

	glm::vec3 min, max;
	bool first = true;
	//optomize this.
	for (int i = 0; i < triangles.size(); i++)
	{
		if (first)
		{
			min = triangles[i]->vert0;
			max = triangles[i]->vert0;
			first = false;
		}
		
		min = glm::min(min, triangles[i]->vert0);
		max = glm::max(max, triangles[i]->vert0);
		min = glm::min(min, triangles[i]->vert1);
		max = glm::max(max, triangles[i]->vert1);
		min = glm::min(min, triangles[i]->vert2);
		max = glm::max(max, triangles[i]->vert2);
	}

	//we get the whole list of triangles...
	//now we need to recursively do the stuff.
	root = new Node(min, max);
	root->create(0, 3, triangles);
	
}

// ======================= TempSoup =======================

bool TempSoup::collide(float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, glm::vec3* faceNorm, int level)
{
	glm::vec3 nPt;
	glm::vec3 gPt;
	glm::vec3 norm;
	float maxNegDot = 10.0;
	int numCollided =0;
	float t = 10.0;

	glm::vec3 vel = v1 - v0;

	for (std::shared_ptr<MeshTriangle> triangle : triangles)
	{
		glm::vec3 nPt_;
		glm::vec3 gPt_;

		float dot = glm::dot(vel, triangle->normal);

		float nt;

		if (dot < 0.0 && triangle->collide(radius, v0, v1, &nt, &nPt_, &gPt_))
		{
			//We collide something...
			numCollided++;

			if (nt < t)
			{
				nPt = nPt_;
				gPt = gPt_;
				norm = triangle->normal;
				//maxNegDot = dot;
				*faceNorm = triangle->normal;
				t = nt;
			}
			
		}
	}

	if (numCollided > 0)
	{
		glm::vec3 levDownPt;

		if (level == 0)
		{
			//ground level, no more...
			*out = nPt; //Don't project anymore..
			return true;
		}
		else
		{
			glm::vec3 n_norm;

			//not ground level. Collide the projected:
			if (collide(radius, nPt, gPt, &levDownPt, &n_norm, level - 1))
			{
				//there is a collision...
				//but we know that levDownPt is the last pt we can touch...
				
				/*
				//we see if the next collided is convex or concave with this one... 
				// check dot of normals..
				float normDot = glm::dot(n_norm, norm);
				//if it is less than 0, then it is overhangy.  We want to not project if overhang...
				if (normDot < 0.0) // this means it is hitting two walls that are contradictory... maybe... maybe just dont?
				{
					//last known good point
					//*out = nPt;
					*out = v0;
				}
				else
				{
					//the projected point.
					*out = levDownPt;
				}*/

				*out = levDownPt;

				return true;
			}
			else
			{
				//no more collisions... We are good to go!
				*out = gPt;
				return true;
			}
		}
	}

	return false;
}

void TempSoup::create(std::shared_ptr<GameObject> worldObject, glm::mat4 parentTransform)
{
	if (!worldObject) return;
	if (worldObject->mesh)
	{
		std::vector<std::shared_ptr<MeshTriangle>> meshTris = worldObject->mesh->getTrianglesFromMesh(parentTransform * worldObject->transform.getTransformMatrix());

		triangles.insert(std::end(triangles), std::begin(meshTris), std::end(meshTris));
	}

	for (std::shared_ptr<GameObject> child : worldObject->children)
	{
		create(child, parentTransform * worldObject->transform.getTransformMatrix());
	}
}
