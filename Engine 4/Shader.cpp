#include "Shader.h"

// ========================================== Shader ================================

Shader::Shader()
{
}

std::shared_ptr<Shader> Shader::loadShader(std::string vertexFile, std::string fragmentShader, std::string geometryShader)
{
	std::shared_ptr<Shader> out(new Shader());
	out->loadShader_(vertexFile, fragmentShader, geometryShader);
	return out;
}

std::shared_ptr<Shader> Shader::loadShader(std::string vertexFile, std::string fragmentFile)
{
	std::shared_ptr<Shader> out(new Shader());
	out->loadShader_(vertexFile, fragmentFile);
	return out;
}

std::shared_ptr<Shader> Shader::loadShaderPreprocess(std::string vertexFile, std::string fragmentFile, std::string preproccessorStuff)
{
	std::shared_ptr<Shader> out(new Shader());
	out->loadShaderPreprocess_(vertexFile, fragmentFile, preproccessorStuff);
	return out;
}

std::shared_ptr<Shader> Shader::loadShader(std::string fileName)
{
	std::shared_ptr<Shader> out(new Shader());
	out->loadShader_(fileName, fileName);
	return out;
}

void Shader::loadShader_(std::string vertexFile, std::string fragmentFile)
{
	std::string vertex_file_path = std::string("src/shaders/") + vertexFile + std::string(".vs");
	std::string fragment_file_path = std::string("src/shaders/") + fragmentFile + std::string(".fs");
	
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("shader : %s\n", vertex_file_path.c_str());
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	//printf("Compiling shader : %s\n", fragment_file_path.c_str());
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
		printf("shader : %s\n", fragment_file_path.c_str());
	}

	// Link the program
	//printf("Linking program\n");
	programID = glCreateProgram();
	glAttachShader(programID, VertexShaderID);
	glAttachShader(programID, FragmentShaderID);
	glLinkProgram(programID);

	// Check the program
	glGetProgramiv(programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("Linking program %s, %s\n", vertexFile, fragmentFile);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(programID, VertexShaderID);
	glDetachShader(programID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
}

void Shader::loadShader_(std::string vertexFile, std::string fragmentFile, std::string geometryFile)
{
	std::string vertex_file_path = std::string("src/shaders/") + vertexFile + std::string(".vs");
	std::string fragment_file_path = std::string("src/shaders/") + fragmentFile + std::string(".fs");
	std::string geometry_file_path = std::string("src/shaders/") + geometryFile + std::string(".gs");

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string GeometryShaderCode;
	std::ifstream GeometryShaderStream(geometry_file_path, std::ios::in);
	if (GeometryShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << GeometryShaderStream.rdbuf();
		GeometryShaderCode = sstr.str();
		GeometryShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path.c_str());
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	//printf("Compiling shader : %s\n", fragment_file_path.c_str());
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("shader : %s\n", fragment_file_path.c_str());
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Compile Geometry Shader
	//printf("Compiling shader : %s\n", geometry_file_path.c_str());
	char const * GeometrySourcePointer = GeometryShaderCode.c_str();
	glShaderSource(GeometryShaderID, 1, &GeometrySourcePointer, NULL);
	glCompileShader(GeometryShaderID);

	// Check Geometry Shader
	glGetShaderiv(GeometryShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(GeometryShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> GeometryShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(GeometryShaderID, InfoLogLength, NULL, &GeometryShaderErrorMessage[0]);
		printf("shader : %s\n", geometry_file_path.c_str());
		printf("%s\n", &GeometryShaderErrorMessage[0]);
	}

	// Link the program
	programID = glCreateProgram();
	glAttachShader(programID, VertexShaderID);
	glAttachShader(programID, FragmentShaderID);
	glAttachShader(programID, GeometryShaderID);
	glLinkProgram(programID);

	// Check the program
	glGetProgramiv(programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("Linking program %s, %s\n", vertexFile, fragmentFile);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(programID, VertexShaderID);
	glDetachShader(programID, FragmentShaderID);
	glDetachShader(programID, GeometryShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	glDeleteShader(GeometryShaderID);
}

void Shader::loadShaderPreprocess_(std::string vertexFile, std::string fragmentFile, std::string preproccessorStuff)
{
	std::string vertex_file_path = std::string("src/shaders/") + vertexFile + std::string(".vs");
	std::string fragment_file_path = std::string("src/shaders/") + fragmentFile + std::string(".fs");

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;
	
	// Compile Vertex Shader
	//printf("Compiling shader : %s\n", vertex_file_path.c_str());
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("shader : %s\n", vertex_file_path.c_str());
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	
	int versionIndex = FragmentShaderCode.find("#version");
	int defineInsertPoint = FragmentShaderCode.find("\n", versionIndex) + 1;

	for (int i = preproccessorStuff.size() - 1; i >= 0; --i)
		FragmentShaderCode.insert(defineInsertPoint, preproccessorStuff);

	// Compile Fragment Shader
	//printf("Compiling shader : %s\n", fragment_file_path.c_str());
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("shader : %s\n", fragment_file_path.c_str());
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	programID = glCreateProgram();
	glAttachShader(programID, VertexShaderID);
	glAttachShader(programID, FragmentShaderID);
	glLinkProgram(programID);

	// Check the program
	glGetProgramiv(programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("Linking program\n");
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(programID, VertexShaderID);
	glDetachShader(programID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
}

void Shader::useShader()
{
	glUseProgram(programID);
}

GLint Shader::getUniformLocation(std::string uniformName)
{
	return glGetUniformLocation(programID, uniformName.c_str());
}

void Shader::loadFloat(int location, float value)
{
	glUniform1f(location, value);
}

void Shader::loadInt(int location, int value)
{
	glUniform1i(location, value);
}

void Shader::loadVector(int location, glm::vec3 vector)
{
	glUniform3f(location, vector.x, vector.y, vector.z);
}

void Shader::loadBoolean(int location, bool value)
{
	float toLoad = 0;
	if (value) toLoad = 1;
	glUniform1f(location, toLoad);
}

void Shader::loadMatrix(int location, glm::mat4 matrix)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
}

void Shader::loadMatrix(int location, glm::mat3 matrix)
{
	glUniformMatrix3fv(location, 1, GL_FALSE, &matrix[0][0]);
}

void Shader::loadTexture(int location, int textureID)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glUniform1i(location, 0);
}

void Shader::loadCubeMap(int location, int textureID)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glUniform1i(location, 0);
}

Shader::~Shader()
{
}

// ========================================== ColorShader ================================

std::shared_ptr<ColorShader> ColorShader::loadShader(std::string fileName)
{
	//Shader::loadShader(fileName);
	std::shared_ptr<ColorShader> out(new ColorShader());
	out->Shader::loadShader_(fileName, fileName);
	out->mvp = out->getUniformLocation("MVP");
	out->mv = out->getUniformLocation("MV");
	out->cm = out->getUniformLocation("ColorMatrix");
	out->dep_mvp = out->getUniformLocation("DepthMVP");
	out->mm = out->getUniformLocation("MM");
	out->useShader();
	out->loadMatrix(out->cm, glm::mat4(1.0));
	out->nm = out->getUniformLocation("NM");
	out->shininessLoc = out->getUniformLocation("specular");
	return out;
}

void ColorShader::setMatrixes(glm::mat4 MVP, glm::mat4 MV, glm::mat4 NM, glm::mat4 MM, glm::mat4 depthMVP, float shininess, glm::mat4 colorMatrix)
{
	loadMatrix(mvp, MVP);
	loadMatrix(mv, MV);
	loadMatrix(cm, colorMatrix);
	loadMatrix(mm, MM);
	loadMatrix(dep_mvp, depthMVP);
	loadMatrix(nm, NM);
	loadFloat(shininessLoc, shininess);
}

// ========================================== TextureShader ================================

std::shared_ptr<TextureShader> TextureShader::loadShader(std::string fileName)
{
	std::shared_ptr<TextureShader> out(new TextureShader());
	out->Shader::loadShader_(fileName, fileName);
	out->mvp = out->getUniformLocation("MVP");
	out->nm = out->getUniformLocation("NM");
	out->mv = out->getUniformLocation("MV");
	out->mm = out->getUniformLocation("MM");
	out->dep_mvp = out->getUniformLocation("DepthMVP");
	out->cm = out->getUniformLocation("ColorMatrix");
	out->useShader();
	out->loadMatrix(out->cm, glm::mat4(1.0));
	out->texLoc = out->getUniformLocation("tex");
	out->shininessLoc = out->getUniformLocation("specular");
	return out;
}

void TextureShader::setTexture(std::shared_ptr<Texture> texture)
{
	loadTexture(texLoc, texture->textureID);
}

void TextureShader::setMatrixes(glm::mat4 MVP, glm::mat4 MV, glm::mat4 NM, glm::mat4 MM, glm::mat4 depthMVP, float shininess, glm::mat4 colorMatrix)
{
	loadMatrix(mvp, MVP);
	loadMatrix(mv, MV);
	loadMatrix(nm, NM);
	loadMatrix(mm, MM);
	loadMatrix(cm, colorMatrix);
	loadMatrix(dep_mvp, depthMVP);
	loadFloat(shininessLoc, shininess);
}

// =============== SkyBoxShader ============================

std::shared_ptr<SkyBoxShader> SkyBoxShader::loadShader(std::string fileName)
{
	std::shared_ptr<SkyBoxShader> out(new SkyBoxShader());
	out->Shader::loadShader_(fileName, fileName);
	out->pvm = out->getUniformLocation("PVM");
	out->cm = out->getUniformLocation("ColorMatrix");
	out->useShader();
	out->loadMatrix(out->cm, glm::mat4(1.0));
	out->skyBoxLoc = out->getUniformLocation("skybox");
	return out;
}

void SkyBoxShader::setSkyBoxTexture(std::shared_ptr<SkyBoxTexture> skyBoxTexture)
{
	loadCubeMap(skyBoxLoc, skyBoxTexture->textureID);
}

void SkyBoxShader::setMatrixes(glm::mat4 PVM, glm::mat4 colorMatrix)
{
	loadMatrix(pvm, PVM);
	loadMatrix(cm, colorMatrix);
}

// =============== SkyTexShader ============================

std::shared_ptr<SkyTexShader> SkyTexShader::loadShader(std::string fileName)
{
	std::shared_ptr<SkyTexShader> out(new SkyTexShader());
	out->Shader::loadShader_(fileName, fileName);
	out->pvm = out->getUniformLocation("PVM");
	out->mvp = out->getUniformLocation("MVP");
	out->mvm = out->getUniformLocation("MV");
	out->cm = out->getUniformLocation("ColorMatrix");
	out->useShader();
	out->loadMatrix(out->cm, glm::mat4(1.0));
	out->skyBoxLoc = out->getUniformLocation("skybox");
	return out;
}

void SkyTexShader::setSkyBoxTexture(std::shared_ptr<SkyBoxTexture> skyBoxTexture)
{
	loadCubeMap(skyBoxLoc, skyBoxTexture->textureID);
}

void SkyTexShader::setMatrixes(glm::mat4 MVP, glm::mat4 PVM, glm::mat4 MV, glm::mat4 colorMatrix)
{
	loadMatrix(mvp, MVP);
	loadMatrix(pvm, PVM);
	loadMatrix(mvm, MV);
	loadMatrix(cm, colorMatrix);
}

// ========================================== PortalShader ================================

std::shared_ptr<PortalShader> PortalShader::loadShader(std::string fileName)
{
	std::shared_ptr<PortalShader> out(new PortalShader());
	out->Shader::loadShader_(fileName, fileName);
	out->mvp = out->getUniformLocation("MVP");
	out->mv = out->getUniformLocation("MV");
	out->cm = out->getUniformLocation("ColorMatrix");
	out->useShader();
	out->loadMatrix(out->cm, glm::mat4(1.0));
	out->colLoc = out->getUniformLocation("colTex");
	out->depLoc = out->getUniformLocation("depthTex");
	out->dep_mvp = out->getUniformLocation("DepthMVP");
	out->useShader();
	glUniform1i(out->colLoc, 0);
	glUniform1i(out->depLoc, 1);
	return out;
}

void PortalShader::setTexture(std::shared_ptr<ScreenBufferRenderTexture> texture)
{
	glBindTextureUnit(0, texture->colTex);
	glBindTextureUnit(1, texture->depthTex);
}

void PortalShader::setMatrixes(glm::mat4 MVP, glm::mat4 MV, glm::mat4 depthMVP, glm::mat4 colorMatrix)
{
	loadMatrix(dep_mvp, depthMVP);
	loadMatrix(mvp, MVP);
	loadMatrix(mv, MV);
	loadMatrix(cm, colorMatrix);
}

// ========================================= WindowShader ==============================

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

// ==================================== ColorParticleShader ===========================

std::shared_ptr<ColorParticleShader> ColorParticleShader::loadShader(std::string fileName)
{
	std::shared_ptr<ColorParticleShader> out(new ColorParticleShader());
	out->Shader::loadShader_(fileName, fileName);
	out->mvp = out->getUniformLocation("MVP");
	out->mv = out->getUniformLocation("MV");
	out->cm = out->getUniformLocation("ColorMatrix");
	out->mm = out->getUniformLocation("MM");
	out->nm = out->getUniformLocation("NM");
	out->useShader();
	out->loadMatrix(out->cm, glm::mat4(1.0));
	return out;
}

void ColorParticleShader::setMatrixes(glm::mat4 MVP, glm::mat4 MV, glm::mat4 NM, glm::mat4 MM, glm::mat4 colorMatrix)
{
	loadMatrix(mvp, MVP);
	loadMatrix(mv, MV);
	loadMatrix(cm, colorMatrix);
	loadMatrix(mm, MM);
	loadMatrix(nm, NM);
}
