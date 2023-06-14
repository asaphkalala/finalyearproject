#pragma once
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>
#include "Shader.h"

class Entity {
public:
    Entity(); // Constructor for Entity class
    virtual ~Entity();

	// Method to draw the entity 
    void draw() const = 0; 
    void scale(const glm::vec3& factor);
    void translate(const glm::vec3& factor);

	// Getters and setters for the entity's transform matrix, scale, translation, and rotation 
    const glm::mat4& getTransformMatrix() const { return transformMatrix; }
    const glm::vec3& getScale() const { return scaleVector; }
    const glm::vec3& getTranslation() const { return translationVector; }
    glm::vec3 color{ 1.0f }; // Colour of the entity set to white 

    
	enum class Material {
        PLASTIC,
        HAIR
    }; // Material of the entity

    void updateMaterial(const Shader& shader, Material material) const; // Method to update the entity's colour based on its material
};

