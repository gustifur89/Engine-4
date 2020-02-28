#pragma once
#include "Headers.h"
//#include "Geometry.h"

class GBufferRenderTexture
{
public:
	GBufferRenderTexture();
	GBufferRenderTexture(int width, int height);

	//parameters
	GLuint frameBuffer;
	GLuint renderedTexture;
	GLuint depthrenderbuffer;

	GLuint colTex;
	GLuint posTex;
	GLuint normTex;
};


class RenderTexture2D
{
public:
	RenderTexture2D();
	RenderTexture2D(int width, int height);

	//parameters
	GLuint frameBuffer;
	GLuint renderedTexture;
	
	GLuint colTex;
};
