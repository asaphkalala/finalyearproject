#pragma once
#include <glad/glad.h>
#include <string>

class Texture {
public:
	// Constructor for Texture class
	Texture(const std::string& textureName, 
	GLuint textureType = GL_TEXTURE_2D, 
	const bool applyGammaCorrection = true);

	// Destructor for Texture class
	~Texture();
	// Method to activate and bind the texture to a specified texture unit
	void activateAndBind(const GLuint textureUnit) const;

private:
	// Type of the texture
	GLuint textureID{ 0 };
	const GLuint textureType; 	
	// Method to generate a cube map texture
	void generateCubeMap(const std::string& name, 
	const bool applyGammaCorrection) const;	

	// Method to generate a 2D texture
	void generate2DMap(const std::string& name, 
	const bool applyGammaCorrection) const;
};