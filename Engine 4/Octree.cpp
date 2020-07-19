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

bool Octree::Node::collide(std::shared_ptr<Collider> collider, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, std::vector<glm::vec3>* faceNorms, int level)
{
	float radius = 0.0;
	if (collider->type == Collider::TYPE::SPHERE)
	{
		radius = std::static_pointer_cast<Sphere>(collider)->radius;
	}
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

bool Octree::collide(std::shared_ptr<Collider> collider, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, std::vector<glm::vec3>* faceNorms, int level)
{
	return root->collide(collider, v0, v1, out, faceNorms, level);

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

bool TempSoup::collide(std::shared_ptr<Collider> collider, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, std::vector<glm::vec3>* faceNorms, int level)
{
	float radius = 0.0;
	if (collider->type == Collider::TYPE::SPHERE)
	{
		radius = std::static_pointer_cast<Sphere>(collider)->radius;
	}
	glm::vec3 nPt;
	glm::vec3 gPt;
	glm::vec3 norm;
	float maxNegDot = 10.0;
	int numCollided =0;
	float t = 10.0;
	glm::vec3 fNorm;

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
				fNorm = triangle->normal;
				t = nt;
			}
			
		}
	}

	if (numCollided > 0)
	{
		glm::vec3 levDownPt;
		faceNorms->push_back(fNorm);
		if (level == 0)
		{
			//ground level, no more...
			*out = nPt; //Don't project anymore..
			return true;
		}
		else
		{
			 //n_norm;

			//not ground level. Collide the projected:
			if (collide(collider, nPt, gPt, &levDownPt, faceNorms, level - 1))
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

// ======================== BSP ===========================

void BSP::appendObject(std::shared_ptr<GameObject> worldObject, glm::mat4 parentTransform)
{
	if (worldObject->mesh)
	{
		if (worldObject->type == GameObject::TYPE::TEXTURE)
		{
			if (std::static_pointer_cast<GameObjectTexture>(worldObject)->multiMesh)
			{
				std::vector<std::shared_ptr<Poly>> meshTris;// = worldObject->mesh->getPolygonsFromMesh(parentTransform * worldObject->transform.getTransformMatrix());
				//polygons.insert(std::end(polygons), std::begin(meshTris), std::end(meshTris));
				std::shared_ptr<TextureMesh> tMesh = std::static_pointer_cast<TextureMesh>(worldObject->mesh);
				for (int i = 0; i < tMesh->subMeshes.size(); i++)
				{
					meshTris = tMesh->subMeshes[i]->getPolygonsFromMesh(parentTransform * worldObject->transform.getTransformMatrix());
					polygons.insert(std::end(polygons), std::begin(meshTris), std::end(meshTris));
				}
			}
			else
			{
				std::vector<std::shared_ptr<Poly>> meshTris = worldObject->mesh->getPolygonsFromMesh(parentTransform * worldObject->transform.getTransformMatrix());
				polygons.insert(std::end(polygons), std::begin(meshTris), std::end(meshTris));
			}
		}
		else
		{
			std::vector<std::shared_ptr<Poly>> meshTris = worldObject->mesh->getPolygonsFromMesh(parentTransform * worldObject->transform.getTransformMatrix());
			polygons.insert(std::end(polygons), std::begin(meshTris), std::end(meshTris));
		}

		//std::vector<std::shared_ptr<Poly>> meshTris = worldObject->mesh->getPolygonsFromMesh(parentTransform * worldObject->transform.getTransformMatrix());
		//polygons.insert(std::end(polygons), std::begin(meshTris), std::end(meshTris));
		/*
		if (worldObject->type == GameObject::TYPE::TEXTURE)
		{
			if (std::static_pointer_cast<GameObjectTexture>(worldObject)->multiMesh)
			{
				std::shared_ptr<TextureMesh> tMesh = std::static_pointer_cast<TextureMesh>(worldObject->mesh);
				for (int i = 0; i < tMesh->subMeshes.size(); i++)
				{
					tMesh->subMeshes[i]->render();
					meshTris = tMesh->subMeshes[i]->getPolygonsFromMesh(parentTransform * worldObject->transform.getTransformMatrix());
					polygons.insert(std::end(polygons), std::begin(meshTris), std::end(meshTris));
				}
			}
		}*/
	}
	for (std::shared_ptr<GameObject> child :  worldObject->children)
	{
		appendObject(child, parentTransform * worldObject->transform.getTransformMatrix());
	}
}

glm::vec3 BSP::Node::impactNormal;

bool BSP::collide(std::shared_ptr<Collider> collider, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, std::vector<glm::vec3>* faceNorms, int level)
{
	//float t = 0.0;
	if (root)
	{
		glm::vec3 norm(0);// (0, 1, 0);// = root->plane.xyz();

		bool hit = false;
		float radius, hieght;
		int type = collider->type;
		if (type == Collider::TYPE::SPHERE)
		{
			radius = std::static_pointer_cast<Sphere>(collider)->radius;
			hit = Node::collideSphere(radius, root, v0, v1, out, norm);
		}
		else if (type == Collider::TYPE::CYLINDER)
		{
			radius = std::static_pointer_cast<Cylinder>(collider)->radius;
			hieght = std::static_pointer_cast<Cylinder>(collider)->hieght;
			hit = Node::collideCylinder(radius, hieght, root, v0, v1, out, norm);
			//hit = Node::collideSphere(radius, root, v0, v1, out, norm);
		}


		if (hit)
		{
			//std::cout << "hit\n";
			glm::vec3 fNorm = Node::impactNormal;
			faceNorms->push_back(Node::impactNormal);
			//std::cout << fNorm.x << " : " << fNorm.y << " : " << fNorm.z << "\n";
			if (glm::dot(v1 - v0, fNorm) > 0)
			{
				//*out = v1;
				//return false;
			}
			/*if (!inside(*out))
			{
				*out = v1;
				return false;
			}*/

			float epsilon = fmin(glm::length(*out - v0), 0.001f);

			*out -= epsilon * glm::normalize(v1 - v0);

			if (level > 0)
			{
				glm::vec3 remainder = v1 - *out;

				//glm::vec3 perp = glm::dot(remainder, norm) * norm;
				glm::vec3 perp = glm::dot(remainder, fNorm) * (fNorm);

				glm::vec3 par = remainder - perp;

				glm::vec3 n_v0 = *out;
				glm::vec3 n_v1 = n_v0 + par;

				glm::vec3 n_out;

				//glm::vec3 face;
			//	std::cout << "yeem\n";
				if (this->collide(collider, n_v0, n_v1, &n_out, faceNorms, level - 1))
				{
					*out = n_out;
					return true;
				}
				else
				{
					*out = n_v1;
					return true;
				}
			}

			return true;

			//*out += 0.001f * (*faceNorm);
			//std::cout << faceNorm->x << " : " << faceNorm->y << " : " << faceNorm->z << "\n";
			/*
			if (level > 0)
			{
				glm::vec3 remainder = v1 - *out;

				//glm::vec3 perp = glm::dot(remainder, norm) * norm;
				glm::vec3 perp = glm::dot(remainder, *faceNorm) * (*faceNorm);

				glm::vec3 par = remainder - perp;

				glm::vec3 n_v0 = *out;
				glm::vec3 n_v1 = n_v0 + par;
				glm::vec3 face;
				this->collide(radius, n_v0, n_v1, out, &face, level - 1);
			}*/
			//return true;

			//return true;
		}

		/*if (collideSphere(radius root, v0, v1, out, faceNorm))
		{
			if (glm::dot(v1 - v0, *faceNorm) > 0)
			{
				//*out = v1;
				//return false;
			}
			return true;

		}*/
		/*
		if (root->collide(radius, v0, v1, out, faceNorm, &t))
		{
			if (glm::dot(v1 - v0, *faceNorm) > 0)
			{
				*out = v1;
				return false;
			}

			//we collide...
			//*out += 0.0001f * (*faceNorm);
			if (false && level > 0)
			{
				glm::vec3 remainder = v1 - *out;

				//glm::vec3 perp = glm::dot(remainder, norm) * norm;
				glm::vec3 perp = glm::dot(remainder, *faceNorm) * (*faceNorm);

				glm::vec3 par = remainder - perp;

				glm::vec3 n_v0 = *out;
				glm::vec3 n_v1 = n_v0 + par;
				glm::vec3 face;
				this->collide(radius, n_v0, n_v1, out, &face, level - 1);
			}
			return true;

		}*/
	}
	return false;
}

void BSP::create(std::shared_ptr<GameObject> worldObject, glm::mat4 parentTransform)
{
	if (!worldObject) return;
	appendObject(worldObject, parentTransform);
	//now we have a list of triangles....
	for (int i = 0; i < polygons.size(); i++)
	{
		std::shared_ptr<Poly> poly = polygons[i];
	//	std::cout << i << " ";
	//	std::cout << poly->plane.x << " : " << poly->plane.y << " : " << poly->plane.z << " : " << poly->plane.w << "\n";
	}

	root = std::shared_ptr<Node>(new Node);
	root->create(polygons, 1, 0);
}

bool BSP::inside(glm::vec3 pt)
{
	if (root)
		return root->inside(pt);
	return false;
}

bool BSP::rayCast(glm::vec3 v0, glm::vec3 v1, glm::vec3* out, glm::vec3* norm)
{
	if(root)
		return Node::rayCastNode(root, v0, v1, out, norm);
	return false;
}

void BSP::makeOnlyTris(std::vector<std::shared_ptr<Poly>>* polygons)
{ 
	for (std::shared_ptr<Poly> poly : *polygons)
	{
		if (!poly) continue;
		if (poly->pts.size() > 3)
		{
			//non triangle....
			std::vector<glm::vec3> pts = poly->pts;
			//replace current poly with the first triangle
			poly->pts = { pts[0], pts[1], pts[2] };
			for (int i = 3; i < pts.size(); i++)
			{
				//fill in the polygon with triangles..
				polygons->push_back(std::shared_ptr<Poly>(new Poly({ pts[0], pts[i-1], pts[i] }, poly->norm, poly->plane)));
			}
		}
	}
}

std::shared_ptr<ColorMesh> BSP::getMesh()
{
	std::vector<std::shared_ptr<Poly>> listOfPolys;
	root->appendPolys(&listOfPolys);
	makeOnlyTris(&listOfPolys);
	return ColorMesh::meshFromPolygons(listOfPolys, 200, 160, 120);
}

std::shared_ptr<ColorMesh> BSP::getMeshTest(std::shared_ptr<GameObject> worldObject, glm::mat4 parentTransform)
{
	std::vector<std::shared_ptr<Poly>> listOfPolys;
	root->appendPolys(&listOfPolys);
	makeOnlyTris(&listOfPolys);
	return ColorMesh::meshFromPolygons(listOfPolys, 200, 160, 120);
}

// ======================= BSP - Node =======================

const float BSP::Node::COINCIDENT_MARGAIN = 0.0001;
int BSP::Node::count = 0;

bool BSP::Node::rayUnder(glm::vec4 plane, float offset, glm::vec3 v0, glm::vec3 v1, glm::vec3* w0, glm::vec3* w1)
{
	glm::vec4 n_plane(plane.xyz(), plane.w + offset);
	int side0, side1;
	side0 = getPtSide(v0, n_plane);
	side1 = getPtSide(v1, n_plane);
	bool below0 = false, below1 = false;
	if (side0 == SIDE::BACK || side1 == SIDE::BACK)
	{
		//at least some part below the shifted plane..

		if (side0 == SIDE::BACK)
		{
			*w0 = v0;
			below0 = true;
		}
		if (side1 == SIDE::BACK)
		{
			*w1 = v1;
			below1 = true;
		}

		if (!below0 || !below1)
		{
			glm::vec3 intPt = Poly::getIntersection(v0, v1, n_plane);
			if (!below0)
			{
				*w0 = intPt;
			}
			if (!below1)
			{
				*w1 = intPt;
			}
		}

		if (glm::length(*w0 - *w1) < COINCIDENT_MARGAIN)
			return false;


		return true;
	}
	return false;
}

bool BSP::Node::rayOver(glm::vec4 plane, float offset, glm::vec3 v0, glm::vec3 v1, glm::vec3* w0, glm::vec3* w1)
{
	glm::vec4 n_plane(plane.xyz(), plane.w + offset);
	int side0, side1;
	side0 = getPtSide(v0, n_plane);
	side1 = getPtSide(v1, n_plane);
	bool over0 = false, over1 = false;
	if (side0 == SIDE::FRONT || side1 == SIDE::FRONT)
	{
		//at least some part below the shifted plane..

		if (side0 == SIDE::FRONT)
		{
			*w0 = v0;
			over0 = true;
		}
		if (side1 == SIDE::FRONT)
		{
			*w1 = v1;
			over1 = true;
		}

		if (!over0 || !over1)
		{
			glm::vec3 intPt = Poly::getIntersection(v0, v1, n_plane);
			if (!over0)
			{
				*w0 = intPt;
			}
			if (!over1)
			{
				*w1 = intPt;
			}
		}
		
		if (glm::length(*w0 - *w1) < COINCIDENT_MARGAIN)
			return false;

		return true;
	}
	return false;
}

bool BSP::Node::collide(std::shared_ptr<Collider> collider, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, glm::vec3* faceNorm, float* t)
{
	/*
	//https://github.com/melax/sandbox/blob/master/testbsp/bspcollide.cpp
	if (leaf == LEAF::EMPTY)
	{
		//terminates in an empty area
		return false;
	}
	if (leaf == LEAF::SOLID)
	{
		//hitting a solid area
		// the point of collision is 
		//*t = 1.0;
		*out = v0;
		return true;
	}

	//if not we need to traverse the tree.

	//determines which way we are moving relative to the mesh...
	// probably disable back colliding because that makes sense....

	glm::vec4 n_plane = this->root->plane;
	//float d = n_plane.w;
	//float f_d = n_plane.w + radius;
	//float b_d = n_plane.w - radius;

	int side0 = getPtSide(v0, n_plane);
	int side1 = getPtSide(v1, n_plane);

	if (side0 == SIDE::BACK && side1 == SIDE::BACK)
	{
		return back->collide(radius, v0, v1, out, faceNorm, t);
	}
	if (side0 == SIDE::FRONT && side1 == SIDE::FRONT)
	{
		return front->collide(radius, v0, v1, out, faceNorm, t);
	}

	glm::vec3 intPt = Poly::getIntersection(v0, v1, n_plane);
	//std::cout << intPt.x << " : " << intPt.y << " : " << intPt.z << "\n";
	if (side0 == SIDE::BACK)
	{
		if (back->collide(radius, v0, intPt, out, faceNorm, t))
		{
			return true;
		}
		*faceNorm = -n_plane.xyz();
		return front->collide(radius, intPt, v1, out, faceNorm, t);
	}
	//side0 == FRONT, side1 == BACK
	if (front->collide(collider, v0, intPt, out, faceNorm, t))
	{
		return true;
	}
	*faceNorm = n_plane.xyz();
	return back->collide(radius, intPt, v1, out, faceNorm, t);
	*/

	/*
		glm::vec3 w0, w1;
	bool hit;



	if (glm::dot(root->norm, v1 - v0) > 0)
	{
		if (rayUnder(plane, 0.0, v0, v1, &w0, &w1))
		{
			hit = back->collide(radius, w0, w1, out, faceNorm, t);
			if (hit)
				v1 = *out;
		}
		if (rayOver(plane, 0.0, v0, v1, &w0, &w1))
		{
			hit |= back->collide(radius, w0, w1, out, faceNorm, t);
		}
		return hit;
	}
	else
	{
		if (rayOver(plane, 0.0, v0, v1, &w0, &w1))
		{
			hit = back->collide(radius, w0, w1, out, faceNorm, t);
			if (hit)
				v1 = *out;
		}
		if (rayUnder(plane, 0.0, v0, v1, &w0, &w1))
		{
			hit != back->collide(radius, w0, w1, out, faceNorm, t);
		}
		return hit;
	}
	*/
	/*
	if (leaf == LEAF::EMPTY)
	{
		//terminates in an empty area
		return false;
	}
	if (leaf == LEAF::SOLID)
	{
		//hitting a solid area
		// the point of collision is 
		*t = 1.0;
		*out = v0;
		return true;
	}

	//if not we need to traverse the tree.
	
	//determines which way we are moving relative to the mesh...
	// probably disable back colliding because that makes sense....
	float dir = glm::dot(root->norm, v1 - v0);

//	std::cout << dir << "\n";

	glm::vec4 n_plane = this->root->plane;

	int side0 = getPtSide(v0, n_plane);
	int side1 = getPtSide(v1, n_plane);

	if (side0 == side1)
	{
		//both on the same side
		if (side0 == SIDE::FRONT)
		{
			//front
			// probably should do a null check... but we fill with leaves.... no null check for now.
			return front->collide(radius, v0, v1, out, faceNorm, t);
			/*if (front->collide(radius, v0, v1, out, faceNorm, t))
			{
				//if there is a collision....
				// how could there be a collision if the ray doesn't intersect the plane....  


			}* /
		}
		else
		{
			//back
			return back->collide(radius, v0, v1, out, faceNorm, t);
			//don't do anything with this...
			//if it does collide directly behind this... then we are inside solide geometry fully. Shouldn't do anything about that. Can't really either.

		}
	}
	else
	{
		//this is the juicy part...
		//need to check which way we are crossing....
		// if it is back to front, then ignore. shoudln't render a collision on this... need to send off the parts to each....
		glm::vec3 intPt = Poly::getIntersection(v0, v1, n_plane);

		//check collide above and below
		if (side0 == SIDE::FRONT)
		{
			//front to back.
			//bool result = front->collide(radius, v0, intPt, out, faceNorm, t);
			//if (result)
			//	return true;

			if (back->collide(radius, intPt, v1, out, faceNorm, t))
			{
				if (*t == 1.0)
				{
					//behind is where it collided.... give it the normal;
					*t = 0.0;
					//*out = intPt;
					*faceNorm = plane.xyz();
					//	*out += 1.0f * plane.xyz();
				}
				return true;
			}


			if (front->collide(radius, v0, intPt, out, faceNorm, t))
			{
				if (*t == 1.0)
				{
					//behind is where it collided.... give it the normal;
					*t = 0.0;
					//*out = intPt;
					*faceNorm = plane.xyz();
					//	*out += 1.0f * plane.xyz();
				}
				return true;
			}


			//if it doesn't collide in thr front, could collide in the behind...
			// if it colludes in the back... then... 

		
			return false;
		}
		else
		{
			//back to front...
			// shouldn't do anything with this if it does return collision... could it?
			bool result = false;
			result |= front->collide(radius, intPt, v1, out, faceNorm, t);
			result |= back->collide(radius, v0, intPt, out, faceNorm, t);

			if (*t == 1.0)
			{
				//behind is where it collided....
				*t = 0.0;
				return false;
			}
			return result;
		}
	}*/
	/*
	glm::vec3 w0, w1;
	bool hit;
	if (leaf == LEAF::EMPTY)
	{
		return false;
	}
	if (leaf == LEAF::SOLID)
	{
		*out = v0;
		return true;
	}

	if (glm::dot(root->norm, v1 - v0) > 0)
	{
		if (rayUnder(plane, 0.0, v0, v1, &w0, &w1))
		{
			hit = back->collide(radius, w0, w1, out, faceNorm, t);
			if(hit)
				v1 = 
		}
	}
	else
	{

	}*/
	/*
	if (leaf == LEAF::SOLID)
	{
		//this is a solid block... so it does collide
		return true;
	}

	glm::vec3 v = v1 - v0;//direction

	int side0 = this->getPtSide(v0);
	int side1 = this->getPtSide(v1);

	if (side1 != side0)
	{
		//transverse
		
		

	}
	else
	{
		//all on one side...
		if (side0 == SIDE::FRONT)
		{
			//both on front
			if (front)
			{
				return front->collide(radius, v0, v1, out, faceNorm, t);
			}
		}
		else
		{
			//both behind
			if (back)
			{
				return back->collide(radius, v0, v1, out, faceNorm, t);
			}
		}
	}
	*/
}

bool BSP::Node::rayCastNode(std::shared_ptr<Node> node, glm::vec3 v0, glm::vec3 v1, glm::vec3* impactPt, glm::vec3* norm)
{
	if (node->leaf == LEAF::EMPTY)
	{
		return false;
	}
	if (node->leaf == LEAF::SOLID)
	{
		*impactPt = v0;
		return true;
	}

	float d0, d1;
	d0 = glm::dot(node->plane.xyz(), v0) + node->plane.w;
	d1 = glm::dot(node->plane.xyz(), v1) + node->plane.w;

	if (d0 <= 0.0f && d1 <= 0.0f)
	{
		return rayCastNode(node->back, v0, v1, impactPt, norm);
	}
	if (d0 > 0.0f && d1 > 0.0f)
	{
		return rayCastNode(node->front, v0, v1, impactPt, norm);
	}

	glm::vec3 midPt = Poly::getIntersection(v0, v1, node->plane);

	if (d0 <= 0.0f)
	{
		if (rayCastNode(node->back, v0, midPt, impactPt, norm))
		{
			return true;
		}

		*norm = -node->plane.xyz();
		return rayCastNode(node->front, midPt, v1, impactPt, norm);
	}

	if (rayCastNode(node->front, v0, midPt, impactPt, norm))
	{
		return true;
	}

	*norm = node->plane.xyz();
	return rayCastNode(node->back, midPt, v1, impactPt, norm);
}

bool BSP::Node::inside(glm::vec3 pt)
{
	if (leaf == LEAF::EMPTY)
	{
		//terminates in an empty area
		return false;
	}
	if (leaf == LEAF::SOLID)
	{
		//hitting a solid area
		return true;
	}

	//if not we need to traverse the tree...
	glm::vec4 n_plane = root->plane;

	int side0 = getPtSide(pt, n_plane);

	if (side0 == SIDE::FRONT)
	{
		//front
		return front->inside(pt);
	}
	else
	{
		//back
		return back->inside(pt);
	}
}

bool BSP::Node::segmentUnder(const glm::vec4& plane, glm::vec3& v0, glm::vec3& v1, glm::vec3& nv0, glm::vec3* w0, glm::vec3* w1, glm::vec3* nw0)
{
	/*
	int s0 = getPtSide(v0, plane);
	int s1 = getPtSide(v1, plane);

	if (s0 == SIDE::COINCIDENT) s0 = SIDE::BACK;
	if (s1 == SIDE::COINCIDENT) s1 = SIDE::BACK;

	if (s0 == SIDE::FRONT && s1 == SIDE::FRONT)
	{
		return false;
	}
	if (s0 == SIDE::BACK && s0 == SIDE::BACK)
	{
		*w0 = v0;
		*w1 = v1;
		*nw0 = nv0;
		return true;
	}

	glm::vec3 midPt = Poly::getIntersection(v0, v1, plane);

	if (s0 == SIDE::FRONT)
	{
		*w1 = v1;
		*w0 = midPt;
		*nw0 = plane.xyz();
		return true;
	}
	else
	{
		*w1 = midPt;
		*w0 = v0;
		*nw0 = nv0;
		return true;
	}
	//*/

	///*
	float d0, d1;
	d0 = glm::dot(plane.xyz(), v0) + plane.w;
	d1 = glm::dot(plane.xyz(), v1) + plane.w;

	if (d0 > 0.0f && d1 > 0.0f)
	{
		return false;
	}
	if (d0 <= 0.0f && d1 <= 0.0f)
	{
		*w0 = v0;
		*w1 = v1;
		*nw0 = nv0;
		return true;
	}

	glm::vec3 midPt = Poly::getIntersection(v0, v1, plane);
	 
	if (d0 > 0.0f)
	{
		*w1 = v1;
		*w0 = midPt;
		*nw0 = plane.xyz();
		return true;
	}
	else
	{
		*w1 = midPt;
		*w0 = v0;
		*nw0 = nv0;
		return true;
	}//*/
}

bool BSP::Node::segmentOver(const glm::vec4& plane, glm::vec3& v0, glm::vec3& v1, glm::vec3& nv0, glm::vec3* w0, glm::vec3* w1, glm::vec3* nw0)
{
	return segmentUnder(glm::vec4(-plane.xyz(), -plane.w), v0, v1, nv0, w0, w1, nw0);
}

bool BSP::Node::collideSphere(float radius, std::shared_ptr<Node> node, glm::vec3 v0, glm::vec3 v1, glm::vec3* impactPt, glm::vec3& nv0)
{
	/*
	if (glm::length(v1 - v0) < COINCIDENT_MARGAIN)
	{
		if (node->leaf == LEAF::EMPTY)
			std::cout << "EMPTY\n";
		if (node->leaf == LEAF::SOLID)
			std::cout << "SOLID\n";

		return false;
	}
	*/
	/*
	if (node->leaf == LEAF::EMPTY)
	{
		//terminates in an empty area
		return false;
	}
	if (node->leaf == LEAF::SOLID)
	{
		//hitting a solid area
		*impactPt = v0;
		//if(nv0 != glm::vec3(0))
	//	impactNormal = nv0;
		return true;
	}

	float d0, d1;
	d0 = glm::dot(node->plane.xyz(), v0) + node->plane.w;
	d1 = glm::dot(node->plane.xyz(), v1) + node->plane.w;

	if (d0 >= 0.0f && d1 >= 0.0f)
	{
		return collideSphere(radius, node->front, v0, v1, impactPt, nv0);
	}
	if (d0 < 0.0f && d1 < 0.0f)
	{
		return collideSphere(radius, node->back, v0, v1, impactPt, nv0);
	}

	glm::vec3 midPt = Poly::getIntersection(v0, v1, node->plane);

	if (d0 < 0.0f)
	{

		if (collideSphere(radius, node->back, v0, midPt, impactPt, nv0))
		{
			return true;
		}

		impactNormal = -node->plane.xyz();
		return collideSphere(radius, node->front, midPt, v1, impactPt, nv0);
	}

	if (collideSphere(radius, node->front, v0, midPt, impactPt, nv0))
	{
		return true;
	}

	impactNormal = node->plane.xyz();
	return collideSphere(radius, node->back, midPt, v1, impactPt, nv0);
	*/

	if (node->leaf == LEAF::EMPTY)
	{
		//terminates in an empty area
		return false;
	}
	if (node->leaf == LEAF::SOLID)
	{
		//hitting a solid area
		
		//if (nv0 != glm::vec3(0))
		
		*impactPt = v0;
		impactNormal = nv0;
		return true;
	}

	//std::cout << node->plane.x << " : " << node->plane.y << " : " << node->plane.z << "\n";

	glm::vec3 w0, w1, nw0;
	bool hit = false;
	if (segmentUnder(glm::vec4(node->plane.xyz(), node->plane.w - radius), v0, v1, nv0, &w0, &w1, &nw0))
	{
		hit |= collideSphere(radius, node->back, w0, w1, &v1, nw0);
	}
	if (segmentOver(glm::vec4(node->plane.xyz(), node->plane.w + radius), v0, v1, nv0, &w0, &w1, &nw0))
	{
		hit |= collideSphere(radius, node->front, w0, w1, &v1, nw0);
	}

	if (hit)
	{
		*impactPt = v1;
	}

	return hit;
	//*/

	/*
	if (node->leaf == LEAF::EMPTY)
	{
		//terminates in an empty area
		return false;
	}
	if (node->leaf == LEAF::SOLID)
	{
		//hitting a solid area
		*impactPt = v0;
		impactNormal = nv0;
		//*faceNorm = 
		return true;
	}

	glm::vec3 w0, w1, nw0;
	bool hit;
	if (segmentUnder(glm::vec4(node->plane.xyz(), node->plane.w - radius), v0, v1, nv0, &w0, &w1, &nw0))
	{
		hit |= collideSphere(radius, node->back, w0, w1, &v1, nw0);
	}
	if (segmentOver(glm::vec4(node->plane.xyz(), node->plane.w + radius), v0, v1, nv0, &w0, &w1, &nw0))
	{
		hit |= collideSphere(radius, node->front, w0, w1, &v1, nw0);
	}
	if (hit)
	{
		*impactPt = v1;
	}

	return hit;
	*/
}

bool BSP::Node::collideCylinder(float radius, float hieght, std::shared_ptr<Node> node, glm::vec3 v0, glm::vec3 v1, glm::vec3* impactPt, glm::vec3& nv0)
{
	//std::cout << "stert\n";
	//std::cout << v0.x << ", " << v0.y << ", " << v0.z << "\n";
	//std::cout << v1.x << ", " << v1.y << ", " << v1.z << "\n";

	/*if (glm::length(v1 - v0) < COINCIDENT_MARGAIN)
	{
		return false;
	}*/

	//https://github.com/melax/sandbox/blob/master/testbsp/bspcollide.cpp
	if (node->leaf == LEAF::EMPTY)
	{
		//terminates in an empty area
		return false;
	}
	if (node->leaf == LEAF::SOLID)
	{
		//hitting a solid area
		*impactPt = v0;
		impactNormal = nv0;
		return true;
	}

	//std::cout << radius << " : " << hieght << "\n";

	//std::cout << node->plane.x << " : " << node->plane.y << " : " << node->plane.z << "\n";
	//std::cout << "2: " << -glm::dot(getTangentPointOnCylinder(radius, 2.0, -node->plane.xyz()), -node->plane.xyz()) << "\n";
	//std::cout << "1: " << -glm::dot(getTangentPointOnCylinder(radius, 1.0, -node->plane.xyz()), -node->plane.xyz()) << "\n";
	//std::cout << "0.5: " << -glm::dot(getTangentPointOnCylinder(radius, 0.5, -node->plane.xyz()), -node->plane.xyz()) << "\n";

	float offset_up = -glm::dot(getTangentPointOnCylinder(radius, hieght, -node->plane.xyz()), -node->plane.xyz());
	float offset_down = glm::dot(getTangentPointOnCylinder(radius, hieght, node->plane.xyz()), node->plane.xyz());

	glm::vec3 w0, w1, nw0;
	bool hit = false;
	//if (segmentUnder(node->plane, v0, v1, nv0, &w0, &w1, &nw0))
	if (segmentUnder(glm::vec4(node->plane.xyz(), node->plane.w + offset_up), v0, v1, nv0, &w0, &w1, &nw0))
	{
		hit |= collideCylinder(radius, hieght, node->back, w0, w1, &v1, nw0);
	}
	//if (segmentOver(node->plane, v0, v1, nv0, &w0, &w1, &nw0))
	if (segmentOver(glm::vec4(node->plane.xyz(), node->plane.w + offset_down), v0, v1, nv0, &w0, &w1, &nw0))
	{
		hit |= collideCylinder(radius, hieght, node->front, w0, w1, &v1, nw0);
	}

	if (hit)
	{
		*impactPt = v1;
	}

	return hit;
}

glm::vec3 BSP::Node::getTangentPointOnCylinder(float radius, float hieght, const glm::vec3& normal)
{
	glm::vec3 pt;
	float xzMag = sqrtf(normal.x * normal.x + normal.z * normal.z);
	if (xzMag == 0.0f)
	{
		xzMag = 1.0f;
	}

	pt.x = radius * normal.x / xzMag;
	pt.z = radius * normal.z / xzMag;
	pt.y = (normal.y > 0.0f) ? hieght : 0;
	//pt.y = (normal.y > 0.0f) ? hieght / 2 : -hieght / 2;

	return pt;
}

/*
bool BSP::Node::segmentUnder(glm::vec4 plane, glm::vec3 v0, glm::vec3 v1, glm::vec3 nv0, glm::vec3* w0, glm::vec3* w1, glm::vec3* nw0)
{
	float d0, d1;
	d0 = glm::dot(plane.xyz(), v0) + plane.w;
	d1 = glm::dot(plane.xyz(), v1) + plane.w;
	if (d0 > 0.0f && d1 > 0.0f)
	{
		return false;
	}
	if (d0 <= 0.0f && d1 <= 0.0f)
	{
		*w0 = v0;
		*w1 - v1;
		*nw0 = nv0;
		return true;
	}

	glm::vec3 midPt = Poly::getIntersection(v0, v1, plane);

	if (d0 > 0.0f)
	{
		*w1 = v1;
		*w0 = midPt;
		*nw0 = plane.xyz();
		return true;
	}
	else
	{
		*w1 = midPt;
		*w0 = v0;
		*nw0 = nv0;
		return true;
	}
}

bool BSP::Node::segmentOver(glm::vec4 plane, glm::vec3 v0, glm::vec3 v1, glm::vec3 nv0, glm::vec3* w0, glm::vec3* w1, glm::vec3* nw0)
{
	return segmentUnder(glm::vec4(-plane.xyz(), - plane.w), v0, v1, nv0, w0, w1, nw0);
}

bool BSP::Node::collideSphere(float radius, glm::vec3 v0, glm::vec3 v1, glm::vec3* out, glm::vec3* faceNorm)
{
	if (leaf == LEAF::EMPTY)
	{
		//terminates in an empty area
		return false;
	}
	if (leaf == LEAF::SOLID)
	{
		//hitting a solid area
		*out = v0;
		//*faceNorm = 
		return true;
	}

	glm::vec3 w0, w1, nw0;
	bool hit;
	if (segmentUnder(glm::vec4(plane.xyz(), plane.w - radius), v0, v1, *faceNorm, &w0, &w1, &nw0))
	{
		hit |= front->collideSphere(radius, w0, w1, &v1, &nw0);
	}
	if (segmentOver(glm::vec4(plane.xyz(), plane.w + radius), v0, v1, *faceNorm, &w0, &w1, &nw0))
	{
		hit |= back->collideSphere(radius, w0, w1, &v1, &nw0);
	}
	if (hit)
	{
		*out = v1;
	}

	return hit;
}
*/

void BSP::Node::splitPoly(std::shared_ptr<Poly> poly, std::shared_ptr<Poly>* frontPart, std::shared_ptr<Poly>* backPart)
{
	//https://people.eecs.berkeley.edu/~jrs/274s19/bsptreefaq.html

	std::vector<glm::vec3> fPts, bPts;
	int count = poly->pts.size();
	glm::vec3 ptA, ptB;
	int sideA, sideB;
	ptA = poly->pts[count - 1];
	sideA = getPtSide(ptA);
	for (int i = 0; i < count; i++)
	{
		ptB = poly->pts[i];
		sideB = getPtSide(ptB);
		if (sideB == SIDE::FRONT)
		{
			if (sideA == SIDE::BACK)
			{
				//need to find intersection pt
				glm::vec3 intPt = root->getIntersection(ptA, ptB); 
				//std::cout << intPt.x << " : " << intPt.y << " : " << intPt.z << "\n";
				fPts.push_back(intPt);
				bPts.push_back(intPt);
			}
			fPts.push_back(ptB);
		}
		else if (sideB == SIDE::BACK)
		{
			if (sideA == SIDE::FRONT)
			{
				//need to find intersection pt
				glm::vec3 intPt = root->getIntersection(ptA, ptB);
				//std::cout << intPt.x << " : " << intPt.y << " : " << intPt.z << "\n";
				fPts.push_back(intPt);
				bPts.push_back(intPt);
			}
			bPts.push_back(ptB);
		}
		else
		{
			//coincident
			//add to both...
			fPts.push_back(ptB);
			bPts.push_back(ptB);
		}
		ptA = ptB;
		sideA = sideB;
	}

	
	for (int i = 0; i < fPts.size(); i++)
	{
		if (isnan(fPts[i].x) || isnan(fPts[i].y) || isnan(fPts[i].z))
		{
			std::cout << i << " : " << fPts[i].x << " : " << fPts[i].y << " : " << fPts[i].z << "\n";
		}
	}
	for (int i = 0; i < bPts.size(); i++)
	{
		if (isnan(bPts[i].x) || isnan(bPts[i].y) || isnan(bPts[i].z))
		{
			std::cout << i << " : " << bPts[i].x << " : " << bPts[i].y << " : " << bPts[i].z << "\n";
		}
	}
	

	*frontPart = std::shared_ptr<Poly>(new Poly(fPts, poly->norm, poly->plane));
	*backPart = std::shared_ptr<Poly>(new Poly(bPts, poly->norm, poly->plane));
}

int BSP::Node::getPtSide(glm::vec3 pt, glm::vec4 plane)
{
	//float d = -glm::dot(plane.xyz(), pt);
	float d = glm::dot(plane.xyz(), pt) + plane.w;
	if (std::isnan(d) || !std::isfinite(d))
	{
		return SIDE::UNDEFINED;
	}
	if (fabs(d) < COINCIDENT_MARGAIN)
	{
		//coincident
		return SIDE::COINCIDENT;
	}
	else if (d > 0.0f)
	{
		//front
		return SIDE::FRONT;
	}
	else
	{
		//back
		return SIDE::BACK;
	}
	/*
	if (fabs(d - plane.w) < COINCIDENT_MARGAIN)
	{
		//coincident
		return SIDE::COINCIDENT;
	}
	else if (d < plane.w)
	{
		//front
		return SIDE::FRONT;
	}
	else
	{
		//back
		return SIDE::BACK;
	}
	*/
}

int BSP::Node::getPolySide(std::shared_ptr<Poly> polygon, glm::vec4 plane)
{
	bool onlyFront = true;
	bool onlyBack = true;
	bool onlyCoincident = true;

	for (glm::vec3 pt : polygon->pts)
	{
		int side = getPtSide(pt, plane);
		if (side == SIDE::UNDEFINED)
		{
			return SIDE::UNDEFINED;
		}
		if (side == SIDE::COINCIDENT)
		{
			//coincident
			//don't update anything...

		}
		else if (side == SIDE::FRONT)
		{
			//front
			onlyBack = false;
			onlyCoincident = false;
		}
		else
		{
			//back
			onlyFront = false;
			onlyCoincident = false;
		}
	}

	if (onlyCoincident)
	{
		return SIDE::COINCIDENT;
	}
	else if (onlyFront)
	{
		return SIDE::FRONT;
	}
	else if (onlyBack)
	{
		return SIDE::BACK;
	}
	else
	{
		return SIDE::SPANNING;
	}
}

int BSP::Node::getPtSide(glm::vec3 pt)
{
	return getPtSide(pt, plane);
	/*
	float d = -glm::dot(plane.xyz(), pt);
	if (fabs(d - plane.w) < COINCIDENT_MARGAIN)
	{
		//coincident
		return SIDE::COINCIDENT;
	}
	else if (d > plane.w)
	{
		//front
		return SIDE::FRONT;
	}
	else
	{
		//back
		return SIDE::BACK;
	}
	*/
}

int BSP::Node::getPolySide(std::shared_ptr<Poly> polygon)
{
	return getPolySide(polygon, plane);
	/*
	bool onlyFront = true;
	bool onlyBack = true;
	bool onlyCoincident = true;

	for (glm::vec3 pt : polygon->pts)
	{
		int side = getPtSide(pt);
		if (side == SIDE::COINCIDENT)
		{
			//coincident
			//don't update anything...

		}
		else if (side == SIDE::FRONT)
		{
			//front
			onlyBack = false;
			onlyCoincident = false;
		}
		else
		{
			//back
			onlyFront = false;
			onlyCoincident = false;
		}
	}

	if (onlyCoincident)
	{
		return SIDE::COINCIDENT;
	}
	else if (onlyFront)
	{
		return SIDE::FRONT;
	}
	else if (onlyBack)
	{
		return SIDE::BACK;
	}
	else
	{
		return SIDE::SPANNING;
	}*/
}

void BSP::Node::create(std::vector<std::shared_ptr<Poly>> polygons, int maxPolys, int side)
{
	if (polygons.size() == 0)
	{
		if(side == SIDE::BACK)
			leaf = LEAF::SOLID;
		if (side == SIDE::FRONT)
			leaf = LEAF::EMPTY;
		return;
	}
	//std::cout << this << " : " << polygons.size() << "\n";
	root = polygons[0];
	//std::cout << "create : " << root << "\n";
	//if (!this->root)
	//{
	//	std::cout << "create : " << root << "\n";
	//}
	count++;
	plane = root->plane;
	//std::cout << plane.x << " : " << plane.y << " : " << plane.z << " : " << plane.w << "\n";
	//this->polygons.push_back(root);
	polygons.push_back(root);
	std::vector<std::shared_ptr<Poly>> frontList;
	std::vector<std::shared_ptr<Poly>> backList;
	leaf = LEAF::NOT_LEAF;

	/*if (polygons.size() == 1)// || polygons.size() < maxPolys)
	{
		//ends. either it only has one or it has less than what it wants.
		this->polygons = polygons;
		//leaf = LEAF::EMPTY;
	}
	else
	{
		for (std::shared_ptr<Poly> poly : polygons)
		{
			int side = getPolySide(poly);
			if (side == SIDE::COINCIDENT)
			{
				//if it is coincidnet, then just push it back...
				this->polygons.push_back(poly);
			}
			else if (side == SIDE::FRONT)
			{
				frontList.push_back(poly);
			}
			else if (side == SIDE::BACK)
			{
				backList.push_back(poly);
			}
			else
			{
				//spanning....
				std::shared_ptr<Poly> frontPart, backPart;
				splitPoly(poly, &frontPart, &backPart);
				frontList.push_back(frontPart);
				backList.push_back(backPart);
			}
		}
	}*/

	for (std::shared_ptr<Poly> poly : polygons)
	{
		//if (poly == root) continue;
		int side = getPolySide(poly);

		if (poly == root)
		{
		//	std::cout << side << "\n";
		}

		//std::cout << poly->plane.w << "\n";

		if (side == SIDE::COINCIDENT)
		{
			//if it is coincidnet, then just push it back...
			// if it is pointed in the front, just add to this plane,
			// if it is pointed behind, push it behind
			//this->polygons.push_back(poly);
			//this->polygons.push_back(poly);
			//this->polygons.push_back(poly);

			/*
			if (glm::dot(poly->plane.xyz(), plane.xyz()) >= 0.0)
				this->polygons.push_back(poly); //std::cout << "front\n";
			else
				std::cout << "back !!!!!\n";
			*/
			this->polygons.push_back(poly);
			//std::cout << poly->plane.x << " : " << poly->plane.y << " : " << poly->plane.z << " : " << poly->plane.w << "\n";
			//std::cout << glm::dot(poly->plane.xyz(), plane.xyz()) << "\n";
			////this->polygons.push_back(poly);
			//if (glm::dot(poly->plane.xyz(), plane.xyz()) >= 0.0)
			//	this->polygons.push_back(poly);
			//else
			//	backList.push_back(poly);
		}
		else if (side == SIDE::FRONT)
		{
			frontList.push_back(poly);
		}
		else if (side == SIDE::BACK)
		{
			backList.push_back(poly);
		}
		else if(side == SIDE::SPANNING)
		{
			//spanning....
			std::shared_ptr<Poly> frontPart, backPart;
			splitPoly(poly, &frontPart, &backPart);
			frontList.push_back(frontPart);
			backList.push_back(backPart);
		}
	}

	front = std::shared_ptr<Node>(new Node);
	front->create(frontList, maxPolys, SIDE::FRONT);

	back = std::shared_ptr<Node>(new Node);
	back->create(backList, maxPolys, SIDE::BACK);
	
	/*if (true || frontList.size() > 0)
	{
		front = std::shared_ptr<Node>(new Node);
		front->create(frontList, maxPolys, SIDE::FRONT);
	}
	if (true || backList.size() > 0)
	{
		//we add back even if empty because we want to create a solid backwarness
		back = std::shared_ptr<Node>(new Node);
		back->create(backList, maxPolys, SIDE::BACK);
	}*/

}

void BSP::Node::appendPolys(std::vector<std::shared_ptr<Poly>>* polygons)
{
	polygons->insert(std::end(*polygons), std::begin(this->polygons), std::end(this->polygons));
	if (front)
	{
		front->appendPolys(polygons);
	}
	if (back)
	{
		back->appendPolys(polygons);
	}
}
