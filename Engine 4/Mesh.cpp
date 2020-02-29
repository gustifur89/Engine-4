#include "Mesh.h"

// ================================== Mesh =====================================

Mesh::Mesh()
{
	//THIS IS SO YOU CAN MAKE MESHES OUTSIDE OF CONTEXT
	hasBound = false;
	if (glfwGetCurrentContext())
	{
		//active context.
		hasBound = true;
		setUpVAO();
	}
	
	//	vaos.push_back(VertexArrayID);
	indexCount = 0;
	numVBOs = 0;
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteBuffers(1, &elementbufferID);
	glDeleteBuffers(bufferIDs.size(), &bufferIDs[0]);
}

void Mesh::setUpVAO()
{
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
}

std::vector<std::shared_ptr<MeshTriangle>> Mesh::getTrianglesFromMesh(glm::mat4 transform)
{
	std::vector<std::shared_ptr<MeshTriangle>> faces;

	std::vector<glm::vec3> avgNorms; // for corners of shared pts.
	std::vector<std::vector<glm::vec3>> normsList; // for corners of shared pts.
	std::vector<glm::vec3> nonDupedVerts;
	std::vector<int> normalIndex(indexBuffer.size(), 0);

	for (int i = 0; i < vertexBuffer.size(); i+=3)
	{
		glm::vec3 vert(vertexBuffer[i + 0], vertexBuffer[i + 1], vertexBuffer[i + 2]);
		glm::vec3 norm(normalBuffer[i + 0], normalBuffer[i + 1], normalBuffer[i + 2]);

		bool unique = true;

		for (int j = 0; j < nonDupedVerts.size(); j++)
		{
			if (vert == nonDupedVerts[j])
			{
				//this is the same...
				unique = false;
				normalIndex[i] = j;
				normsList[j].push_back(norm);
			}
		}

		if (unique)
		{
			//need to add.
			nonDupedVerts.push_back(vert);
			normsList.push_back(std::vector<glm::vec3>(1, norm));
			normalIndex[i] = normsList.size() - 1;// .push_back(normsList.size() - 1);
		}
	}

	avgNorms.resize(normsList.size());
	for (int i = 0; i < normsList.size(); i++)
	{
		glm::vec3 avg(0.0);
		for (int j = 0; j < normsList[i].size(); j++)
		{
			avg += normsList[i][j];
		}
		avg = glm::normalize((1.0f / normsList[i].size()) * avg);
		avgNorms[i] = avg;
	}

	for (int i = 0; i < indexBuffer.size(); i += 3)
	{
		int i_p0 = 3 * indexBuffer[i + 0];
		int i_vert0 = 3 * indexBuffer[i + 1];
		int i_vert1 = 3 * indexBuffer[i + 2];

		std::cout << i << " : " << normalIndex.size() << " : " << normalIndex[i + 0] << " : " << avgNorms.size() << " \n";


		glm::vec3 n0 = avgNorms[normalIndex[i + 0]];
		glm::vec3 n1 = avgNorms[normalIndex[i + 1]];
		glm::vec3 n2 = avgNorms[normalIndex[i + 2]];

		glm::vec3 p0(vertexBuffer[i_p0 + 0], vertexBuffer[i_p0 + 1], vertexBuffer[i_p0 + 2]);
		glm::vec3 vert0(vertexBuffer[i_vert0 + 0], vertexBuffer[i_vert0 + 1], vertexBuffer[i_vert0 + 2]);
		glm::vec3 vert1(vertexBuffer[i_vert1 + 0], vertexBuffer[i_vert1 + 1], vertexBuffer[i_vert1 + 2]);


		std::shared_ptr<MeshTriangle> tri(new MeshTriangle(p0,vert0,vert1,n0,n1,n2));

		tri->applyMatrixSelf(transform, glm::transpose(glm::inverse(transform)));

		faces.push_back(tri);
	}

	return faces;
}

void Mesh::bindVAO()
{
	glBindVertexArray(VertexArrayID);
}

void Mesh::unbindVAO()
{
	glBindVertexArray(0);
}

void Mesh::recalculateBounds()
{
	glm::vec3 low(0,0,0);
	glm::vec3 high(0,0,0);

	bool first = true;

	for (int i = 0; i < vertexBuffer.size(); i += 3)
	{
		if (first)
		{
			first = false;
			low.x = vertexBuffer[i + 0];
			high.x = vertexBuffer[i + 0];
			low.y = vertexBuffer[i + 1];
			high.y = vertexBuffer[i + 1];
			low.z = vertexBuffer[i + 2];
			high.z = vertexBuffer[i + 2];
		}

		float x = vertexBuffer[i + 0];
		float y = vertexBuffer[i + 1];
		float z = vertexBuffer[i + 2];

		if (x < low.x) low.x = x;
		if (x > high.x) high.x = x;
		if (y < low.y) low.y = y;
		if (y > high.y) high.y = y;
		if (z < low.z) low.z = z;
		if (z > high.z) high.z = z;
	}

	this->bounds = Bounds(low, high);
}

void Mesh::bindArrays()
{
//	glGenVertexArrays(1, &VertexArrayID);
//	glBindVertexArray(VertexArrayID);
}

void Mesh::bindIndexVBO(std::vector<GLuint> indexes)
{
	bindVAO();
	glGenBuffers(1, &elementbufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_STATIC_DRAW);
	indexCount = indexes.size();
	unbindVAO();
}

void Mesh::bindVertexAttribVBO(int attrib, int size, std::vector<GLfloat> data)
{
	bindVAO();
	
	GLuint vertexbufferID;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbufferID);
	bufferIDs.push_back(vertexbufferID);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferID);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), &data[0], GL_STATIC_DRAW);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(attrib);
	glVertexAttribPointer(
		attrib,             // attribute
		size,               // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glDisableVertexAttribArray(attrib);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	unbindVAO();

	bufferAttribs.push_back(attrib);
	numVBOs++;
//	vbos.push_back(vertexbufferID);
}

void Mesh::render()
{
	glBindVertexArray(VertexArrayID);
	for (int i = 0; i < bufferAttribs.size(); i++)
		glEnableVertexAttribArray(bufferAttribs[i]);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	for (int i = 0; i < bufferAttribs.size(); i++)
		glDisableVertexAttribArray(bufferAttribs[i]);
	glBindVertexArray(0);
}

// ================================== ColorMesh =====================================

std::shared_ptr<ColorMesh> ColorMesh::loadFromFile(std::string fileName)
{

	fileName = std::string("src/meshes/") + fileName + std::string(".ply");
	std::ifstream file;
	file = std::ifstream(fileName);

	if (file.fail())
	{
		fprintf(stderr, "failed to load file at : ");
		fprintf(stderr, fileName.c_str());
		fprintf(stderr, "\n");
		return std::shared_ptr<ColorMesh>(NULL);
	}
	std::shared_ptr<ColorMesh> mesh(new ColorMesh());

	std::string line;

	int vertex_number = 0;
	int face_number = 0;
	int count_v = 0;
	int count_f = 0;
	bool vertexes = false;
	bool faces = false;
	bool alphaChannel = false;

	while (true)
	{
		//assumes you get 9 in.
		file >> line;
		if (file.eof())
			return std::shared_ptr<ColorMesh>(NULL);
		
		if (line == "element")
		{
			std::string type;
			file >> type;
			if (type == "vertex")
			{
				file >> vertex_number;
			}
			if (type == "face")
			{
				file >> face_number;
			}
		}
		else if (line == "property")
		{
			std::string type;
			file >> type;
			if (type == "uchar")
			{
				file >> type;
				if (type == "alpha")
				{
					alphaChannel = true;
				}
			}
		}
		else if (line == "end_header")
		{
			vertexes = true;
			break;
		}

	}

	while (true)
	{
		if (file.eof())
			return std::shared_ptr<ColorMesh>(NULL);;

		if (vertexes)
		{
			if (count_v == vertex_number)
			{
				//exit
				vertexes = false;
				faces = true;
			}
			else
			{
				//read vertex;
				float x, y, z, nx, ny, nz;
				int r, g, b;
				file >> x >> z >> y >> nx >> nz >> ny;	//y and z switch
				file >> r >> g >> b;

				if (alphaChannel)
				{
					int a;
					file >> a;
				}

				mesh->vertexBuffer.push_back(-x);
				mesh->vertexBuffer.push_back(y);
				mesh->vertexBuffer.push_back(z);

				mesh->normalBuffer.push_back(-nx);
				mesh->normalBuffer.push_back(ny);
				mesh->normalBuffer.push_back(nz);

				mesh->colorBuffer.push_back(r / 255.0);
				mesh->colorBuffer.push_back(g / 255.0);
				mesh->colorBuffer.push_back(b / 255.0);

				count_v++;
			}
		}
		else if (faces)
		{
			if (count_f == face_number)
			{
				//exit
				break;
			}
			else
			{
				//read vertex;

				int num_v;			//whether it is triangles or quads
				int v1, v2, v3, v4;
				file >> num_v;
				if (num_v == 3)
				{
					file >> v1 >> v2 >> v3;
					mesh->indexBuffer.push_back(v1);
					mesh->indexBuffer.push_back(v2);
					mesh->indexBuffer.push_back(v3);
				}
				else if (num_v == 4)
				{
					file >> v1 >> v2 >> v3 >> v4;
					mesh->indexBuffer.push_back(v1);
					mesh->indexBuffer.push_back(v2);
					mesh->indexBuffer.push_back(v3);

					mesh->indexBuffer.push_back(v1);
					mesh->indexBuffer.push_back(v3);
					mesh->indexBuffer.push_back(v4);
				}

				count_f++;
			}
		}
	}
	file.close();

	mesh->vertexCount = mesh->vertexBuffer.size() / 3;

	mesh->bindIndexVBO(mesh->indexBuffer);

	mesh->bindVertexAttribVBO(0, 3, mesh->vertexBuffer);
	mesh->bindVertexAttribVBO(1, 3, mesh->normalBuffer);
	mesh->bindVertexAttribVBO(2, 3, mesh->colorBuffer);
	mesh->recalculateBounds();
	return mesh;
}

void ColorMesh::recolorMesh(std::shared_ptr<ColorMesh> mesh, int r, int g, int b)
{
	float r_ = r / 255.f;
	float g_ = g / 255.f;
	float b_ = b / 255.f;

	mesh->colorBuffer.resize(0);

	for (int i = 0; i < mesh->colorBuffer.size(); i += 3)
	{
		mesh->colorBuffer.push_back(r_);
		mesh->colorBuffer.push_back(g_);
		mesh->colorBuffer.push_back(b_);
	}
	mesh->bindVertexAttribVBO(2, 3, mesh->colorBuffer);
}

void ColorMesh::bindArrays()  
{
	this->bindIndexVBO(this->indexBuffer);
	this->bindVertexAttribVBO(0, 3, this->vertexBuffer);
	this->bindVertexAttribVBO(1, 3, this->normalBuffer);
	this->bindVertexAttribVBO(2, 3, this->colorBuffer);
}

std::shared_ptr<ColorMesh> ColorMesh::meshFromTriangles(std::vector<std::shared_ptr<MeshTriangle>>& faces, int r, int g, int b, float thickness)
{
	std::shared_ptr<ColorMesh> mesh(new ColorMesh());
	float r_ = r / 255.f;
	float g_ = g / 255.f;
	float b_ = b / 255.f;

	for (int i = 0; i < faces.size(); i++)
	{
		//faces[i]->thicken(thickness);

		mesh->vertexBuffer.push_back(faces[i]->vert0.x);
		mesh->vertexBuffer.push_back(faces[i]->vert0.y);
		mesh->vertexBuffer.push_back(faces[i]->vert0.z);
		mesh->normalBuffer.push_back(faces[i]->n0.x);
		mesh->normalBuffer.push_back(faces[i]->n0.y);
		mesh->normalBuffer.push_back(faces[i]->n0.z);
		mesh->colorBuffer.push_back(r_);
		mesh->colorBuffer.push_back(g_);
		mesh->colorBuffer.push_back(b_);
		mesh->indexBuffer.push_back(3 * i + 0);

		mesh->vertexBuffer.push_back(faces[i]->vert1.x);
		mesh->vertexBuffer.push_back(faces[i]->vert1.y);
		mesh->vertexBuffer.push_back(faces[i]->vert1.z);
		mesh->normalBuffer.push_back(faces[i]->n1.x);
		mesh->normalBuffer.push_back(faces[i]->n1.y);
		mesh->normalBuffer.push_back(faces[i]->n1.z);
		mesh->colorBuffer.push_back(r_);
		mesh->colorBuffer.push_back(g_);
		mesh->colorBuffer.push_back(b_);
		mesh->indexBuffer.push_back(3 * i + 1);

		mesh->vertexBuffer.push_back(faces[i]->vert2.x);
		mesh->vertexBuffer.push_back(faces[i]->vert2.y);
		mesh->vertexBuffer.push_back(faces[i]->vert2.z);
		mesh->normalBuffer.push_back(faces[i]->n2.x);
		mesh->normalBuffer.push_back(faces[i]->n2.y);
		mesh->normalBuffer.push_back(faces[i]->n2.z);
		mesh->colorBuffer.push_back(r_);
		mesh->colorBuffer.push_back(g_);
		mesh->colorBuffer.push_back(b_);
		mesh->indexBuffer.push_back(3 * i + 2);
	}
	mesh->recalculateBounds();
	mesh->bindArrays();
	mesh->recalculateBounds();
	return mesh;
}

std::shared_ptr<ColorMesh> ColorMesh::triangle()
{
	std::shared_ptr<ColorMesh> mesh(new ColorMesh());
	GLfloat vertexData[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};
	GLfloat normalData[] = {
		 0.0f, 0.0f, 1.0f,
		 0.0f, 0.0f, 1.0f,
		 0.0f, 0.0f, 1.0f,
	};
	GLfloat colorData[] = {
		 1.0f, 0.0f, 1.0f,
		 0.0f, 1.0f, 1.0f,
		 1.0f, 0.0f, 0.0f,
	};

	GLuint indexData[] = {
		 0, 1, 2
	};

	for (int i = 0; i < 9; i++)
	{
		mesh->vertexBuffer.push_back(vertexData[i]);
		mesh->normalBuffer.push_back(normalData[i]);
		mesh->colorBuffer.push_back(colorData[i]);
	}
	
	for (int i = 0; i < 3; i++)
	{
		mesh->indexBuffer.push_back(indexData[i]);
	}

	mesh->bindIndexVBO(mesh->indexBuffer);

	mesh->bindVertexAttribVBO(0, 3, mesh->vertexBuffer);
	mesh->bindVertexAttribVBO(1, 3, mesh->normalBuffer);
	mesh->bindVertexAttribVBO(2, 3, mesh->colorBuffer);
	mesh->recalculateBounds();
	return mesh;
}
