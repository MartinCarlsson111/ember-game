#include "mesh.h"
#include "GL/glew.h"
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
