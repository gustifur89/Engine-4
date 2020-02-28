#pragma once
#include "Header.h"
#include "Shader.h"

class SSAOShader : public Shader
{
public:
	static std::shared_ptr<SSAOShader> loadShader(std::string vertName, std::string fragName);
};

