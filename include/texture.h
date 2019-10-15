#pragma once
struct Texture2D
{
	unsigned int textureHandle;
};


class Texture2DHandler
{
public:
	void LoadTexture(const char* fileName, Texture2D& texture);
	void LoadTexture(char* pixels, int width, int height, Texture2D& texture);
	void LoadTextureNoFlip(const char* fileName, Texture2D& texture);
	void FreeTexture(Texture2D& texture);
};

