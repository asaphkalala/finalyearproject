#include <glad/glad.h>
#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include "Shader.h"
#include "PathConfig.h"

Shader::~Shader()
{
	glDeleteProgram(programID);
}

GLint Shader::getUniformLocation(const std::string& name) const // Get the location of a uniform variable
{
	if ((uniformCache.find(name)) != uniformCache.end()) // If the uniform variable is already in the cache, return its location
		return uniformCache[name].first;
	return location;
}


void Shader::linkProgram() const
{
	if (!programID) // If the program ID is 0, then no shaders have been attached
		std::cout << "Error: No shaders attached!" << std::endl;

	GLint success;
	char infoLog[512]; // For storing the error log
	
	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		(programID, 512, nullptr, infoLog);
		std::cout << "Failed to link: " << infoLog << std::endl; // Print the error log
	}
}

void Shader::attachShader(const std::string& shaderFileName, GLuint& shaderID)
{
	std::string shaderCode;
	std::ifstream shaderFile;

	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit); // Set the exception flags for the shader file stream

	try {
		shaderFile.open(SHADER_FOLDER + shaderFileName); // Open the shader file
		std::stringstream ShaderStream;

		ShaderStream << shaderFile.rdbuf(); // Read the shader file's buffer contents into the string stream
		shaderCode = ShaderStream.str(); // 
	}
	catch (std::ifstream::failure& e) { // If the shader file stream fails to open, print an error message
		std::cout << e.what() << std::endl;
		std::cout << "Error: File not found!" << std::endl;
	}

	GLint success;
	char infoLog[512];
	const char* shaderCodeString = shaderCode.c_str();

	glShaderSource(shaderID, 1, &shaderCodeString, NULL);
	glCompileShader(shaderID);
	glGetShaderz(shaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
		std::cout << "Failed to compile  shader: " << shaderFileName << " " << infoLog << std::endl;
	}

	if (!programID)
		programID = glCreateProgram();

	glAttachShader(programID, shaderID);
}


// Use the shader program for rendering
void Shader::use() const 
{
	glUseProgram(programID);
}

void Shader::setBool(const std::string& name, const bool value) const
{
	glUniform1i(getUniformLocation(name), value);
}

void Shader::setBoolArray(const std::string& name, GLsizei count, bool values[]) const
{
	glUniform1iv(getUniformLocation(name), count, (GLint*)values);
}

void Shader::setInt(const std::string& name, const int value) const
{
	glUniform1i(getUniformLocation(name), value);
}

void Shader::setIntArray(const std::string& name, GLsizei count, const GLint value[]) const
{
	glUniform1iv(getUniformLocation(name), count, value);
}

void Shader::setUint(const std::string& name, const uint32_t value) const
{
	glUniform1ui(getUniformLocation(name), value);
}

void Shader::setUintArray(const std::string& name, int count, const uint32_t value[]) const
{
	glUniform1uiv(getUniformLocation(name), count, value);
}

void Shader::setFloat(const std::string& name, const float value) const
{
	glUniform1f(getUniformLocation(name), value);
}

void Shader::setFloatArray(const std::string& name, GLsizei count, const GLfloat value[]) const
{
	glUniform1fv(getUniformLocation(name), count, value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2f(getUniformLocation(name), value.x, value.y);
}

void Shader::setVec2Array(const std::string& name, GLsizei count, const glm::vec2 values[]) const
{
	glUniform2fv(getUniformLocation(name), count, &values[0].x);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}

void Shader::setVec3Array(const std::string& name, GLsizei count, const glm::vec3 values[]) const
{
	glUniform3fv(getUniformLocation(name), count, &values[0].x);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::setVec4Array(const std::string& name, GLsizei count, const glm::vec4 values[]) const
{
	glUniform4fv(getUniformLocation(name), count, &values[0].x);
}

void Shader::setMat2(const std::string& name, const glm::mat2& value) const
{
	glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat2Array(const std::string& name, GLsizei count, const glm::mat2 values[]) const
{
	glUniformMatrix2fv(getUniformLocation(name), count, GL_FALSE, &values[0][0].x);
}

void Shader::setMat3(const std::string& name, const glm::mat3& value) const
{
	glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat3Array(const std::string& name, GLsizei count, const glm::mat3 values[]) const
{
	glUniformMatrix3fv(getUniformLocation(name), count, GL_FALSE, &values[0][0].x);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat4Array(const std::string& name, GLsizei count, const glm::mat4 values[]) const
{
	glUniformMatrix4fv(getUniformLocation(name), count, GL_FALSE, &values[0][0].x);
}

void Shader::bindShaderUboToBindingPoint(const std::string& uniformBlockName, const GLuint bindingPoint) const
{
	glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, uniformBlockName.c_str()), bindingPoint);
}