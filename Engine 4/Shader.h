#pragma once
#include "Headers.h"
#include "Texture.h"
#include "RenderTexture.h"

class Shader
{
protected:
	void loadShader_(std::string vertexFile, std::string fragmentFile);
	void loadShader_(std::string vertexFile, std::string fragmentFile, std::string geometryFile);
	void loadShaderPreprocess_(std::string vertexFile, std::string fragmentFile, std::string preproccessorStuff);

public:
	Shader();
	~Shader();

	static std::shared_ptr<Shader> loadShader(std::string vertexFile, std::string fragmentShader, std::string geometryShader);
	static std::shared_ptr<Shader> loadShader(std::string vertexFile, std::string fragmentShader);
	static std::shared_ptr<Shader> loadShaderPreprocess(std::string vertexFile, std::string fragmentFile, std::string preproccessorStuff);
	static std::shared_ptr<Shader> loadShader(std::string fileName);

	void useShader();

	GLint getUniformLocation(std::string uniformName);

	void loadFloat(int location, float value);

	void loadInt(int location, int value);

	void loadVector(int location, glm::vec3 vector);

	void loadBoolean(int location, bool value);

	void loadTexture(int location, int textureID);

	void loadCubeMap(int location, int textureID);

	void loadMatrix(int location, glm::mat4 matrix);

	void loadMatrix(int location, glm::mat3 matrix);

	GLuint programID;
};

class ColorShader : public Shader
{
private:
	GLuint shininessLoc;
	GLuint mvp, mv, nm, cm, mm;
	glm::mat4 colorMatrix;

public:	
	
	static std::shared_ptr<ColorShader> loadShader(std::string fileName);

	void setMatrixes(glm::mat4 MVP, glm::mat4 MV, glm::mat4 NM, glm::mat4 MM, float shininess, glm::mat4 colorMatrix = glm::mat4(1.0));
};

class TextureShader : public Shader
{
public:
	GLuint shininessLoc;
	int mvp, mv, nm, cm, mm, texLoc;

	static std::shared_ptr<TextureShader> loadShader(std::string fileName);
	void setTexture(std::shared_ptr<Texture> texture);
	void setMatrixes(glm::mat4 MVP, glm::mat4 MV, glm::mat4 NM, glm::mat4 MM, float shininess, glm::mat4 colorMatrix = glm::mat4(1.0));
};

class PortalShader : public Shader
{
public:
	int mvp, mv, cm, colLoc, depLoc;

	static std::shared_ptr<PortalShader> loadShader(std::string fileName);
	void setTexture(std::shared_ptr<ScreenBufferRenderTexture> texture);
	void setMatrixes(glm::mat4 MVP, glm::mat4 MV, glm::mat4 colorMatrix = glm::mat4(1.0));
};

class SkyBoxShader : public Shader
{
public:
	int pvm, cm, skyBoxLoc;

	static std::shared_ptr<SkyBoxShader> loadShader(std::string fileName);
	void setSkyBoxTexture(std::shared_ptr<SkyBoxTexture> skyBoxTexture);
	void setMatrixes(glm::mat4 PVM, glm::mat4 colorMatrix = glm::mat4(1.0));
};
