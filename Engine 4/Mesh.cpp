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
	std::vector<int> normalIndex;// (indexBuffer.size(), 0);

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
				//normalIndex[i] = j;
				normalIndex.push_back(j);
				normsList[j].push_back(norm);
			}
		}

		if (unique)
		{
			//need to add.
			nonDupedVerts.push_back(vert);
			normsList.push_back(std::vector<glm::vec3>(1, norm));

			//normalIndex[i] = normsList.size() - 1;// .push_back(normsList.size() - 1);
			normalIndex.push_back(normsList.size() - 1);
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
		int i_vert0 = indexBuffer[i + 0];
		int i_vert1 = indexBuffer[i + 1];
		int i_vert2 = indexBuffer[i + 2];


		int indx0 = normalIndex[i_vert0];
		int indx1 = normalIndex[i_vert1];
		int indx2 = normalIndex[i_vert2];


		//std::cout << (vertexBuffer.size()/3) << " : "  << i << " : " << normalIndex.size() << " : " << normalIndex[i_vert0] << " : " << avgNorms.size() << " \n";
		/*
		std::cout << i << " : " << indexBuffer.size() << "\n";
		std::cout << "0 " << i_vert0 << " : " << normalIndex.size() << " : " << indx0 << " : " << avgNorms.size() << "\n";
		std::cout << "1 " << i_vert1 << " : " << normalIndex.size() << " : " << indx1 << " : " << avgNorms.size() << "\n";
		std::cout << "2 " << i_vert2 << " : " << normalIndex.size() << " : " << indx2 << " : " << avgNorms.size() << "\n";
		*/

		glm::vec3 n0 = avgNorms[indx0];
		glm::vec3 n1 = avgNorms[indx1];
		glm::vec3 n2 = avgNorms[indx2];

		glm::vec3 vert0(vertexBuffer[3 * i_vert0 + 0], vertexBuffer[3 * i_vert0 + 1], vertexBuffer[3 * i_vert0 + 2]);
		glm::vec3 vert1(vertexBuffer[3 * i_vert1 + 0], vertexBuffer[3 * i_vert1 + 1], vertexBuffer[3 * i_vert1 + 2]);
		glm::vec3 vert2(vertexBuffer[3 * i_vert2 + 0], vertexBuffer[3 * i_vert2 + 1], vertexBuffer[3 * i_vert2 + 2]);

		//glm::vec3 vert0 = nonDupedVerts[i_vert0];
		//glm::vec3 vert1 = nonDupedVerts[i_vert1];
		//glm::vec3 vert2 = nonDupedVerts[i_vert2];


		std::shared_ptr<MeshTriangle> tri(new MeshTriangle(vert0, vert1, vert2, n0, n1, n2));

		tri->applyMatrixSelf(transform, glm::transpose(glm::inverse(transform)));

		faces.push_back(tri);
	}

	return faces;
}

std::vector<std::shared_ptr<Poly>> Mesh::getPolygonsFromMesh(glm::mat4 transform)
{
	std::vector<std::shared_ptr<Poly>> polygons;

	for (int i = 0; i < indexBuffer.size(); i += 3)
	{
		int i_vert0 = indexBuffer[i + 0];
		int i_vert1 = indexBuffer[i + 1];
		int i_vert2 = indexBuffer[i + 2];

		glm::vec3 vert0(vertexBuffer[3 * i_vert0 + 0], vertexBuffer[3 * i_vert0 + 1], vertexBuffer[3 * i_vert0 + 2]);
		glm::vec3 vert1(vertexBuffer[3 * i_vert1 + 0], vertexBuffer[3 * i_vert1 + 1], vertexBuffer[3 * i_vert1 + 2]);
		glm::vec3 vert2(vertexBuffer[3 * i_vert2 + 0], vertexBuffer[3 * i_vert2 + 1], vertexBuffer[3 * i_vert2 + 2]);

		std::vector<glm::vec3> pts = { vert0, vert1, vert2 };
		glm::vec3 norm = glm::normalize(glm::cross(vert1 - vert0, vert2 - vert0));
	//	std::cout << norm.x << " : " << norm.y << " : " << norm.z << "\n";
		glm::vec4 plane = glm::vec4(norm, -glm::dot(norm, pts[0]));

		//std::shared_ptr<Poly> poly(new Poly(pts, norm, plane));

		//poly->applyMatrixSelf(transform, glm::transpose(glm::inverse(transform)));

		//polygons.push_back(poly);
		//std::shared_ptr<Poly> poly(new Poly(pts, norm, plane));

	//	poly->applyMatrixSelf(transform, glm::transpose(glm::inverse(transform)));

	//	polygons.push_back(poly);
		
		//std::shared_ptr<Poly> poly(new Poly(pts, norm, plane));

		//std::cout << plane.x << ", " << plane.y << ", " << plane.z << ", " << plane.w << "\n";

		//poly->applyMatrixSelf(transform, glm::transpose(glm::inverse(transform)));

		//polygons.push_back(poly);

		if (true || !(std::isnan(plane.x) || !std::isfinite(plane.x)))
		{
			std::shared_ptr<Poly> poly(new Poly(pts, norm, plane));

			poly->applyMatrixSelf(transform, glm::transpose(glm::inverse(transform)));

			polygons.push_back(poly);
		}
	}

	return polygons;
	/*
	std::vector<std::shared_ptr<Poly>> polygons;

	std::cout << "get the polys\n";

	std::vector<glm::vec3> avgNorms; // for corners of shared pts.
	std::vector<std::vector<glm::vec3>> normsList; // for corners of shared pts.
	std::vector<glm::vec3> nonDupedVerts;
	std::vector<int> normalIndex;// (indexBuffer.size(), 0);

	for (int i = 0; i < vertexBuffer.size(); i += 3)
	{
		glm::vec3 vert(vertexBuffer[i + 0], vertexBuffer[i + 1], vertexBuffer[i + 2]);
		glm::vec3 norm(normalBuffer[i + 0], normalBuffer[i + 1], normalBuffer[i + 2]);
		std::cout << i << " : " << vertexBuffer.size() << " : verts\n";
		bool unique = true;

		for (int j = 0; j < nonDupedVerts.size(); j++)
		{
			if (vert == nonDupedVerts[j])
			{
				//this is the same...
				unique = false;
				//normalIndex[i] = j;
				normalIndex.push_back(j);
				normsList[j].push_back(norm);
			}
		}

		if (unique)
		{
			//need to add.
			nonDupedVerts.push_back(vert);
			normsList.push_back(std::vector<glm::vec3>(1, norm));

			//normalIndex[i] = normsList.size() - 1;// .push_back(normsList.size() - 1);
			normalIndex.push_back(normsList.size() - 1);
		}
	}

	avgNorms.resize(normsList.size());
	for (int i = 0; i < normsList.size(); i++)
	{
		glm::vec3 avg(0.0);
		std::cout << "norms\n";
		for (int j = 0; j < normsList[i].size(); j++)
		{
			avg += normsList[i][j];
		}
		avg = glm::normalize((1.0f / normsList[i].size()) * avg);
		avgNorms[i] = avg;
	}

	for (int i = 0; i < indexBuffer.size(); i += 3)
	{
		int i_vert0 = indexBuffer[i + 0];
		int i_vert1 = indexBuffer[i + 1];
		int i_vert2 = indexBuffer[i + 2];

		int indx0 = normalIndex[i_vert0];
		int indx1 = normalIndex[i_vert1];
		int indx2 = normalIndex[i_vert2];

		glm::vec3 vert0(vertexBuffer[3 * i_vert0 + 0], vertexBuffer[3 * i_vert0 + 1], vertexBuffer[3 * i_vert0 + 2]);
		glm::vec3 vert1(vertexBuffer[3 * i_vert1 + 0], vertexBuffer[3 * i_vert1 + 1], vertexBuffer[3 * i_vert1 + 2]);
		glm::vec3 vert2(vertexBuffer[3 * i_vert2 + 0], vertexBuffer[3 * i_vert2 + 1], vertexBuffer[3 * i_vert2 + 2]);

		std::vector<glm::vec3> pts = {vert0, vert1, vert2};
		glm::vec3 norm = glm::normalize(glm::cross(vert1 - vert0, vert2 - vert0));
		glm::vec4 plane = glm::vec4(norm, -glm::dot(norm, pts[0]));

		std::shared_ptr<Poly> poly(new Poly(pts, norm, plane));

		std::cout << "make\n";

		poly->applyMatrixSelf(transform, glm::transpose(glm::inverse(transform)));

		polygons.push_back(poly);
	}

	return polygons;
	*/
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

void Mesh::bindDynamicIndexVBO(std::vector<GLuint> indexes, GLuint* indexID)
{
	bindVAO();
	glGenBuffers(1, &elementbufferID);
	*indexID = elementbufferID;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_DYNAMIC_DRAW);
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

void Mesh::bindDynamicVertexAttribVBO(int attrib, int size, std::vector<GLfloat> data, GLuint* vboID)
{
	bindVAO();

	GLuint vertexbufferID;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbufferID);
	bufferIDs.push_back(vertexbufferID);
	*vboID = vertexbufferID;
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferID);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), &data[0], GL_DYNAMIC_DRAW);

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

void Mesh::updateVertexVBO(int attrib, int size, GLuint vertexbufferID, std::vector<GLfloat> data)
{
	bindVAO();

	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferID);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), &data[0], GL_DYNAMIC_DRAW);

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
}

void Mesh::updateIndexVBO(GLuint vertexbufferID, std::vector<GLuint> indexes)
{
	bindVAO();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_DYNAMIC_DRAW);
	indexCount = indexes.size();
	unbindVAO();
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

std::vector<std::string> Mesh::split(std::string str, std::string delim)
{
	std::vector<std::string> out;
	size_t pos = 0;
	std::string token;
	while ((pos = str.find(delim)) != std::string::npos) {
		token = str.substr(0, pos);
		//std::cout << token << std::endl;
		if(token.length() > 0)
			out.push_back(token);
		str.erase(0, pos + delim.length());
	}
	out.push_back(str);
	return out;
}

std::vector<float> Mesh::splitFloat(std::string str, std::string delim)
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

std::vector<int> Mesh::splitInt(std::string str, std::string delim)
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

void Mesh::parseOBJVert(std::string vert, int* vIndx, int* uvIndx, int* nIndx)
{
	std::vector<std::string> strs = Mesh::split(vert, "/");
	*vIndx = std::stoi(strs[0]);
	*uvIndx = std::stoi(strs[1]);
	*nIndx = std::stoi(strs[2]);
}

// ================================== ColorMesh =====================================

std::shared_ptr<ColorMesh> ColorMesh::loadFromFilePLY(std::string fileName, bool dynamic)
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
	bool uvChannel = false;

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
			if (type == "float")
			{
				file >> type;
				if (type == "s" || type == "t")
				{
					uvChannel = true;
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
				if (uvChannel)
				{
					float s,t;
					file >> s >> t;
				}
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

	if (dynamic)
	{
		//dynamic Verts/norm for animations
		mesh->bindDynamicIndexVBO(mesh->indexBuffer, &mesh->indexVBOId);

		mesh->bindDynamicVertexAttribVBO(0, 3, mesh->vertexBuffer, &mesh->vertVBOId);
		mesh->bindDynamicVertexAttribVBO(1, 3, mesh->normalBuffer, &mesh->normVBOId);
		mesh->bindDynamicVertexAttribVBO(2, 3, mesh->colorBuffer, &mesh->colorDataVBOId);
		mesh->colorDataSize = 3;

	}
	else
	{
		//static Verts/norms for non animations
		mesh->bindIndexVBO(mesh->indexBuffer);

		mesh->bindVertexAttribVBO(0, 3, mesh->vertexBuffer);
		mesh->bindVertexAttribVBO(1, 3, mesh->normalBuffer);
		mesh->bindVertexAttribVBO(2, 3, mesh->colorBuffer);
	}

	mesh->recalculateBounds();
	mesh->dynamic = dynamic;
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

std::shared_ptr<ColorMesh> ColorMesh::meshFromTriangles(std::vector<std::shared_ptr<MeshTriangle>> faces, int r, int g, int b, float thickness)
{
	std::shared_ptr<ColorMesh> mesh(new ColorMesh());
	float r_ = r / 255.f;
	float g_ = g / 255.f;
	float b_ = b / 255.f;

	for (int i = 0; i < faces.size(); i++)
	{
		/*
		std::cout << "face " << i << "\n";
		std::cout << faces[i]->vert0.x << " : " << faces[i]->vert0.y << " : " << faces[i]->vert0.z << "\n";
		std::cout << faces[i]->vert1.x << " : " << faces[i]->vert1.y << " : " << faces[i]->vert1.z << "\n";
		std::cout << faces[i]->vert2.x << " : " << faces[i]->vert2.y << " : " << faces[i]->vert2.z << "\n";
		std::cout << faces[i]->normal.x << " : " << faces[i]->normal.y << " : " << faces[i]->normal.z << "\n";
		std::cout << faces[i]->n0.x << " : " << faces[i]->n0.y << " : " << faces[i]->n0.z << "\n";
		std::cout << faces[i]->n1.x << " : " << faces[i]->n1.y << " : " << faces[i]->n1.z << "\n";
		std::cout << faces[i]->n2.x << " : " << faces[i]->n2.y << " : " << faces[i]->n2.z << "\n";

		mesh->vertexBuffer.push_back(faces[i]->vert0.x);
		mesh->vertexBuffer.push_back(faces[i]->vert0.y);
		mesh->vertexBuffer.push_back(faces[i]->vert0.z);
		mesh->normalBuffer.push_back(faces[i]->normal.x);
		mesh->normalBuffer.push_back(faces[i]->normal.y);
		mesh->normalBuffer.push_back(faces[i]->normal.z);
		mesh->colorBuffer.push_back(r_);
		mesh->colorBuffer.push_back(g_);
		mesh->colorBuffer.push_back(b_);
		mesh->indexBuffer.push_back(3 * i + 0);

		mesh->vertexBuffer.push_back(faces[i]->vert1.x);
		mesh->vertexBuffer.push_back(faces[i]->vert1.y);
		mesh->vertexBuffer.push_back(faces[i]->vert1.z);
		mesh->normalBuffer.push_back(faces[i]->normal.x);
		mesh->normalBuffer.push_back(faces[i]->normal.y);
		mesh->normalBuffer.push_back(faces[i]->normal.z);
		mesh->colorBuffer.push_back(r_);
		mesh->colorBuffer.push_back(g_);
		mesh->colorBuffer.push_back(b_);
		mesh->indexBuffer.push_back(3 * i + 1);

		mesh->vertexBuffer.push_back(faces[i]->vert2.x);
		mesh->vertexBuffer.push_back(faces[i]->vert2.y);
		mesh->vertexBuffer.push_back(faces[i]->vert2.z);
		mesh->normalBuffer.push_back(faces[i]->normal.x);
		mesh->normalBuffer.push_back(faces[i]->normal.y);
		mesh->normalBuffer.push_back(faces[i]->normal.z);
		mesh->colorBuffer.push_back(r_);
		mesh->colorBuffer.push_back(g_);
		mesh->colorBuffer.push_back(b_);
		mesh->indexBuffer.push_back(3 * i + 2); 
		*/

		///*
		MeshTriangle temp = faces[i]->getThicker(thickness);

		/*
		std::cout << "face " << i << "\n";
		std::cout << temp.vert0.x << " : " << temp.vert0.y << " : " << temp.vert0.z << "\n";
		std::cout << temp.vert1.x << " : " << temp.vert1.y << " : " << temp.vert1.z << "\n";
		std::cout << temp.vert2.x << " : " << temp.vert2.y << " : " << temp.vert2.z << "\n";
		std::cout << temp.normal.x << " : " << temp.normal.y << " : " << temp.normal.z << "\n";
		std::cout << temp.n0.x << " : " << temp.n0.y << " : " << temp.n0.z << "\n";
		std::cout << temp.n1.x << " : " << temp.n1.y << " : " << temp.n1.z << "\n";
		std::cout << temp.n2.x << " : " << temp.n2.y << " : " << temp.n2.z << "\n";
		*/
		mesh->vertexBuffer.push_back(temp.vert0.x);
		mesh->vertexBuffer.push_back(temp.vert0.y);
		mesh->vertexBuffer.push_back(temp.vert0.z);
		mesh->normalBuffer.push_back(temp.normal.x);
		mesh->normalBuffer.push_back(temp.normal.y);
		mesh->normalBuffer.push_back(temp.normal.z);
		mesh->colorBuffer.push_back(r_);
		mesh->colorBuffer.push_back(g_);
		mesh->colorBuffer.push_back(b_);
		mesh->indexBuffer.push_back(3 * i + 0);

		mesh->vertexBuffer.push_back(temp.vert1.x);
		mesh->vertexBuffer.push_back(temp.vert1.y);
		mesh->vertexBuffer.push_back(temp.vert1.z);
		mesh->normalBuffer.push_back(temp.normal.x);
		mesh->normalBuffer.push_back(temp.normal.y);
		mesh->normalBuffer.push_back(temp.normal.z);
		mesh->colorBuffer.push_back(r_);
		mesh->colorBuffer.push_back(g_);
		mesh->colorBuffer.push_back(b_);
		mesh->indexBuffer.push_back(3 * i + 1);

		mesh->vertexBuffer.push_back(temp.vert2.x);
		mesh->vertexBuffer.push_back(temp.vert2.y);
		mesh->vertexBuffer.push_back(temp.vert2.z);
		mesh->normalBuffer.push_back(temp.normal.x);
		mesh->normalBuffer.push_back(temp.normal.y);
		mesh->normalBuffer.push_back(temp.normal.z);
		mesh->colorBuffer.push_back(r_);
		mesh->colorBuffer.push_back(g_);
		mesh->colorBuffer.push_back(b_);
		mesh->indexBuffer.push_back(3 * i + 2);
		//*/
	}
	mesh->bindArrays();
	mesh->recalculateBounds();	
	return mesh;
}

std::shared_ptr<ColorMesh> ColorMesh::meshFromPolygons(std::vector<std::shared_ptr<Poly>> polygons, int r, int g, int b, float thickness)
{
	std::shared_ptr<ColorMesh> mesh(new ColorMesh());
	float r_ = r / 255.f;
	float g_ = g / 255.f;
	float b_ = b / 255.f;

	for (int i = 0; i < polygons.size(); i++)
	{
		//MeshTriangle temp = polygons[i]->getThicker(thickness);
		std::shared_ptr<Poly> poly = polygons[i];

		r_ = (rand() % 155 + 100) / 255.f;
		g_ = (rand() % 155 + 100) / 255.f;
		b_ = (rand() % 155 + 100) / 255.f;

		//vert 0
		mesh->vertexBuffer.push_back(poly->pts[0].x);
		mesh->vertexBuffer.push_back(poly->pts[0].y);
		mesh->vertexBuffer.push_back(poly->pts[0].z);
		mesh->normalBuffer.push_back(poly->norm.x);
		mesh->normalBuffer.push_back(poly->norm.y);
		mesh->normalBuffer.push_back(poly->norm.z);
		mesh->colorBuffer.push_back(r_);
		mesh->colorBuffer.push_back(g_);
		mesh->colorBuffer.push_back(b_);
		mesh->indexBuffer.push_back(3 * i + 0);

		//vert 1
		mesh->vertexBuffer.push_back(poly->pts[1].x);
		mesh->vertexBuffer.push_back(poly->pts[1].y);
		mesh->vertexBuffer.push_back(poly->pts[1].z);
		mesh->normalBuffer.push_back(poly->norm.x);
		mesh->normalBuffer.push_back(poly->norm.y);
		mesh->normalBuffer.push_back(poly->norm.z);
		mesh->colorBuffer.push_back(r_);
		mesh->colorBuffer.push_back(g_);
		mesh->colorBuffer.push_back(b_);
		mesh->indexBuffer.push_back(3 * i + 1);

		//vert 2
		mesh->vertexBuffer.push_back(poly->pts[2].x);
		mesh->vertexBuffer.push_back(poly->pts[2].y);
		mesh->vertexBuffer.push_back(poly->pts[2].z);
		mesh->normalBuffer.push_back(poly->norm.x);
		mesh->normalBuffer.push_back(poly->norm.y);
		mesh->normalBuffer.push_back(poly->norm.z);
		mesh->colorBuffer.push_back(r_);
		mesh->colorBuffer.push_back(g_);
		mesh->colorBuffer.push_back(b_);
		mesh->indexBuffer.push_back(3 * i + 2);
	}
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

// ================================== TextureMesh =====================================

void TextureMesh::recalculateBoundsSub()
{
	glm::vec3 mins = subMeshes[0]->bounds.low;
	glm::vec3 maxs = subMeshes[0]->bounds.high;
	for (int i = 1; i < subMeshes.size(); i++)
	{
		mins = glm::min(mins, subMeshes[i]->bounds.low);
		maxs = glm::max(maxs, subMeshes[i]->bounds.high);
	}
	this->bounds = Bounds(mins, maxs);
}

std::shared_ptr<TextureMesh> TextureMesh::meshFromData(std::vector<GLfloat> vertexBuffer, std::vector<GLfloat> normalBuffer, std::vector<GLfloat> uvBuffer, std::vector<GLuint> indexBuffer)
{
	std::shared_ptr<TextureMesh> mesh(new TextureMesh());
	mesh->vertexBuffer = vertexBuffer;
	mesh->normalBuffer = normalBuffer;
	mesh->uvBuffer = uvBuffer;
	mesh->indexBuffer = indexBuffer;
	mesh->vertexCount = mesh->vertexBuffer.size() / 3;

	mesh->bindIndexVBO(mesh->indexBuffer);

	mesh->bindVertexAttribVBO(0, 3, mesh->vertexBuffer);
	mesh->bindVertexAttribVBO(1, 3, mesh->normalBuffer);
	mesh->bindVertexAttribVBO(2, 2, mesh->uvBuffer);
	mesh->recalculateBounds();
	return mesh;
}

std::shared_ptr<TextureMesh> TextureMesh::loadFromFilePLY(std::string fileName, bool dynamic)
{

	fileName = std::string("src/meshes/") + fileName + std::string(".ply");
	std::ifstream file;
	file = std::ifstream(fileName);

	if (file.fail())
	{
		fprintf(stderr, "failed to load file at : ");
		fprintf(stderr, fileName.c_str());
		fprintf(stderr, "\n");
		return std::shared_ptr<TextureMesh>(NULL);
	}
	std::shared_ptr<TextureMesh> mesh(new TextureMesh());

	std::string line;

	int vertex_number = 0;
	int face_number = 0;
	int count_v = 0;
	int count_f = 0;
	bool vertexes = false;
	bool faces = false;
	bool alphaChannel = false;
	bool rgbChannel = false;

	while (true)
	{
		//assumes you get 9 in.
		file >> line;
		if (file.eof())
			return std::shared_ptr<TextureMesh>(NULL);

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
				if (type == "red" || type == "green" || type == "blue")
				{
					rgbChannel = true;
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
			return std::shared_ptr<TextureMesh>(NULL);;

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

				//read vertex
				float x, y, z, nx, ny, nz;
				float u, v;// r, g, b;
				file >> x >> z >> y >> nx >> nz >> ny;	//y and z switch
				file >> u >> v;
				if (rgbChannel)
				{
					int r, g, b;
					file >> r >> g >> b;
				}
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

				mesh->uvBuffer.push_back(u);
				mesh->uvBuffer.push_back(1.0 - v);
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

	if (dynamic)
	{
		//dynamic Verts/norm for animations
		mesh->bindDynamicIndexVBO(mesh->indexBuffer, &mesh->indexVBOId);

		mesh->bindDynamicVertexAttribVBO(0, 3, mesh->vertexBuffer, &mesh->vertVBOId);
		mesh->bindDynamicVertexAttribVBO(1, 3, mesh->normalBuffer, &mesh->normVBOId);
		mesh->bindDynamicVertexAttribVBO(2, 2, mesh->uvBuffer, &mesh->colorDataVBOId);
		mesh->colorDataSize = 2;
	}
	else
	{
		//static Verts/norms for non animations
		
		mesh->bindIndexVBO(mesh->indexBuffer);
		
		mesh->bindVertexAttribVBO(0, 3, mesh->vertexBuffer);
		mesh->bindVertexAttribVBO(1, 3, mesh->normalBuffer);
		mesh->bindVertexAttribVBO(2, 2, mesh->uvBuffer);
		//	mesh = TextureMesh::meshFromData(mesh->vertexBuffer, mesh->normalBuffer, mesh->uvBuffer, mesh->indexBuffer);
	}

	mesh->recalculateBounds();
	mesh->dynamic = dynamic;
	return mesh;
}

std::shared_ptr<TextureMesh> TextureMesh::loadFromFileOBJ(std::string fileName)
{
	fileName = std::string("src/meshes/") + fileName + std::string(".obj");
	std::ifstream file;
	file = std::ifstream(fileName);

	if (file.fail())
	{
		fprintf(stderr, "failed to load file at : ");
		fprintf(stderr, fileName.c_str());
		fprintf(stderr, "\n");
		return std::shared_ptr<TextureMesh>(NULL);
	}
	std::shared_ptr<TextureMesh> mesh(new TextureMesh());
	mesh->multiMesh = false;
	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> norms;
	std::vector<glm::vec2> uvs;

	std::vector<GLfloat> vertexBufferTemp;
	std::vector<GLfloat> normalBufferTemp;
	std::vector<GLfloat> uvBufferTemp;
	std::vector<GLuint> indexBufferTemp;


	std::string line;

	while (!file.eof())
	{
		file >> line;
		if (line == "v")
		{
			//vertex
			float x, y, z; 
			file >> x >> y >> z; // swap x, z
			verts.push_back(glm::vec3(-x,y,-z));
			//verts.push_back(-x);
			//verts.push_back(y);
			//verts.push_back(z);
		}
		else if (line == "vn")
		{
			//normal
			float nx, ny, nz;
			file >> nx >> ny >> nz; // swap x, z
			norms.push_back(glm::vec3(-nx, ny, -nz));
			//norms.push_back(-nx);
			//norms.push_back(ny);
			//norms.push_back(nz);
		}
		else if (line == "vt")
		{
			//uv
			float u, v;
			file >> u >> v;
			uvs.push_back(glm::vec2(u, 1.0 - v));
			//uvs.push_back(u);
			//uvs.push_back(1.0 - v);
		}
		else if (line == "f")
		{
			//face
			//extract the entire line. To find out if we are using quad or tri...
			std::getline(file, line);
			std::vector<std::string> vindx = Mesh::split(line, " ");

			if (vindx.size() == 3)
			{
				//need to construct the buffers....
				//v1
				int vi, uvi, ni;
				Mesh::parseOBJVert(vindx[0], &vi, &uvi, &ni);
				glm::vec3 v = verts[vi - 1];
				glm::vec3 n = norms[ni - 1];
				glm::vec2 uv = uvs[uvi - 1];
				mesh->vertexBuffer.push_back(v.x);
				mesh->vertexBuffer.push_back(v.y);
				mesh->vertexBuffer.push_back(v.z);
				mesh->normalBuffer.push_back(n.x);
				mesh->normalBuffer.push_back(n.y);
				mesh->normalBuffer.push_back(n.z);
				mesh->uvBuffer.push_back(uv.x);
				mesh->uvBuffer.push_back(uv.y);
				mesh->indexBuffer.push_back(mesh->indexBuffer.size());//just make it sequtially...
				//v2
				Mesh::parseOBJVert(vindx[1], &vi, &uvi, &ni);
				v = verts[vi - 1];
				n = norms[ni - 1];
				uv = uvs[uvi - 1];
				mesh->vertexBuffer.push_back(v.x);
				mesh->vertexBuffer.push_back(v.y);
				mesh->vertexBuffer.push_back(v.z);
				mesh->normalBuffer.push_back(n.x);
				mesh->normalBuffer.push_back(n.y);
				mesh->normalBuffer.push_back(n.z);
				mesh->uvBuffer.push_back(uv.x);
				mesh->uvBuffer.push_back(uv.y);
				mesh->indexBuffer.push_back(mesh->indexBuffer.size());
				//v3
				Mesh::parseOBJVert(vindx[2], &vi, &uvi, &ni);
				v = verts[vi - 1];
				n = norms[ni - 1];
				uv = uvs[uvi - 1];
				mesh->vertexBuffer.push_back(v.x);
				mesh->vertexBuffer.push_back(v.y);
				mesh->vertexBuffer.push_back(v.z);
				mesh->normalBuffer.push_back(n.x);
				mesh->normalBuffer.push_back(n.y);
				mesh->normalBuffer.push_back(n.z);
				mesh->uvBuffer.push_back(uv.x);
				mesh->uvBuffer.push_back(uv.y);
				mesh->indexBuffer.push_back(mesh->indexBuffer.size());
			}
			else if (vindx.size() == 4)
			{
				//v1
				int vi, uvi, ni;
				Mesh::parseOBJVert(vindx[0], &vi, &uvi, &ni);
				glm::vec3 v = verts[vi - 1];
				glm::vec3 n = norms[ni - 1];
				glm::vec2 uv = uvs[uvi - 1];
				mesh->vertexBuffer.push_back(v.x);
				mesh->vertexBuffer.push_back(v.y);
				mesh->vertexBuffer.push_back(v.z);
				mesh->normalBuffer.push_back(n.x);
				mesh->normalBuffer.push_back(n.y);
				mesh->normalBuffer.push_back(n.z);
				mesh->uvBuffer.push_back(uv.x);
				mesh->uvBuffer.push_back(uv.y);
				mesh->indexBuffer.push_back(mesh->indexBuffer.size());//just make it sequtially...
				//v2
				Mesh::parseOBJVert(vindx[1], &vi, &uvi, &ni);
				v = verts[vi - 1];
				n = norms[ni - 1];
				uv = uvs[uvi - 1];
				mesh->vertexBuffer.push_back(v.x);
				mesh->vertexBuffer.push_back(v.y);
				mesh->vertexBuffer.push_back(v.z);
				mesh->normalBuffer.push_back(n.x);
				mesh->normalBuffer.push_back(n.y);
				mesh->normalBuffer.push_back(n.z);
				mesh->uvBuffer.push_back(uv.x);
				mesh->uvBuffer.push_back(uv.y);
				mesh->indexBuffer.push_back(mesh->indexBuffer.size());
				//v3
				Mesh::parseOBJVert(vindx[2], &vi, &uvi, &ni);
				v = verts[vi - 1];
				n = norms[ni - 1];
				uv = uvs[uvi - 1];
				mesh->vertexBuffer.push_back(v.x);
				mesh->vertexBuffer.push_back(v.y);
				mesh->vertexBuffer.push_back(v.z);
				mesh->normalBuffer.push_back(n.x);
				mesh->normalBuffer.push_back(n.y);
				mesh->normalBuffer.push_back(n.z);
				mesh->uvBuffer.push_back(uv.x);
				mesh->uvBuffer.push_back(uv.y);
				mesh->indexBuffer.push_back(mesh->indexBuffer.size());

				//second tri
				//v1
				Mesh::parseOBJVert(vindx[0], &vi, &uvi, &ni);
				v = verts[vi - 1];
				n = norms[ni - 1];
				uv = uvs[uvi - 1];
				mesh->vertexBuffer.push_back(v.x);
				mesh->vertexBuffer.push_back(v.y);
				mesh->vertexBuffer.push_back(v.z);
				mesh->normalBuffer.push_back(n.x);
				mesh->normalBuffer.push_back(n.y);
				mesh->normalBuffer.push_back(n.z);
				mesh->uvBuffer.push_back(uv.x);
				mesh->uvBuffer.push_back(uv.y);
				mesh->indexBuffer.push_back(mesh->indexBuffer.size());
				//v3
				Mesh::parseOBJVert(vindx[2], &vi, &uvi, &ni);
				v = verts[vi - 1];
				n = norms[ni - 1];
				uv = uvs[uvi - 1];
				mesh->vertexBuffer.push_back(v.x);
				mesh->vertexBuffer.push_back(v.y);
				mesh->vertexBuffer.push_back(v.z);
				mesh->normalBuffer.push_back(n.x);
				mesh->normalBuffer.push_back(n.y);
				mesh->normalBuffer.push_back(n.z);
				mesh->uvBuffer.push_back(uv.x);
				mesh->uvBuffer.push_back(uv.y);
				mesh->indexBuffer.push_back(mesh->indexBuffer.size());
				//v4
				Mesh::parseOBJVert(vindx[3], &vi, &uvi, &ni);
				v = verts[vi - 1];
				n = norms[ni - 1];
				uv = uvs[uvi - 1];
				mesh->vertexBuffer.push_back(v.x);
				mesh->vertexBuffer.push_back(v.y);
				mesh->vertexBuffer.push_back(v.z);
				mesh->normalBuffer.push_back(n.x);
				mesh->normalBuffer.push_back(n.y);
				mesh->normalBuffer.push_back(n.z);
				mesh->uvBuffer.push_back(uv.x);
				mesh->uvBuffer.push_back(uv.y);
				mesh->indexBuffer.push_back(mesh->indexBuffer.size());
			}
		}
		else if (line == "usemtl")
		{
			//texture...
			file >> line;
			mesh->materialNames.push_back(line);

			if (mesh->materialNames.size() > 1)
			{
				//this is a multi
				mesh->multiMesh = true;
				mesh->subMeshes.push_back(TextureMesh::meshFromData(mesh->vertexBuffer, mesh->normalBuffer, mesh->uvBuffer, mesh->indexBuffer));
				//reset all the buffers for the next submesh
				vertexBufferTemp.insert(std::end(vertexBufferTemp), std::begin(mesh->vertexBuffer), std::end(mesh->vertexBuffer));
				normalBufferTemp.insert(std::end(normalBufferTemp), std::begin(mesh->normalBuffer), std::end(mesh->normalBuffer));
				uvBufferTemp.insert(std::end(uvBufferTemp), std::begin(mesh->uvBuffer), std::end(mesh->uvBuffer));
				indexBufferTemp.insert(std::end(indexBufferTemp), std::begin(mesh->indexBuffer), std::end(mesh->indexBuffer));
				
				mesh->vertexBuffer.resize(0);
				mesh->normalBuffer.resize(0);
				mesh->uvBuffer.resize(0);
				mesh->indexBuffer.resize(0);
			}
		}
	}
	file.close();

	if (mesh->multiMesh)
	{
		//make the last one...
		mesh->subMeshes.push_back(TextureMesh::meshFromData(mesh->vertexBuffer, mesh->normalBuffer, mesh->uvBuffer, mesh->indexBuffer));
	
		//need to calculate the bounds....
		mesh->recalculateBoundsSub();		
		mesh->vertexBuffer = vertexBufferTemp;
		mesh->normalBuffer = normalBufferTemp;
		mesh->uvBuffer = uvBufferTemp;
		mesh->indexBuffer = indexBufferTemp;
	}
	else
	{
		//non multi... like usual
		mesh->vertexCount = mesh->vertexBuffer.size() / 3;

		mesh->bindIndexVBO(mesh->indexBuffer);

		mesh->bindVertexAttribVBO(0, 3, mesh->vertexBuffer);
		mesh->bindVertexAttribVBO(1, 3, mesh->normalBuffer);
		mesh->bindVertexAttribVBO(2, 2, mesh->uvBuffer);
		mesh->recalculateBounds();
	}
	return mesh;
}

std::shared_ptr<TextureMesh> TextureMesh::loadFromFileDAE(std::string fileName)
{
	fileName = std::string("src/meshes/") + fileName + std::string(".dae");
	std::ifstream file;
	file = std::ifstream(fileName);

	if (file.fail())
	{
		fprintf(stderr, "failed to load file at : ");
		fprintf(stderr, fileName.c_str());
		fprintf(stderr, "\n");
		return std::shared_ptr<TextureMesh>(NULL);
	}
	std::shared_ptr<TextureMesh> mesh(new TextureMesh());
	mesh->multiMesh = true;

	rapidxml::xml_document<> doc;
	// Read the xml file into a vector
	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc 
	doc.parse<0>(&buffer[0]);

	//std::cout << doc.name << "\n";

	rapidxml::xml_node<>* root_node = doc.first_node("COLLADA");

	rapidxml::xml_node<>* geometry_node = root_node->first_node("library_geometries")->first_node("geometry");
	//std::cout << "geometry id = " << geometry_node->first_attribute("id")->name() << " value = " << geometry_node->first_attribute("id")->value() << "\n";
	std::string meshName = geometry_node->first_attribute("id")->value();
	rapidxml::xml_node<>* mesh_node = geometry_node->first_node("mesh");
	//std::map<std::string, std::vector<float>> data;
	std::map<std::string, int> offsets;
	std::map<std::string, std::vector<int>> materials;

	std::vector<glm::vec3> positionBuffer;
	std::vector<glm::vec3> normalBuffer;
	std::vector<std::vector<glm::vec2>> uvsBuffer;

	//read from file
	for (rapidxml::xml_node<>* source = mesh_node->first_node(); source; source = source->next_sibling())
	{
		if (!strcmp(source->name(), "source"))
		{
			std::string id = source->first_attribute("id")->value();
			id = id.substr(meshName.length() + 1, id.length() + 1);
			rapidxml::xml_node<>* float_array = source->first_node("float_array");
			std::vector<float> arrayData = splitFloat(float_array->value(), " ");

			if (id == "positions")
			{
				//y and z switch, -x
				for (int i = 0; i < arrayData.size(); i += 3)
				{
					positionBuffer.push_back(glm::vec3(-arrayData[i + 0], arrayData[i + 2], arrayData[i + 1]));
				}
			}
			else if (id == "normals")
			{
				//y and z switch, -x
				for (int i = 0; i < arrayData.size(); i += 3)
				{
					normalBuffer.push_back(glm::vec3(-arrayData[i + 0], arrayData[i + 2], arrayData[i + 1]));
				}
			}
			else
			{
				//maps... i.e. uv
				uvsBuffer.push_back(std::vector<glm::vec2>());
				for (int i = 0; i < arrayData.size(); i += 2)
				{
					uvsBuffer[uvsBuffer.size()-1].push_back(glm::vec2(arrayData[i + 0], arrayData[i + 1]));
				}

			}
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

			mesh->materialNames.push_back(material);

			for (rapidxml::xml_node<>* triangleData = source->first_node(); triangleData; triangleData = triangleData->next_sibling())
			{
				if (!strcmp(triangleData->name(), "input"))
				{
					std::string name = triangleData->first_attribute("source")->value();
					name = name.substr(meshName.length() + 1, name.length() + 1);
					if (name == "vertices")
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

	//get mesh from data
	for (auto const& material : materials)
	{
		mesh->materialNames.push_back(material.first);
		std::vector<int> vertIndx = material.second;

		std::vector<float> vBuf;
		std::vector<float> nBuf;
		std::vector<float> uvBuf;
		std::vector<unsigned int> iBuf;

		for (int i = 0; i < vertIndx.size(); i += 3)
		{
			int vi = vertIndx[i + 0];
			float vx = positionBuffer[vi].x;
			float vy = positionBuffer[vi].y;
			float vz = positionBuffer[vi].z;

			int ni = vertIndx[i + 1];
			float nx = normalBuffer[ni].x;
			float ny = normalBuffer[ni].y;
			float nz = normalBuffer[ni].z;

			int uvi = vertIndx[i + 2];
			float u = uvsBuffer[0][uvi].x;
			float v = uvsBuffer[0][uvi].y;

			vBuf.push_back(vx);
			vBuf.push_back(vy);
			vBuf.push_back(vz);
			nBuf.push_back(nx);
			nBuf.push_back(ny);
			nBuf.push_back(nz);
			uvBuf.push_back(u);
			uvBuf.push_back(v);
			iBuf.push_back(iBuf.size());
		}

		mesh->subMeshes.push_back(TextureMesh::meshFromData(vBuf, nBuf, uvBuf, iBuf));
		mesh->recalculateBoundsSub();
	}
	return mesh;
}

// ================================= GenereicMesh =====================

std::shared_ptr<GenericMesh> GenericMesh::loadFromFilePLY(std::string fileName, bool dynamic, bool fullFIlePath)
{
	if(!fullFIlePath)
		fileName = std::string("src/meshes/") + fileName + std::string(".ply");
	
	std::ifstream file;
	file = std::ifstream(fileName);

	if (file.fail())
	{
		fprintf(stderr, "failed to load file at : ");
		fprintf(stderr, fileName.c_str());
		fprintf(stderr, "\n");
		return std::shared_ptr<GenericMesh>(NULL);
	}
	std::shared_ptr<GenericMesh> mesh(new GenericMesh());

	std::string line;

	int vertex_number = 0;
	int face_number = 0;
	int count_v = 0;
	int count_f = 0;
	bool vertexes = false;
	bool faces = false;

	bool alphaChannel = false;
	bool rgbChannel = false;
	bool uv = false;

	if (uv)
	{
		mesh->type = TYPE::TEXTURE;
	}
	else if (rgbChannel)
	{
		mesh->type = TYPE::COLOR;
	}
	else
	{
		mesh->type = TYPE::NO_COLOR;
	}

	while (true)
	{
		//assumes you get 9 in.
		file >> line;
		if (file.eof())
			return std::shared_ptr<GenericMesh>(NULL);

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
				if (type == "red" || type == "green" || type == "blue")
				{
					rgbChannel = true;
				}
			}
			if (type == "float")
			{
				file >> type;
				if (type == "s" || type == "t")
				{
					uv = true;
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
			return std::shared_ptr<GenericMesh>(NULL);

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

				//read vertex
				float x, y, z, nx, ny, nz;
				file >> x >> z >> y >> nx >> nz >> ny;	//y and z switch

				if (uv)
				{
					float u, v;
					file >> u >> v;
					mesh->colorDataBuffer.push_back(u);
					mesh->colorDataBuffer.push_back(1.0 - v);
				}
				if (rgbChannel)
				{
					int r, g, b;
					file >> r >> g >> b;
					if (!uv)
					{
						//if uv is active, then we don't want to render color... probably.
						mesh->colorDataBuffer.push_back(r / 255.0);
						mesh->colorDataBuffer.push_back(g / 255.0);
						mesh->colorDataBuffer.push_back(b / 255.0);
					}
				}
				
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

	if (dynamic)
	{
		//dynamic Verts/norm for animations
		mesh->bindDynamicIndexVBO(mesh->indexBuffer, &mesh->indexVBOId);

		mesh->bindDynamicVertexAttribVBO(0, 3, mesh->vertexBuffer, &mesh->vertVBOId);
		mesh->bindDynamicVertexAttribVBO(1, 3, mesh->normalBuffer, &mesh->normVBOId);
		if (uv)
		{
			mesh->bindDynamicVertexAttribVBO(2, 2, mesh->colorDataBuffer, &mesh->colorDataVBOId);
			mesh->colorDataSize = 2;
		}
		else if (rgbChannel)
		{
			mesh->bindDynamicVertexAttribVBO(2, 3, mesh->colorDataBuffer, &mesh->colorDataVBOId);
			mesh->colorDataSize = 3;
		}
	}
	else
	{
		//static Verts/norms for non animations
		mesh->bindIndexVBO(mesh->indexBuffer);

		mesh->bindVertexAttribVBO(0, 3, mesh->vertexBuffer);
		mesh->bindVertexAttribVBO(1, 3, mesh->normalBuffer);
		if (uv)
		{
			mesh->bindVertexAttribVBO(2, 2, mesh->colorDataBuffer);
			mesh->colorDataSize = 2;
		}
		else if (rgbChannel)
		{
			mesh->bindVertexAttribVBO(2, 2, mesh->colorDataBuffer);
			mesh->colorDataSize = 3;
		}
	}

	mesh->recalculateBounds();
	mesh->dynamic = dynamic;
	return mesh;
}

// ============================ AnimationData ====================

std::shared_ptr<AnimationData> AnimationData::loadFromFilePLY(std::string fileName, int numFrames)
{

	std::shared_ptr<AnimationData> animation = std::shared_ptr<AnimationData>(new AnimationData());
	animation->numFrames = numFrames;
	animation->frames.resize(numFrames);
	
	std::string filePath = std::string("src/meshes/") + fileName + std::string("/") + fileName;
	std::string line;

	for (int i = 1; i <= numFrames; i++)
	{
		fileName = filePath + std::to_string(i) + std::string(".ply");
		animation->frames[i - 1] = GenericMesh::loadFromFilePLY(fileName, false, true);
	}
	

	return animation;
}
