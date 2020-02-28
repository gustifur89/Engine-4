#include "WindowShader.h"

void WindowShader::setGlobalLight(glm::vec3 globalLight)
{
	useShader();
	loadVector(globalLightLoc, globalLight);
}

void WindowShader::setViewMatrix(glm::mat4 viewMatrix)
{
	useShader();
	loadMatrix(viewMatrixLoc, viewMatrix);
	loadMatrix(normalViewMatrixLoc, glm::transpose(glm::inverse(viewMatrix)));
}

void WindowShader::setAmbient(float ambient)
{
	loadFloat(ambientLoc, ambient);
}

std::shared_ptr<WindowShader> WindowShader::loadShader(std::string vertName, std::string fragName)
{
	std::shared_ptr<WindowShader> out(new WindowShader);
	out->Shader::loadShader_(vertName, fragName);
	out->useShader();
	out->globalLightLoc = out->getUniformLocation("globalLight"); 
	out->viewMatrixLoc = out->getUniformLocation("viewMatrix");
	out->normalViewMatrixLoc = out->getUniformLocation("normalViewMatrix");
	out->ambientLoc = out->getUniformLocation("ambient");
	return out;
}
