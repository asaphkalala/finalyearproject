#include "DrawingShader.h"
// Constructor for DrawingShader class 
DrawingShader::DrawingShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) 
{
	// Create vertex and fragment shader ID
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER); 
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER); 

	// Attach vertex and fragment shader to shader program and link program
	attachShader(vertexShaderFile, vertexShaderID);
	attachShader(fragmentShaderFile, fragmentShaderID);
	linkProgram();

	// Delete vertex and fragment shader 
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

DrawingShader::DrawingShader(const std::string& vertexShaderFile, 
const std::string& geometryShaderFile, 
const std::string& fragmentShaderFile) 
{
	// Create vertex, geometry, and fragment shader ID
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);


	// Attach vertex, geometry, and fragment shader to shader program and link program
	attachShader(vertexShaderFile, vertexShaderID);
	attachShader(fragmentShaderFile, fragmentShaderID);
	attachShader(geometryShaderFile, geometryShaderID);
	linkProgram();

	// Delete vertex, geometry, and fragment shader
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	glDeleteShader(geometryShaderID);
}
