#pragma once
#include "Header.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "GameObject.h"

class SkyBox : public GameObject
{
private:
	static const GLfloat skyboxVertices[];

	GLuint skyboxVBO;
	GLuint skyboxVAO;

public:
	glm::mat4 colorMatrix;
	std::shared_ptr<SkyBoxTexture> texture;
	std::shared_ptr<SkyBoxShader> shader;
	Transform transform;

	SkyBox();

	//void render(std::shared_ptr<Camera> camera);
	void renderFunc(std::shared_ptr<Camera> camera, glm::mat4 parentTrasform);
	
	//TO work with portals, it disallows easy changing of skyboxes, since this is now an object and goes into the render pipeline as such...

};

