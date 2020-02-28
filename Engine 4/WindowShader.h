#pragma once
#include "Header.h"
#include "Shader.h"

class WindowShader : public Shader
{
protected:
	GLuint globalLightLoc;
	GLuint viewMatrixLoc;
	GLuint normalViewMatrixLoc;
	GLuint ambientLoc;

public:
	void setGlobalLight(glm::vec3 globalLight);
	void setViewMatrix(glm::mat4 viewMatrix);
	void setAmbient(float ambient);

	static std::shared_ptr<WindowShader> loadShader(std::string vertName, std::string fragName);
};

