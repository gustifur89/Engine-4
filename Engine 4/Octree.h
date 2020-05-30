#pragma once
#include "Header.h"
#include "GameObject.h"
#include "Geometry.h"
#include "TempGeometry.h"

class CollisionStructure
{
public:
	virtual bool collide(std::shared_ptr<Collider> collider, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, glm::vec3* faceNorm, int level) = 0;
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
		bool collide(std::shared_ptr<Collider> collider, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, glm::vec3* faceNorm, int level);
	};


	Node* root;

public:
	std::vector<std::shared_ptr<MeshTriangle>> triangles;
	glm::vec3 min, max;

	Octree();

	bool hitCheck(Node* node, float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3* out);

	bool collide(std::shared_ptr<Collider> collider, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, glm::vec3* faceNorm, int level);

	void create(std::shared_ptr<GameObject> worldObject, glm::mat4 parentTransform);

	//construct...
	void construct(std::shared_ptr<GameObject> worldObject);
};

class TempSoup : public CollisionStructure
{
public:
	bool collide(std::shared_ptr<Collider> collider, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, glm::vec3* faceNorm, int level);
	std::vector<std::shared_ptr<MeshTriangle>> triangles;

	void create(std::shared_ptr<GameObject> worldObject, glm::mat4 parentTransform);

};

class BSP : public CollisionStructure
{
protected:
	void appendObject(std::shared_ptr<GameObject> worldObject, glm::mat4 parentTransform);

public:
	class Node
	{
	private:
		static const float COINCIDENT_MARGAIN;
		enum SIDE
		{
			COINCIDENT,
			FRONT,
			BACK,
			SPANNING
		};
		enum LEAF
		{
			NOT_LEAF,
			SOLID,
			EMPTY
		};

		static int getPtSide(glm::vec3 pt, glm::vec4 plane);
		static int getPolySide(std::shared_ptr<Poly> polygon, glm::vec4 plane);

		void splitPoly(std::shared_ptr<Poly> poly, std::shared_ptr<Poly>* frontPart, std::shared_ptr<Poly>* backPart);
		int getPtSide(glm::vec3 pt);
		int getPolySide(std::shared_ptr<Poly> polygon);
	public:
		void create(std::vector<std::shared_ptr<Poly>> polygons, int maxPolys, int side);
		void appendPolys(std::vector<std::shared_ptr<Poly>>* polygons);
		bool rayUnder(glm::vec4 plane, float offset, glm::vec3 v0, glm::vec3 v1, glm::vec3* w0, glm::vec3* w1);
		bool rayOver(glm::vec4 plane, float offset, glm::vec3 v0, glm::vec3 v1, glm::vec3* w0, glm::vec3* w1);
		bool collide(std::shared_ptr<Collider> collider, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, glm::vec3* faceNorm, float* t);
		bool inside(glm::vec3 pt);
		//bool segmentUnder(glm::vec4 plane, glm::vec3 v0, glm::vec3 v1, glm::vec3 nv0, glm::vec3* w0, glm::vec3* w1, glm::vec3* nw0);
		//bool segmentOver(glm::vec4 plane, glm::vec3 v0, glm::vec3 v1, glm::vec3 nv0, glm::vec3* w0, glm::vec3* w1, glm::vec3* nw0);
		//bool collideSphere(float radius, std::shared_ptr<Node> node, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, glm::vec3* faceNorm);
		std::shared_ptr<Poly> root;
		glm::vec4 plane;
		std::vector<std::shared_ptr<Poly>> polygons;
		std::shared_ptr<Node> front;
		std::shared_ptr<Node> back;

		static glm::vec3 impactNormal;
		static bool segmentUnder(const glm::vec4& plane, glm::vec3& v0, glm::vec3& v1, glm::vec3& nv0, glm::vec3* w0, glm::vec3* w1, glm::vec3* nw0);
		static bool segmentOver(const glm::vec4 &plane, glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &nv0, glm::vec3* w0, glm::vec3* w1, glm::vec3* nw0);
		static bool collideSphere(float radius, std::shared_ptr<Node> node, glm::vec3 v0, glm::vec3 v1, glm::vec3* impactPt, glm::vec3& nv0);

		static bool collideCylinder(float radius, float hieght, std::shared_ptr<Node> node, glm::vec3 v0, glm::vec3 v1, glm::vec3* impactPt, glm::vec3& nv0);

		int leaf;
		static int count;
	};
	
	std::vector<std::shared_ptr<Poly>> polygons;
	std::shared_ptr<Node> root;

	bool collide(std::shared_ptr<Collider> collider, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, glm::vec3* faceNorm, int level);
	void create(std::shared_ptr<GameObject> worldObject, glm::mat4 parentTransform);
	bool inside(glm::vec3 pt);

	void makeOnlyTris(std::vector<std::shared_ptr<Poly>>* polygons);
	std::shared_ptr<ColorMesh> getMesh();
	std::shared_ptr<ColorMesh> getMeshTest(std::shared_ptr<GameObject> worldObject, glm::mat4 parentTransform);
};

