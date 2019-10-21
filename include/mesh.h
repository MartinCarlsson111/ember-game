#pragma once
#include "vertex.h"
#include <vector>

struct Mesh
{
	unsigned int vao, vbo, nVerts;
};

class _Mesh
{
public:
	void CreateUnitQuad();
	void CreateSphere(float radius);
	void Initialize();
	void DeleteBuffers();

	std::vector<Vertex> vertices;
	Mesh mesh;
};