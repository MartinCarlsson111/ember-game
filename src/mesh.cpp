#include "mesh.h"
#include "GL/glew.h"
#include "glm/glm.hpp"


void _Mesh::CreateUnitQuad()
{
	glm::vec3 verts[] =
	{
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f)
	};
	Vertex v;
	v.position = verts[0];
	vertices.push_back(v);

	v.position = verts[3];
	vertices.push_back(v);

	v.position = verts[1];
	vertices.push_back(v);

	v.position = verts[0];
	vertices.push_back(v);

	v.position = verts[2];
	vertices.push_back(v);

	v.position = verts[3];
	vertices.push_back(v);

	Initialize();
}

#define LOGITUDE 12
#define LATITUDE 12

#include "glm/gtc/constants.hpp"
const int numVertices = LATITUDE * (LOGITUDE + 1) + 2;

void _Mesh::CreateSphere(float radius)
{
	glm::vec3 positions[numVertices];
	glm::vec2 texcoords[numVertices];
	positions[0] = glm::vec3(0, radius, 0);
	texcoords[0] = glm::vec2(0, 1);

	float latitudeSpacing = 1.0f / (LATITUDE + 1.0f);
	float longitudeSpacing = 1.0f / (LOGITUDE);

	positions[numVertices - 1] = glm::vec3(0, -radius, 0);
	texcoords[numVertices - 1] = glm::vec2(0, 0);

	int v = 1;
	for (int latitude = 0; latitude < LATITUDE; latitude++)
	{
		for (int logitude = 0; logitude < LOGITUDE; logitude++)
		{
			texcoords[v] = glm::vec2(logitude * longitudeSpacing, 1.0f - (latitude +1) * latitudeSpacing);
			float theta = texcoords[v].x * 2.0f * glm::pi<float>();
			float phi = (texcoords[v].y - 0.5f) * glm::pi<float>();
			float c = cos(phi);
			positions[v] = glm::vec3(c * cos(theta), sin(phi), c * sin(theta)) * radius;
			v++;
		}
	}
	Initialize();
}

void _Mesh::Initialize()
{
	glGenBuffers(1, &mesh.vbo);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);

	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	glEnableVertexAttribArray(0);

	//tex coords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	mesh.nVerts = vertices.size();
}

void _Mesh::DeleteBuffers()
{
	glDeleteVertexArrays(1, &mesh.vao);
	glDeleteBuffers(1, &mesh.vbo);
}
