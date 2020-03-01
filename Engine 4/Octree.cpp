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

// ========================== Octree ===================

Octree::Octree()
{
	root = nullptr;
}

bool Octree::collide(float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3* out)
{
	return hitCheck(root, radius, v0, v1, out);
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

// ======================= TempSoup =======================

bool TempSoup::collide(float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3* out)
{
	*out = v1;

	for (std::shared_ptr<MeshTriangle> triangle : triangles)
	{
		if (triangle->collide(radius, v0, v1, out))
		{
			std::cout << "hit\n";
			return true;
		}
	}

	return false;
}

void TempSoup::create(std::shared_ptr<GameObject> worldObject, glm::mat4 parentTransform)
{

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
