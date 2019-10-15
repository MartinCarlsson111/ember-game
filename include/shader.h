#pragma once
#include "GL/glew.h"
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include <map>

class Shader
{
public:
	GLuint handle;

	enum ShaderType
	{
		VERTEX,
		FRAGMENT,
		TESS,
		TESSCONTROL,
		PROGRAM
	};
	void DeleteProgram();
	void LoadShaders(const char* vsFilename, const char* fsFilename);
	void LoadShaders(const char* vsFilename, const char* fsFilename, const char* tesFileName, const char* tescFileName);

	void LoadShaders(const char* vsFilename, const char* fsFilename, const char* geoShaderName);

	void CheckCompileErrors(GLuint shader, ShaderType type);
	std::string FileToString(const std::string& filename);
	GLint GetUniformLocation(const GLchar* name);
	void SetUniform(const GLchar* name, const glm::vec2& v);
	void SetUniform(const GLchar* name, const glm::vec3& v);
	void SetUniform(const GLchar* name, const glm::vec4& v);
	void SetUniform(const GLchar* name, std::vector<float>& m, unsigned int count);
	void SetUniform(const GLchar* name, const glm::mat4& m);
	void SetUniform(const GLchar* name, const GLfloat f);
	void SetUniform(const GLchar* name, const GLint i);
	void SetUniformSampler(const GLchar* name, const GLint slot);
	void SetUniformBlock(const GLchar* name, const GLint blockIndex);

	std::map<std::string, GLint> mUniformLocations;
	void Use();
};