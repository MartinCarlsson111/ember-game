#include "texture.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "GL/glew.h"

void Texture2DHandler::LoadTexture(const char* fileName, Texture2D& texture)
{
	int width, height, comp;
	unsigned char* imageData;

	imageData = stbi_load(fileName, &width, &height, &comp, STBI_rgb_alpha);

	if (imageData == nullptr)
	{
		std::cout << "Could not load file" << std::endl;
		return;
	}
	int widthInBytes = width * 4;
	unsigned char* top = NULL;
	unsigned char* bottom = NULL;
	unsigned char temp = 0;
	int halfHeight = height / 2;
	for (int row = 0; row < halfHeight; row++)
	{
		top = imageData + (size_t)row * widthInBytes;
		bottom = imageData + ((size_t)height - row - 1) * widthInBytes;
		for (int col = 0; col < widthInBytes; col++)
		{
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}

	glGenTextures(1, &texture.textureHandle);
	glBindTexture(GL_TEXTURE_2D, texture.textureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

	//glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(imageData);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2DHandler::LoadTexture(char* pixels, int width, int height, Texture2D& texture)
{
	glGenTextures(1, &texture.textureHandle);
	glBindTexture(GL_TEXTURE_2D, texture.textureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pixels);

	glGenerateMipmap(GL_TEXTURE_2D);

}

void Texture2DHandler::LoadTextureNoFlip(const char* fileName, Texture2D& texture)
{
	int comp;
	unsigned char* imageData;
	imageData = stbi_load(fileName, &texture.pixelSizeX, &texture.pixelSizeY, &comp, STBI_rgb_alpha);

	if (imageData == nullptr)
	{
		std::cout << "Could not load file" << std::endl;
	}

	glGenTextures(1, &texture.textureHandle);
	glBindTexture(GL_TEXTURE_2D, texture.textureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.pixelSizeX, texture.pixelSizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

	stbi_image_free(imageData);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2DHandler::FreeTexture(Texture2D& texture)
{
	glDeleteTextures(1, &texture.textureHandle);
}
