#pragma once
#include "Header.h"
#include "Texture.h"
#include "WindowShader.h"
#include "Shader.h"
#include "Mesh.h"
#include "GameObject.h"

class FileReader
{
private:
	static int getGLFilterModeFromString(std::string filterMode);
public:
	static std::map<std::string, std::shared_ptr<Mesh>> readMeshFile(std::string fileName);
	static std::map<std::string, std::shared_ptr<Shader>> readShaderFile(std::string fileName);
	static std::map<std::string, std::shared_ptr<Texture>> readTextureFile(std::string fileName);
	static void setPlayerSettings(std::string fileName, float* fov, float* sensitivity, float* speed);
};

