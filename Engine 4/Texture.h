#pragma once
#include "Headers.h"
#include "stb_image.h"
class Texture
{
private:
	// Data read from the header of the BMP file
	std::vector<unsigned char> header; // Each BMP file begins by a 54-bytes header
	int dataPos;     // Position in the file where the actual data begins
	int width, height;
	int imageSize;   // = width*height*3
	bool alpha;
	unsigned char* data;
public:
	Texture();
	~Texture();

	static std::shared_ptr<Texture> loadFromFile(std::string fileName, int blendMode, bool alpha);

	glm::vec3 getColorFromPixel(glm::vec2 texel);

	GLuint textureID;
};

class SkyBoxTexture : public Texture
{
private:
	// Data read from the header of the BMP file
	std::vector<unsigned char> header; // Each BMP file begins by a 54-bytes header
	int dataPos;     // Position in the file where the actual data begins
	int width, height;
	int imageSize;   // = width*height*3
	//unsigned char * data;	// Actual RGB data
	std::vector<std::uint8_t> data;
public:

	//right, left, top, bottom, front, back
	static std::shared_ptr<SkyBoxTexture> loadFromFile(std::vector<std::string> fileNames, int blendMode, bool alpha);

	GLuint textureID;
};
