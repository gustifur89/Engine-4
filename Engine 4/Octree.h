#pragma once
#include "Header.h"
#include "GameObject.h"
#include "TempGeometry.h"

class CollisionStructure
{
public:
	virtual bool collide(float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, glm::vec3* faceNorm, int level) = 0;
};

class Octree : public CollisionStructure
{
private:

	class Node
	{
	public:
		glm::vec3 min, max;
		glm::vec3 center;
		bool leaf;
		Node* nodes[8];
		std::vector<int> TriangleIndexList;
		int getRegion(glm::vec3 pt);
		std::vector<std::shared_ptr<MeshTriangle>> triangles;
		std::vector<int> getRegions(glm::vec3 v0, glm::vec3 v1);
		Node(glm::vec3 min, glm::vec3 max);
		void getExtentsFromRegion(int region, glm::vec3*min_, glm::vec3*max_);
		void create(int level, int minTris, std::vector<std::shared_ptr<MeshTriangle>>& trianglesIn);
		bool isTriangleInRegion(std::shared_ptr<MeshTriangle> triangle);
		bool collide(float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, glm::vec3* faceNorm, int level);
	};


	Node* root;

public:
	std::vector<std::shared_ptr<MeshTriangle>> triangles;
	glm::vec3 min, max;

	Octree();

	bool hitCheck(Node* node, float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3* out);

	bool collide(float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, glm::vec3* faceNorm, int level);

	void create(std::shared_ptr<GameObject> worldObject, glm::mat4 parentTransform);

	//construct...
	void construct(std::shared_ptr<GameObject> worldObject);
};

class TempSoup : public CollisionStructure
{
public:
	bool collide(float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, glm::vec3* faceNorm, int level);
	std::vector<std::shared_ptr<MeshTriangle>> triangles;

	void create(std::shared_ptr<GameObject> worldObject, glm::mat4 parentTransform);

};


