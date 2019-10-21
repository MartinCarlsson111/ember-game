#include "planetGen.h"
#include "stb_image.h"
#include "stb_image_write.h"

void PlanetGen::CreateTris()
{
	triangles.clear();
	int nTriangles = 20;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, 1024);
	std::uniform_int_distribution<int> disExtent(-50, 50);


	for (int i = 0; i < nTriangles; i++)
	{
		int midPoint = dis(gen);
		int midPoint2 = dis(gen);

		int extent1 = disExtent(gen);
		int extent2 = disExtent(gen);
		int extent3 = disExtent(gen);
		triangles.push_back(glm::ivec4(midPoint, midPoint2, extent2, extent3));
	}

}
void PlanetGen::RaisePointsInTris()
{
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, 15);

	for (int i = 0; i < triangles.size(); i++)
	{
		for (int x = 0; x < triangles[i].z; x++)
		{
			for (int y = 0; y < triangles[i].w; y++)
			{
				int xPos = triangles[i].x + x;
				int yPos = triangles[i].y + y;

				if (xPos >= reso)
				{
					xPos = xPos - (reso);
				}

				if (yPos >= reso)
				{
					yPos = yPos - (reso);
				}

				if (yPos < 0)
				{
					yPos = yPos + reso;
				}
				if (xPos < 0)
				{
					xPos = xPos + reso;
				}

				data[(size_t)xPos + ((size_t)yPos * reso)].r += dis(gen);
				//data[(size_t)xPos + ((size_t)yPos * reso)].g += dis(gen);
				//data[(size_t)xPos + ((size_t)yPos * reso)].b += dis(gen);
			}
		}
	}
}


void PlanetGen::FractureAlgo()
{
	data = std::vector<col>(1024 * 1024);

	for (int i = 0; i < iterations; i++)
	{
		CreateTris();
		RaisePointsInTris();
	}


	for (int i = 0; i < reso * reso; i++)
	{
	//	data[i].a = 255;
	}


	stbi_write_tga("surfaceTexture.tga", reso, reso, 4, data.data());

	int sizeX, sizeY, comp;
	auto d = stbi_load("surfaceTexture.tga", &sizeX, &sizeY, &comp, 4);

	//Create height map and normal map from surface texture
}
