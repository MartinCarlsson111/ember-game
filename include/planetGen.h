#pragma once
#include <random>
#include "planetParameters.h"
#include <stdint.h>

#include "stb_image.h"

#include "glm/glm.hpp"

#include "mesh.h"
#include "shader.h"
class PlanetGen
{
public:
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


	std::random_device rd;
	std::vector<col> data;
	std::vector<glm::ivec4> triangles;
	_Mesh mesh;
	Shader shader;
	int iterations = 800;

		
	void CreateTris();


	void RaisePointsInTris();


	void FractureAlgo();

	void Generate(PlanetParameters parameters)
	{
		FractureAlgo();
		CreateSphere();
	}

	void CreateSphere()
	{
		mesh.CreateSphere(15.0f);

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

