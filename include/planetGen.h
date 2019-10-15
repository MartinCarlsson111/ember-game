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
	int iterations = 400;
	/*Create seeded random source(spatial / non - spatial)
		Set planet parameters*/

		
	void CreateTris()
	{
		triangles.clear();
		//n triangles;

		int nTriangles = 10;
		std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
		std::uniform_int_distribution<int> dis(0, 1024);
		std::uniform_int_distribution<int> disExtent(3, 15);


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
		for (int i = 0; i < triangles.size(); i++)
		{
			for (int x = 0; x < triangles[i].z; x++)
			{
				for (int y = 0; y < triangles[i].w; y++)
				{
					int xPos = triangles[i].x + x;
					int yPos = triangles[i].y + y;

					data[xPos + (yPos* reso)].r = 



				}
			}
		}

		for (int i = 0; i < 250; i++)
		{
			for (int y = 0; y < 233; y++)
			{

			}
		}
	}

	void FractureAlgo()
	{
		data = std::vector<col>(1024 * 1024);
		CreateTris();


		//for (int i = 0; i < iterations; i++)
	//	{
			RaisePointsInTris();
	//	}


		for (int i = 0; i < reso * reso; i++)
		{
			data[i].a = 255;
		}


		stbi_write_tga("surfaceTexture.png", reso, reso, 4, data.data());

		int sizeX, sizeY, comp;
		auto d = stbi_load("surfaceTexture.png", &sizeX, &sizeY, &comp, 4);

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

