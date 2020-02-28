#pragma once
#include "Header.h"
#include "GameObject.h"

class Primitive
{
public:
	static void init();


	static std::shared_ptr<ColorMesh> capsuleCap;
	static std::shared_ptr<ColorMesh> capsuleCyl;
	static std::shared_ptr<ColorMesh> cube;
	static std::shared_ptr<ColorMesh> sphere;

	static std::shared_ptr<GameObjectColor> makeCapsule(float radius, float height, std::shared_ptr<ColorShader> shader, float r, float g, float b, float specular);
	static std::shared_ptr<GameObjectColor> makeAABB(glm::vec3 size, std::shared_ptr<ColorShader> shader, float r, float g, float b, float specular);
	static std::shared_ptr<GameObjectColor> makeSphere(float radius, std::shared_ptr<ColorShader> shader, float r, float g, float b, float specular);
	static std::shared_ptr<GameObjectColor> makeEllipsoid(glm::vec3 axis, std::shared_ptr<ColorShader> shader, float r, float g, float b, float specular);

};



