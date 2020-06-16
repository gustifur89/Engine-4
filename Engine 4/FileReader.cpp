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
			else if (type == "dae")
			{
				texMesh = TextureMesh::loadFromFileDAE(meshFile);
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
		else if (line == "SkyTexShader")
		{
			std::string name;
			std::string vertFile;
			std::string fragFile;
			file >> name >> vertFile >> fragFile;
			std::shared_ptr<SkyTexShader> shader = SkyTexShader::loadShader(vertFile);
			shaderCollection[name] = shader;
		}
		else if (line == "ColorParticleShader")
		{
			std::string name;
			std::string vertFile;
			std::string fragFile;
			file >> name >> vertFile >> fragFile;
			std::shared_ptr<ColorParticleShader> shader = ColorParticleShader::loadShader(vertFile);
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

std::map<std::string, std::shared_ptr<GameObject>> FileReader::readObjectFile(std::string fileName, std::map<std::string, std::shared_ptr<Mesh>>& meshCollection, std::map<std::string, std::shared_ptr<Shader>>& shaderCollection, std::map<std::string, std::shared_ptr<Texture>>& textureCollection)
{
	std::map<std::string, std::shared_ptr<GameObject>> objectCollection;

	fileName = std::string("src/data/") + fileName;
	std::ifstream file;
	file = std::ifstream(fileName);

	if (file.fail())
	{
		fprintf(stderr, "failed to load file at : ");
		fprintf(stderr, fileName.c_str());
		fprintf(stderr, "\n");
		return objectCollection;
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

		if (line == "GameObjectTexture")
		{
			std::string name;
			std::string shader;
			std::string mesh;
			std::string texture;
			file >> name >> shader >> mesh >> texture;
			std::shared_ptr<GameObjectTexture> objTex = std::shared_ptr<GameObjectTexture>(new GameObjectTexture);
			objTex->shader = std::static_pointer_cast<TextureShader>(shaderCollection[shader]);
			objTex->texture = textureCollection[texture];
			objTex->mesh = meshCollection[mesh];
			objectCollection[name] = objTex;
		}
		else if (line == "GameObjectColor")
		{
			std::string name;
			std::string shader;
			std::string mesh;
			file >> name >> shader >> mesh;

			std::shared_ptr<GameObjectColor> objCol = std::shared_ptr<GameObjectColor>(new GameObjectColor);
			objCol->shader = std::static_pointer_cast<ColorShader>(shaderCollection[shader]);
			objCol->mesh = meshCollection[mesh];
			objectCollection[name] = objCol;
		}
	}

	return objectCollection;
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

std::vector<std::string> FileReader::split(std::string str, std::string delim)
{
	std::vector<std::string> out;
	size_t pos = 0;
	std::string token;
	while ((pos = str.find(delim)) != std::string::npos) {
		token = str.substr(0, pos);
		//std::cout << token << std::endl;
		if (token.length() > 0)
			out.push_back(token);
		str.erase(0, pos + delim.length());
	}
	out.push_back(str);
	return out;
}

std::vector<float> FileReader::splitFloat(std::string str, std::string delim)
{
	std::vector<float> out;
	size_t pos = 0;
	std::string token;
	while ((pos = str.find(delim)) != std::string::npos) {
		token = str.substr(0, pos);
		//std::cout << token << std::endl;
		if (token.length() > 0)
			out.push_back(std::stof(token));
		str.erase(0, pos + delim.length());
	}
	out.push_back(std::stof(str));
	return out;
}

std::vector<int> FileReader::splitInt(std::string str, std::string delim)
{
	std::vector<int> out;
	size_t pos = 0;
	std::string token;
	while ((pos = str.find(delim)) != std::string::npos) {
		token = str.substr(0, pos);
		//std::cout << token << std::endl;
		if (token.length() > 0)
			out.push_back(std::stoi(token));
		str.erase(0, pos + delim.length());
	}
	out.push_back(std::stoi(str));
	return out;
}

void FileReader::readXML(std::string fileName)
{
//	fileName = std::string("src/data/") + fileName;
	
	/*std::ifstream file;
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
	}
	*/

	rapidxml::xml_document<> doc;
	// Read the xml file into a vector
	std::ifstream theFile(fileName);
	std::vector<char> buffer((std::istreambuf_iterator<char>(theFile)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc 
	doc.parse<0>(&buffer[0]);

	//std::cout << doc.name << "\n";

	rapidxml::xml_node<>* root_node = doc.first_node("COLLADA");

	rapidxml::xml_node<>* geometry_node = root_node->first_node("library_geometries")->first_node("geometry");
	//std::cout << "geometry id = " << geometry_node->first_attribute("id")->name() << " value = " << geometry_node->first_attribute("id")->value() << "\n";
	std::string meshName = geometry_node->first_attribute("id")->value();
	rapidxml::xml_node<>* mesh_node = geometry_node->first_node("mesh");
	std::map<std::string, std::vector<float>> data;
	std::map<std::string, int> offsets;
	std::map<std::string, std::vector<int>> materials;
	for (rapidxml::xml_node<>* source = mesh_node->first_node(); source; source = source->next_sibling())
	{
		if (!strcmp(source->name(), "source"))
		{
			std::string id = source->first_attribute("id")->value();
			id = id.substr(meshName.length() + 1, id.length() + 1);
			rapidxml::xml_node<>* float_array = source->first_node("float_array");
			
			std::vector<float> arrayData = splitFloat(float_array->value(), " ");
			data[id] = arrayData;
		}
		else if (!strcmp(source->name(), "vertices"))
		{
			//this is nothing...
		}
		else if (!strcmp(source->name(), "triangles"))
		{
			//this is where we make stuff and split by material...
			std::string material = source->first_attribute("material")->value();
			material = material.substr(0, material.find("-"));

			//std::cout << material << "\n";

			for (rapidxml::xml_node<>* triangleData = source->first_node(); triangleData; triangleData = triangleData->next_sibling())
			{
				if (!strcmp(triangleData->name(), "input"))
				{
					std::string name = triangleData->first_attribute("source")->value();
					name = name.substr(meshName.length() + 1, name.length() + 1);
					if(name == "vertices")
					{
						name = "positions";
					}
					int offset = std::stoi(triangleData->first_attribute("offset")->value());
					offsets[name] = offset;
				}
				else if (!strcmp(triangleData->name(), "p"))
				{
					std::vector<int> arrayData = splitInt(triangleData->value(), " ");
					materials[material] = arrayData;
				}
			}
		}
	}


}
