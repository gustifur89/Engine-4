#pragma once
#include "Headers.h"
#include "Geometry.h"

class Mesh
{
protected:

	void bindIndexVBO(std::vector<GLuint> indexes);
	void bindVertexAttribVBO(int attrib, int size, std::vector<GLfloat> data);

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
	virtual void bindArrays();
};

class ColorMesh : public Mesh
{
public:
	GLuint ColorBufferID;
	std::vector<GLfloat> colorBuffer;
	
	static void recolorMesh(std::shared_ptr<ColorMesh> mesh, int r, int g, int b);
	
	void bindArrays();

	static std::shared_ptr<ColorMesh> triangle();
	static std::shared_ptr<ColorMesh> loadFromFile(std::string fileName);
	static std::shared_ptr<ColorMesh> applyMatrixToMesh(std::shared_ptr<ColorMesh> & mesh, glm::mat4 matrix);
};
