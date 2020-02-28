#include "SSAOShader.h"

std::shared_ptr<SSAOShader> SSAOShader::loadShader(std::string vertName, std::string fragName)
{
	std::shared_ptr<SSAOShader> out(new SSAOShader);
	out->Shader::loadShader_(vertName, fragName);
	out->useShader();
	//out->globalLightLoc = out->getUniformLocation("globalLight");
	//out->viewMatrixLoc = out->getUniformLocation("viewMatrix");
	//out->normalViewMatrixLoc = out->getUniformLocation("normalViewMatrix");
	//out->ambientLoc = out->getUniformLocation("ambient");
	return out;
}
