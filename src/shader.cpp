#include "shader.h"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>
#include <sstream>
#include <fstream>

GLint Shader::GetUniformLocation(const GLchar* name)
{
	std::map < std::string, GLint > ::iterator it = mUniformLocations.find(name);

	if (it == mUniformLocations.end())
	{
		mUniformLocations[name] = glGetUniformLocation(handle, name);
	}
	return mUniformLocations[name];
}

void Shader::Use()
{
	if (handle > 0)
	{
		glUseProgram(handle);
	}
}

void Shader::SetUniform(const GLchar* name, const glm::vec2& v)
{
	Use();
	GLint loc = GetUniformLocation(name);
	glUniform2f(loc, v.x, v.y);
}

void Shader::SetUniform(const GLchar* name, const glm::vec3& v)
{
	Use();
	GLint loc = GetUniformLocation(name);
	glUniform3f(loc, v.x, v.y, v.z);
}

void Shader::SetUniform(const GLchar* name, const glm::vec4& v)
{
	Use();
	GLint loc = GetUniformLocation(name);
	glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void Shader::SetUniform(const GLchar* name, std::vector<float>& m, unsigned int count)
{
	Use();
	GLint loc = GetUniformLocation(name);

	assert(count < 76);

	glUniformMatrix4fv(loc, count, GL_FALSE, m.data());
}

void Shader::SetUniform(const GLchar* name, const glm::mat4& m)
{
	Use();
	GLint loc = GetUniformLocation(name);

	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::SetUniform(const GLchar* name, const GLfloat f)
{
	Use();
	GLint loc = GetUniformLocation(name);
	glUniform1f(loc, f);
}

void Shader::SetUniform(const GLchar* name, const GLint i)
{
	Use();
	GLint loc = GetUniformLocation(name);
	glUniform1i(loc, i);
}

void Shader::SetUniformSampler(const GLchar* name, const GLint slot)
{
	Use();
	glActiveTexture(GL_TEXTURE0 + slot);
	GLint loc = GetUniformLocation(name);
	glUniform1i(loc, slot);
}

void Shader::SetUniformBlock(const GLchar* name, const GLint blockIndex)
{
	GLuint index = glGetUniformBlockIndex(handle, name);
	if (index != GL_INVALID_INDEX)
	{
		glUniformBlockBinding(handle, index, blockIndex);
	}
	else
	{
		std::cout << "GL_INVALID_INDEX" << std::endl;
	}

}


void Shader::DeleteProgram()
{
	glDeleteProgram(handle);
}

void Shader::LoadShaders(const char* vsFilename, const char* fsFilename)
{
	std::string vsString = FileToString(vsFilename);
	const GLchar* vsSourcePtr = vsString.c_str();

	std::string fsString = FileToString(fsFilename);
	const GLchar* fsSourcePtr = fsString.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vs, 1, &vsSourcePtr, NULL);
	glShaderSource(fs, 1, &fsSourcePtr, NULL);

	glCompileShader(vs);
	CheckCompileErrors(vs, VERTEX);

	glCompileShader(fs);
	CheckCompileErrors(fs, FRAGMENT);

	handle = glCreateProgram();

	glAttachShader(handle, vs);
	glAttachShader(handle, fs);
	glLinkProgram(handle);

	CheckCompileErrors(handle, PROGRAM);

	glDeleteShader(vs);
	glDeleteShader(fs);
}

void Shader::LoadShaders(const char* vsFilename, const char* fsFilename, const char* tesFileName, const char* tescFileName)
{
	std::string vsString = FileToString(vsFilename);
	const GLchar* vsSourcePtr = vsString.c_str();

	std::string fsString = FileToString(fsFilename);
	const GLchar* fsSourcePtr = fsString.c_str();

	std::string tesString = FileToString(tesFileName);
	const GLchar* tesSourcePtr = tesString.c_str();

	std::string tescString = FileToString(tescFileName);
	const GLchar* tescSourcePtr = tescString.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint tes = glCreateShader(GL_TESS_EVALUATION_SHADER);
	GLuint tesc = glCreateShader(GL_TESS_CONTROL_SHADER);

	glShaderSource(vs, 1, &vsSourcePtr, NULL);
	glShaderSource(fs, 1, &fsSourcePtr, NULL);
	glShaderSource(tes, 1, &tesSourcePtr, NULL);
	glShaderSource(tesc, 1, &tescSourcePtr, NULL);


	glCompileShader(vs);
	CheckCompileErrors(vs, VERTEX);

	glCompileShader(fs);
	CheckCompileErrors(fs, FRAGMENT);

	glCompileShader(tes);
	CheckCompileErrors(tes, TESS);

	glCompileShader(tesc);
	CheckCompileErrors(tesc, TESSCONTROL);

	handle = glCreateProgram();

	glAttachShader(handle, vs);
	glAttachShader(handle, fs);
	glAttachShader(handle, tes);
	glAttachShader(handle, tesc);
	glLinkProgram(handle);

	CheckCompileErrors(handle, PROGRAM);

	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(tes);
	glDeleteShader(tesc);
}
void Shader::LoadShaders(const char* vsFilename, const char* fsFilename, const char* geoShaderName)
{
	std::string vsString = FileToString(vsFilename);
	const GLchar* vsSourcePtr = vsString.c_str();

	std::string fsString = FileToString(fsFilename);
	const GLchar* fsSourcePtr = fsString.c_str();


	std::string geoSource = FileToString(geoShaderName);
	const GLchar* gsSourcePtr = geoSource.c_str();



	GLint gs = glCreateShader(GL_GEOMETRY_SHADER);
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(gs, 1, &gsSourcePtr, NULL);

	glShaderSource(vs, 1, &vsSourcePtr, NULL);
	glShaderSource(fs, 1, &fsSourcePtr, NULL);

	glCompileShader(vs);
	CheckCompileErrors(vs, VERTEX);

	glCompileShader(fs);
	CheckCompileErrors(fs, FRAGMENT);

	glCompileShader(gs);
	CheckCompileErrors(fs, ShaderType::TESS);


	handle = glCreateProgram();

	glAttachShader(handle, vs);
	glAttachShader(handle, fs);
	glAttachShader(handle, gs);
	glLinkProgram(handle);

	CheckCompileErrors(handle, PROGRAM);

	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(gs);
}


std::string Shader::FileToString(const std::string& filename)
{
	std::stringstream ss;
	std::ifstream file;

	try
	{
		file.open(filename, std::ios::in);

		if (!file.fail())
		{
			ss << file.rdbuf();
		}
		file.close();
	}
	catch (std::exception ex)
	{
		std::cerr << "Error reading filename" << std::endl;
	}
	return ss.str();
}

void Shader::CheckCompileErrors(GLuint shader, ShaderType type)
{
	int status = 0;
	if (type == PROGRAM)
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint length = 0;
			glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &length);

			std::string errorLog(length, ' ');
			glGetProgramInfoLog(shader, length, &length, &errorLog[0]);
			std::cerr << "Error: Program failed to link" << errorLog << std::endl;
		}
	}
	else
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint length = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

			std::string errorLog(length, ' ');
			glGetShaderInfoLog(shader, length, &length, &errorLog[0]);
			std::cerr << "Error: " << type << "shader failed to compile" << errorLog << std::endl;
		}
	}
}
