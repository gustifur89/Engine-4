#pragma once
#include "Headers.h"
#include "TempGeometry.h"
#include "Geometry.h"

class Mesh
{
protected:

	void bindIndexVBO(std::vector<GLuint> indexes);
	void bindDynamicIndexVBO(std::vector<GLuint> indexes, GLuint* indexID);
	void bindVertexAttribVBO(int attrib, int size, std::vector<GLfloat> data);
	void bindDynamicVertexAttribVBO(int attrib, int size, std::vector<GLfloat> data, GLuint * vboID);

	void bindVAO();
	void unbindVAO();
	
	std::vector<GLuint> bufferAttribs;
	bool hasBound;

public:
	Mesh();
	~Mesh();

	void render();
	void recalculateBounds();
	void setUpVAO();

	std::vector<std::shared_ptr<MeshTriangle>> getTrianglesFromMesh(glm::mat4 transform);

	void updateVertexVBO(int attrib, int size, GLuint vertexbufferID, std::vector<GLfloat> data);
	void updateIndexVBO(GLuint vertexbufferID, std::vector<GLuint> indexes);

	GLuint VertexArrayID;
	GLuint elementbufferID;
	std::vector<GLuint> indexBuffer;
	std::vector<GLfloat> vertexBuffer;
	std::vector<GLfloat> normalBuffer;
	std::vector<GLuint> bufferIDs;
	Bounds bounds;
	int indexCount;
	int vertexCount;
	int numVBOs;
	GLuint vertVBOId;
	GLuint normVBOId;
	GLuint indexVBOId;
	GLuint colorDataVBOId;
	int colorDataSize;
	int vertAttrib;
	bool dynamic;
	virtual void bindArrays();
};

class ColorMesh : public Mesh
{
public:
	GLuint ColorBufferID;
	std::vector<GLfloat> colorBuffer;
	
	static void recolorMesh(std::shared_ptr<ColorMesh> mesh, int r, int g, int b);
	
	void bindArrays();

	static std::shared_ptr<ColorMesh> meshFromTriangles(std::vector<std::shared_ptr<MeshTriangle>> triangleList, int r, int g, int b, float thickness = 0.0);
	static std::shared_ptr<ColorMesh> triangle();
	static std::shared_ptr<ColorMesh> loadFromFilePLY(std::string fileName, bool dynamic = false);
	static std::shared_ptr<ColorMesh> applyMatrixToMesh(std::shared_ptr<ColorMesh> & mesh, glm::mat4 matrix);
};

class TextureMesh : public Mesh
{
public:
	GLuint uvBufferID;
	std::vector<GLfloat> uvBuffer;

	static std::shared_ptr<TextureMesh> loadFromFilePLY(std::string fileName, bool dynamic = false);
	static std::shared_ptr<TextureMesh> loadFromFileOBJ(std::string fileName);
};

class GenericMesh : public Mesh
{
public:
	enum TYPE
	{
		COLOR,
		TEXTURE,
		NO_COLOR
	};

	TYPE type;
	std::vector<GLfloat> colorDataBuffer;

	static std::shared_ptr<GenericMesh> loadFromFilePLY(std::string fileName, bool dynamic = false);
};

class AnimationData
{
public:
	std::vector<std::vector<GLfloat>> vertexBuffer;
	std::vector<std::vector<GLfloat>> normalBuffer;
	std::vector<std::vector<GLuint>> indexBuffer;
	std::vector<std::vector<GLfloat>> colorDataBuffer; // color or uv
	int numFrames;

	static std::shared_ptr<AnimationData> loadFromFilePLY(std::string fileName, int numFrames);
};

