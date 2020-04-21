#include "Texture.h"

// ======================= Texture =====================

Texture::Texture()
{

}

Texture::~Texture()
{

}

std::shared_ptr<Texture> Texture::loadFromFile(std::string fileName, int blendMode, bool alpha)
{
	fileName = std::string("src/textures/") + fileName;
	int width, height, nrChannels;

    int STBI = (alpha) ? STBI_rgb_alpha : STBI_rgb;

	unsigned char *data = stbi_load(fileName.c_str(), &width, &height, &nrChannels, STBI);

	std::shared_ptr<Texture> texture(new Texture());
	
    bool mipmap = false;

    int magBlendMode = blendMode;

    if (blendMode == GL_LINEAR_MIPMAP_LINEAR)
    {
        magBlendMode = GL_LINEAR;
        mipmap = true;
    }
    if (blendMode == GL_LINEAR_MIPMAP_NEAREST)
    {
        magBlendMode = GL_LINEAR;
        mipmap = true;
    }
    if (blendMode == GL_NEAREST_MIPMAP_NEAREST)
    {
        mipmap = true;
        magBlendMode = GL_NEAREST;
    }
    if (blendMode == GL_NEAREST_MIPMAP_LINEAR)
    {
        magBlendMode = GL_NEAREST;
        mipmap = true;
    }


	// Create one OpenGL texture
	glGenTextures(1, &texture->textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, texture->textureID);

	// Give the image to OpenGL
   // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    if (alpha)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    else
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    if (mipmap)
        glGenerateMipmap(GL_TEXTURE_2D);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, blendMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, magBlendMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return texture;
	
}


// ======================= SkyBoxTexture =====================

std::shared_ptr<SkyBoxTexture> SkyBoxTexture::loadFromFile(std::vector<std::string> fileNames, int blendMode, bool alpha)
{

    for (int i = 0; i < fileNames.size(); i++)
    {
        fileNames[i] = std::string("src/textures/") + fileNames[i];
    }

	std::shared_ptr<SkyBoxTexture> skyBox(new SkyBoxTexture());

    int STBI = (alpha) ? STBI_rgb_alpha : STBI_rgb;

    bool mipmap = false;

    int magBlendMode = blendMode;

    if (blendMode == GL_LINEAR_MIPMAP_LINEAR)
    {
        magBlendMode = GL_LINEAR;
        mipmap = true;
    }   
    if (blendMode == GL_LINEAR_MIPMAP_NEAREST)
    {
        magBlendMode = GL_LINEAR;
        mipmap = true;
    }
    if (blendMode == GL_NEAREST_MIPMAP_NEAREST)
    {
        mipmap = true;
        magBlendMode = GL_NEAREST;
    }
    if (blendMode == GL_NEAREST_MIPMAP_LINEAR)
    {
        magBlendMode = GL_NEAREST;
        mipmap = true;
    }        

    glGenTextures(1, &skyBox->textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox->textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < fileNames.size(); i++)
    {
        unsigned char* data = stbi_load(fileNames[i].c_str(), &width, &height, &nrChannels, STBI);
        if (data)
        {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            if (alpha)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
                );

            }
            else
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                );

            }
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << fileNames[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, blendMode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magBlendMode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return skyBox;
}

