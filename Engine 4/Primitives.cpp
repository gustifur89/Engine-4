#include "Primitives.h"

// ===================== Capsule =======================

std::shared_ptr<GameObjectColor> Primitive::makeCapsule(float radius, float height, std::shared_ptr<ColorShader> shader, float r, float g, float b, float specular)
{
	std::shared_ptr<GameObjectColor> capsule = std::shared_ptr<GameObjectColor>(new GameObjectColor());

	std::shared_ptr<GameObjectColor> topCap = std::shared_ptr<GameObjectColor>(new GameObjectColor());
	topCap->mesh = Primitive::capsuleCap;
	topCap->shader = shader;
	topCap->transform.setPosition(0, height / 2, 0);
	topCap->transform.setScale(radius, radius, radius);
	topCap->setFillColor(r, g, b);
	topCap->setShininess(specular);
	capsule->addChild(topCap);

	std::shared_ptr<GameObjectColor> bottomCap = std::shared_ptr<GameObjectColor>(new GameObjectColor());
	bottomCap->mesh = Primitive::capsuleCap;
	bottomCap->shader = shader;
	bottomCap->transform.setPosition(0, -height / 2, 0);
	bottomCap->transform.setScale(radius, radius, radius);
	bottomCap->transform.setRotation(0, 0, 180);
	bottomCap->setFillColor(r, g, b);
	bottomCap->setShininess(specular);
	capsule->addChild(bottomCap);

	std::shared_ptr<GameObjectColor> cyl = std::shared_ptr<GameObjectColor>(new GameObjectColor());
	cyl->mesh = Primitive::capsuleCyl;
	cyl->shader = shader;
	cyl->transform.setScale(radius, height, radius);
	cyl->setFillColor(r, g, b);
	cyl->setShininess(specular);
	capsule->addChild(cyl);
	
	capsule->shader = shader;

	capsule->collider = std::shared_ptr<Capsule>(new Capsule(radius, height));

	return capsule;
}

// ==================== AABB ========================

std::shared_ptr<GameObjectColor> Primitive::makeAABB(glm::vec3 size, std::shared_ptr<ColorShader> shader, float r, float g, float b, float specular)
{
//	std::shared_ptr<GameObjectColor> object = std::shared_ptr<GameObjectColor>(new GameObjectColor());

	std::shared_ptr<GameObjectColor> cube = std::shared_ptr<GameObjectColor>(new GameObjectColor());
	cube->mesh = Primitive::cube;
	cube->shader = shader;
	cube->transform.setScale(size);
	cube->setFillColor(r, g, b);
	cube->setShininess(specular);
	cube->collider = std::shared_ptr<AABB>(new AABB(size));

//	object->addChild(cube);

	return cube;
}

std::shared_ptr<GameObjectColor> Primitive::makeSphere(float radius, std::shared_ptr<ColorShader> shader, float r, float g, float b, float specular)
{
	//std::shared_ptr<GameObjectColor> object = std::shared_ptr<GameObjectColor>(new GameObjectColor());

	std::shared_ptr<GameObjectColor> spheroid = std::shared_ptr<GameObjectColor>(new GameObjectColor());
	spheroid->mesh = Primitive::sphere;
	spheroid->shader = shader;
	spheroid->setFillColor(r, g, b);
	spheroid->transform.setScale(glm::vec3(radius));
	spheroid->setShininess(specular);
	spheroid->collider = std::shared_ptr<Sphere>(new Sphere(radius));
	spheroid->radius = radius;
	//object->addChild(spheroid);

	return spheroid;
}

std::shared_ptr<GameObjectColor> Primitive::makeEllipsoid(glm::vec3 axis, std::shared_ptr<ColorShader> shader, float r, float g, float b, float specular)
{
	std::shared_ptr<GameObjectColor> ellipsoid = std::shared_ptr<GameObjectColor>(new GameObjectColor());
	ellipsoid->mesh = Primitive::sphere;
	ellipsoid->shader = shader;
	ellipsoid->setFillColor(r, g, b);
	ellipsoid->transform.setScale(axis);
	ellipsoid->setShininess(specular);
	ellipsoid->collider = std::shared_ptr<Ellipsoid>(new Ellipsoid(axis));

	return ellipsoid;
}

// ===================== Primitives =====================

std::shared_ptr<ColorMesh> Primitive::capsuleCap = NULL;
std::shared_ptr<ColorMesh> Primitive::capsuleCyl = NULL;
std::shared_ptr<ColorMesh> Primitive::cube = NULL;
std::shared_ptr<ColorMesh> Primitive::sphere = NULL;

void Primitive::init()
{
	capsuleCap = ColorMesh::loadFromFilePLY("capsuleCap");
	capsuleCyl = ColorMesh::loadFromFilePLY("capsuleCyl");
	cube = ColorMesh::loadFromFilePLY("cube");
	sphere = ColorMesh::loadFromFilePLY("smoothSphere");
}

