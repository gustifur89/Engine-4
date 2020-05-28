#include "FileReader.h"

std::map<std::string, std::shared_ptr<Mesh>> FileReader::readMeshFile(std::string fileName)
{
	std::map<std::string, std::shared_ptr<Mesh>> meshCollection;

	fileName = std::string("src/data/") + fileName;
	std::ifstream file;
	file = std::ifstream(fileName);

	if (file.fail())
	{
		fprintf(stderr, "failed to load file at : ");
		fprintf(stderr, fileName.c_str());
		fprintf(stderr, "\n");
		return meshCollection;
	}

	while (!file.eof())
	{
		std::string line;
		file >> line;

		//test for comment
		if (line.find("//") != -1)
		{
			//skip to next line...
			std::getline(file, line);
		}

		if (line == "ColorMesh")
		{
			std::string name;
			std::string meshFile;
			std::string type;
			std::string animType;
			file >> type >> animType >> name >> meshFile;
			std::shared_ptr<ColorMesh> colMesh;
			if (type == "ply")
			{
				if(animType == "static")
					colMesh = ColorMesh::loadFromFilePLY(meshFile);
				if (animType == "dynamic")
					colMesh = ColorMesh::loadFromFilePLY(meshFile, true);
			}
			if (colMesh == NULL) {
				std::cout << "Failled to create mesh : " << meshFile << "\n";
			}
			meshCollection[name] = colMesh;
		}
		else if (line == "TextureMesh")
		{
			std::string name;
			std::string meshFile;
			std::string type;
			std::string animType;
			file >> type >> animType >> name >> meshFile;
			std::shared_ptr<TextureMesh> texMesh;
			if (type == "ply")
			{
				if (animType == "static")
					texMesh = TextureMesh::loadFromFilePLY(meshFile);
				if (animType == "dynamic")
					texMesh = TextureMesh::loadFromFilePLY(meshFile, true);
			}
			else if (type == "obj")
			{
				texMesh = TextureMesh::loadFromFileOBJ(meshFile);
			}
			if (texMesh == NULL) {
				std::cout << "Failled to create mesh : " << meshFile << "\n";
			}
			meshCollection[name] = texMesh;
		}
		else if (line == "GenericMesh")
		{
			std::string name;
			std::string meshFile;
			std::string type;
			std::string animType;
			file >> type >> animType >> name >> meshFile;
			std::shared_ptr<GenericMesh> genMesh;
			if (type == "ply")
			{
				if (animType == "static")
					genMesh = GenericMesh::loadFromFilePLY(meshFile);
				if (animType == "dynamic")
					genMesh = GenericMesh::loadFromFilePLY(meshFile, true);
			}
			if (genMesh == NULL) {
				std::cout << "Failled to create mesh : " << meshFile << "\n";
			}
			meshCollection[name] = genMesh;
		}
	}

	return meshCollection;

}

std::map<std::string, std::shared_ptr<Shader>> FileReader::readShaderFile(std::string fileName)
{
	std::map<std::string, std::shared_ptr<Shader>> shaderCollection;

	fileName = std::string("src/data/") + fileName;
	std::ifstream file;
	file = std::ifstream(fileName);

	if (file.fail())
	{
		fprintf(stderr, "failed to load file at : ");
		fprintf(stderr, fileName.c_str());
		fprintf(stderr, "\n");
		return shaderCollection;
	}

	while (!file.eof())
	{
		std::string line;
		file >> line;

		//test for comment
		if (line.find("//") != -1)
		{
			//skip to next line...
			std::getline(file, line);
		}

		if (line == "WindowShader")
		{
			std::string name;
			std::string vertFile;
			std::string fragFile;
			file >> name >> vertFile >> fragFile;
			std::shared_ptr<WindowShader> shader = WindowShader::loadShader(vertFile, fragFile);
			shaderCollection[name] = shader;
		}
		else if (line == "PortalShader")
		{
			std::string name;
			std::string vertFile;
			std::string fragFile;
			file >> name >> vertFile >> fragFile;
			std::shared_ptr<PortalShader> shader = PortalShader::loadShader(vertFile);
			shaderCollection[name] = shader;
		}
		else if (line == "Shader")
		{
			std::string name;
			std::string vertFile;
			std::string fragFile;
			file >> name >> vertFile >> fragFile;
			std::shared_ptr<Shader> shader = Shader::loadShader(vertFile, fragFile);
			shaderCollection[name] = shader;
		}
		else if (line == "ColorShader")
		{
			std::string name;
			std::string vertFile;
			std::string fragFile;
			file >> name >> vertFile >> fragFile;
			std::shared_ptr<ColorShader> shader = ColorShader::loadShader(vertFile);
			shaderCollection[name] = shader;
		}
		else if (line == "TextureShader")
		{
			std::string name;
			std::string vertFile;
			std::string fragFile;
			file >> name >> vertFile >> fragFile;
			std::shared_ptr<TextureShader> shader = TextureShader::loadShader(vertFile);
			shaderCollection[name] = shader;
		}
		else if (line == "SkyBoxShader")
		{
			std::string name;
			std::string vertFile;
			std::string fragFile;
			file >> name >> vertFile >> fragFile;
			std::shared_ptr<SkyBoxShader> shader = SkyBoxShader::loadShader(vertFile);
			shaderCollection[name] = shader;
		}
	}

	return shaderCollection;
}

int FileReader::getGLFilterModeFromString(std::string filterMode)
{
	if (filterMode == "GL_LINEAR_MIPMAP_LINEAR")
		return GL_LINEAR_MIPMAP_LINEAR;
	if (filterMode == "GL_LINEAR_MIPMAP_NEAREST")
		return GL_LINEAR_MIPMAP_NEAREST;
	if (filterMode == "GL_NEAREST")
		return GL_NEAREST;
	if (filterMode == "GL_LINEAR")
		return GL_LINEAR;
}

std::map<std::string, std::shared_ptr<Texture>> FileReader::readTextureFile(std::string fileName)
{
	std::map<std::string, std::shared_ptr<Texture>> textureCollection;

	fileName = std::string("src/data/") + fileName;
	std::ifstream file;
	file = std::ifstream(fileName);

	if (file.fail())
	{
		fprintf(stderr, "failed to load file at : ");
		fprintf(stderr, fileName.c_str());
		fprintf(stderr, "\n");
		return textureCollection;
	}

	while (!file.eof())
	{
		std::string line;
		file >> line;

		//test for comment
		if (line.find("//") != -1)
		{
			//skip to next line...
			std::getline(file, line);
		}

		if (line == "Texture")
		{
			std::string name;
			std::string fileName;
			std::string filterMode;
			std::string alpha;
			bool alphaBool;
			file >> name >> fileName >> filterMode >> alpha;
			if (alpha == "true") alphaBool = true;
			if (alpha == "false") alphaBool = false;
			std::shared_ptr<Texture> tex = Texture::loadFromFile(fileName, getGLFilterModeFromString(filterMode), alphaBool);
			textureCollection[name] = tex;
		}
		else if (line == "SkyBoxTexture")
		{
			std::string name;
			std::vector<std::string> fileNames(6);
			std::string filterMode;
			std::string alpha;
			file >> name >> fileNames[0] >> fileNames[1] >> fileNames[2] >> fileNames[3] >> fileNames[4] >> fileNames[5] >> filterMode >> alpha;
			bool alphaBool;
			if (alpha == "true") alphaBool = true;
			if (alpha == "false") alphaBool = false;
			std::shared_ptr<SkyBoxTexture> sb_tex = SkyBoxTexture::loadFromFile(fileNames, getGLFilterModeFromString(filterMode), alphaBool);
			textureCollection[name] = sb_tex;
		}
	}

	return textureCollection;

}

void FileReader::setPlayerSettings(std::string fileName, float* fov, float* sensitivity, float* speed, std::string* playerTex)
{
	fileName = std::string("src/data/") + fileName;
	std::ifstream file;
	file = std::ifstream(fileName);

	if (file.fail())
	{
		fprintf(stderr, "failed to load file at : ");
		fprintf(stderr, fileName.c_str());
		fprintf(stderr, "\n");
		return;
	}

	while (!file.eof())
	{
		std::string line;
		file >> line;

		//test for comment
		if (line.find("//") != -1)
		{
			//skip to next line...
			std::getline(file, line);
		}

		if (line == "fov")
		{
			file >> *fov;
		}
		else if (line == "sensitivity")
		{
			file >> *sensitivity;
		}
		else if (line == "speed")
		{
			file >> *speed;
		}
		else if (line == "playerTexture")
		{
			file >> *playerTex;
		}
	}
}
