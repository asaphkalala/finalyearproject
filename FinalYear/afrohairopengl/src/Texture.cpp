#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION // Define this macro to include implementation of stb_image.h
#include <stb_image.h> // Library for loading images
#include <iostream>
#include "PathConfig.h" 


Texture::Texture(const std::string& texture_name, GLuint texture_type, const bool gamma_correction) : textureType(texture_type)
{
    // Generate texture ID
    glGenTextures(1, &textureID);

    // Generate texture based on texture type
    switch (texture_type)
    {
    case GL_TEXTURE_2D:
        generate2DTexture(texture_name, gamma_correction);
        break;

		
    case GL_TEXTURE_CUBE_MAP:
        generateCubeMapTexture(texture_name, gamma_correction);
        break;
    }
}

Texture::~Texture()
{
    // Delete texture
    glDeleteBuffers(1, &textureID);
}

void Texture::activateAndBind(const GLuint texture_unit) const
{
    // Activate texture unit and bind texture
    glActiveTexture(texture_unit);
    glBindTexture(textureType, textureID);
}

void Texture::generateCubeMapTexture(const std::string& texture_name, const bool gamma_correction) const
{
    // Bind texture ID and set vertical to false
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    stbi_set_flip_vertically_on_load(false);

    // Load and generate each face of the cube map texture
    int width, height, nrChannels;
    unsigned char* data;
    for (int i = 0; i < 6; i++)
    {
        std::string texName = TEXTURE_FOLDER + texture_name;
        texName.insert(texName.begin() + texName.find('.'), char(i + CHAR_OFFSET));
        data = stbi_load(texName.c_str(), &width, &height, &nrChannels, 0);

        // Determine texture format based on number of channels and gamma correction flag
        if (nrChannels == 4)
        {
            if (gamma_correction) 
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            else 
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            if (gamma_correction) 
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            else 
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }

        // Free image data
        stbi_image_free(data);
    }

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Texture::generate2DTexture(const std::string& texture_name, const bool gamma_correction) const
{
    // Load texture image and set vertical flip flag
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load((TEXTURE_FOLDER + texture_name).c_str(), &width, &height, &nrChannels, 0);

    // Bind texture ID
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Determine texture format based on number of channels and gamma correction flag
    if (nrChannels == 4)
    {
        if (gamma_correction) 
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        else 
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Generate image data
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}
