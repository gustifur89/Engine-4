#pragma once
#include "Header.h"
#include "Texture.h"
#include "WindowShader.h"
#include "Shader.h"
#include "Mesh.h"
#include "GameObject.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

class FileReader
{
private:
	static int getGLFilterModeFromString(std::string filterMode);
public:
	static std::map<std::string, std::shared_ptr<Mesh>> readMeshFile(std::string fileName);
	static std::map<std::string, std::shared_ptr<Shader>> readShaderFile(std::string fileName);
	static std::map<std::string, std::shared_ptr<Texture>> readTextureFile(std::string fileName);
	static std::map<std::string, std::shared_ptr<GameObject>> readObjectFile(std::string fileName, std::map<std::string, std::shared_ptr<Mesh>>& meshCollection, std::map<std::string, std::shared_ptr<Shader>>& shaderCollection, std::map<std::string, std::shared_ptr<Texture>>& textureCollection);
	static void setPlayerSettings(std::string fileName, float* fov, float* sensitivity, float* speed, std::string* playerTex);
	static std::vector<std::string> split(std::string str, std::string delim);
	static std::vector<float> splitFloat(std::string str, std::string delim);
	static std::vector<int> splitInt(std::string str, std::string delim);
	static void readXML(std::string fileName);
};

