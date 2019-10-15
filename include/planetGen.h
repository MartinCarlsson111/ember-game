#include <random>
#include "planetParameters.h"
#include <stdint.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


#include "stb_image.h"

class PlanetGen
{
public:
	//random engine
	//random gen
	const int reso = 1024;

	struct col
	{
		col()
		{
			r = 0;
			g = 0; 
			b = 0;
			a = 0;
		}
		int8_t r, g, b,a;
	};


	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::vector<col> data;
	std::vector<glm::ivec4> triangles;
	int iterations = 800;
	/*Create seeded random source(spatial / non - spatial)
		Set planet parameters*/

		
	void CreateTris()
	{
		triangles.clear();
		//n triangles;

		int nTriangles = 20;
		std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
		std::uniform_int_distribution<int> dis(0, 1024);
		std::uniform_int_distribution<int> disExtent(-20, 20);


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

	void RaisePointsInTris()
	{
		std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
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

					data[xPos + (yPos * reso)].r += dis(gen);
					data[xPos + (yPos * reso)].g += dis(gen);
					data[xPos + (yPos * reso)].b += dis(gen);
				}
			}
		}
	}

	void FractureAlgo()
	{
		data = std::vector<col>(1024 * 1024);

		for (int i = 0; i < iterations; i++)
		{
			CreateTris();
			RaisePointsInTris();
		}


		for (int i = 0; i < reso * reso; i++)
		{
			data[i].a = 255;
		}


		stbi_write_tga("surfaceTexture.tga", reso, reso, 4, data.data());

		int sizeX, sizeY, comp;
		auto d = stbi_load("surfaceTexture.tga", &sizeX, &sizeY, &comp, 4);

		//Create height map and normal map from surface texture
	}

	void Generate(PlanetParameters parameters)
	{
		
		FractureAlgo();
	}

	void CreateSphere()
	{

	}

	void Deform()
	{
		//Deform sphere according to height map and normal map


	}

	void Color()
	{
		//Color sphere based on surface texture
		//	Done!
	}
};

