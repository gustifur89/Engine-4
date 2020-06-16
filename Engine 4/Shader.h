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
	GLuint mvp, mv, nm, cm, mm, dep_mvp;
	glm::mat4 colorMatrix;

public:	
	
	static std::shared_ptr<ColorShader> loadShader(std::string fileName);

	void setMatrixes(glm::mat4 MVP, glm::mat4 MV, glm::mat4 NM, glm::mat4 MM, glm::mat4 depthMVP, float shininess, glm::mat4 colorMatrix = glm::mat4(1.0));
};

class ColorParticleShader : public Shader
{
private:
	GLuint mvp, mv, nm, cm, mm;
	glm::mat4 colorMatrix;

public:

	static std::shared_ptr<ColorParticleShader> loadShader(std::string fileName);

	void setMatrixes(glm::mat4 MVP, glm::mat4 MV, glm::mat4 NM, glm::mat4 MM, glm::mat4 colorMatrix = glm::mat4(1.0));
};

class TextureShader : public Shader
{
public:
	GLuint shininessLoc;
	int mvp, mv, nm, cm, mm, texLoc, dep_mvp;

	static std::shared_ptr<TextureShader> loadShader(std::string fileName);
	void setTexture(std::shared_ptr<Texture> texture);
	void setMatrixes(glm::mat4 MVP, glm::mat4 MV, glm::mat4 NM, glm::mat4 MM, glm::mat4 depthMVP, float shininess, glm::mat4 colorMatrix = glm::mat4(1.0));
};

class PortalShader : public Shader
{
public:
	int mvp, mv, cm, colLoc, depLoc, dep_mvp;

	static std::shared_ptr<PortalShader> loadShader(std::string fileName);
	void setTexture(std::shared_ptr<ScreenBufferRenderTexture> texture);
	void setMatrixes(glm::mat4 MVP, glm::mat4 MV, glm::mat4 depthMVP, glm::mat4 colorMatrix = glm::mat4(1.0));
};

class SkyBoxShader : public Shader
{
public:
	int pvm, cm, skyBoxLoc;

	static std::shared_ptr<SkyBoxShader> loadShader(std::string fileName);
	void setSkyBoxTexture(std::shared_ptr<SkyBoxTexture> skyBoxTexture);
	void setMatrixes(glm::mat4 PVM, glm::mat4 colorMatrix = glm::mat4(1.0));
};

class SkyTexShader : public Shader
{
public:
	int pvm, mvp, mvm, cm, skyBoxLoc;

	static std::shared_ptr<SkyTexShader> loadShader(std::string fileName);
	void setSkyBoxTexture(std::shared_ptr<SkyBoxTexture> skyBoxTexture);
	void setMatrixes(glm::mat4 MVP, glm::mat4 PVM, glm::mat4 MV, glm::mat4 colorMatrix = glm::mat4(1.0));
};

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
