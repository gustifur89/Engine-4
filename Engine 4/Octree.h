#pragma once
#include "Header.h"
#include "GameObject.h"
#include "Geometry.h"

class Octree
{
private:

	class Node
	{
	public:
		glm::vec3 center;
		Node* nodes[8];
		std::vector<int> TriangleIndexList;
		int getRegion(glm::vec3 pt);
		std::vector<std::shared_ptr<MeshTriangle>> triangles;
		std::vector<int> getRegions(glm::vec3 v0, glm::vec3 v1);

	};


	Node* root;

public:

	Octree();

	bool hitCheck(Node* node, float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3* out);

	bool collide(float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3* out);

	//construct...
	void construct(std::shared_ptr<GameObject> worldObject);
};

class TempSoup
{
public:
	bool collide(float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3* out);
	std::vector<std::shared_ptr<MeshTriangle>> triangles;

	void create(std::shared_ptr<GameObject> worldObject, glm::mat4 parentTransform);

};


