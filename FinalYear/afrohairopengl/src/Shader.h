#pragma once
#include <string>
#include <glad/glad.h>
#include <unordered_map>
#include <glm/glm.hpp>

class Shader {
public:
	Shader() = default;
	virtual ~Shader() = 0; // Virtual destructor for Shader class
	void use() const;
	// Setters for the shader's uniform variables
	void setInt(const std::string& name, const int value) const; // Integer as a uniform variable in the shader
	void setIntArray(const std::string& name, GLsizei count, const GLint value[]) const; // Array of integers as a uniform variable in the shader 

	void setUint(const std::string& name, const uint32_t value) const;
	void setUintArray(const std::string& name, int count, const uint32_t value[]) const; // Unsigned integers 

	void setBool(const std::string& name, const bool value) const;
	void setBoolArray(const std::string& name, GLsizei count, bool values[]) const; // Array of booleans 

	void setFloat(const std::string& name, const float value) const;
	void setFloatArray(const std::string& name, GLsizei count, const GLfloat values[]) const; // Array of floats

	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec2Array(const std::string& name, GLsizei count, const glm::vec2 values[]) const; // Array of vec2s

	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec3Array(const std::string& name, GLsizei count, const glm::vec3 values[]) const; // Array of vec3s

	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setVec4Array(const std::string& name, GLsizei count, const glm::vec4 values[]) const; // Array of vec4s

	void setMat2(const std::string& name, const glm::mat2& value) const;
	void setMat2Array(const std::string& name, GLsizei count, const glm::mat2 values[]) const; // Array of mat2s

	void setMat3(const std::string& name, const glm::mat3& value) const;
	void setMat3Array(const std::string& name, GLsizei count, const glm::mat3 values[]) const; // Array of mat3s

	void setMat4(const std::string& name, const glm::mat4& value) const;
	void setMat4Array(const std::string& name, GLsizei count, const glm::mat4 values[]) const; // Array of mat4s

	void bindShaderUboToBindingPoint(const std::string& uniformBlockName, const GLuint bindingPoint) const; // Method to bind a shader uniform block to a binding point

protected:
	GLuint programID = GL_NONE; // ID of the shader program
	GLint getUniformLocation(const std::string& name) const; // Method to get the location of a uniform variable in the shader
	void linkProgram() const; // Method to link the shader program 
	void attachShader(const std::string& shaderFileName, GLuint& shaderID); // Method to compile and attach a shader to the shader program
};