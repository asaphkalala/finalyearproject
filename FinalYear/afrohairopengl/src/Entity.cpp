#include "Entity.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

class Entity {
public:
    Entity();
    ~Entity();
		void rotate(float angle, const glm::vec3& axis);
		void scale(const glm::vec3& factor);
		void translate(const glm::vec3& factor);
		void updateMaterial(const Shader& shader, Material material) const;

	// Getters and setters for the entity's rotation, scale, and translation vectors
		const glm::mat4& getTransformMatrix() const { return transformMatrix; }
		const glm::vec3& getScale() const { return scaleVector; }
		const glm::vec3& getTranslation() const { return translationVector; }
    
    glm::vec3 color { 1.0f }; // The color of the entity is white by default

    enum class Material {
        PLASTIC,
        HAIR
    };

private:
		GLuint vbo { 0 }; // Vertex buffer object
		GLuint vao { 0 }; // Vertex array object
		glm::mat4 transformMatrix { glm::mat4(1.0f) };
		glm::vec3 scaleVector { 1.0f };
		glm::vec3 translationVector { 0.0f };

    void updateTransformMatrix();
};

// Generate the vertex array and buffer object
Entity::Entity() {
    glGenBuffers(1, &vbo); 
    glGenVertexArrays(1, &vao); 
}

// Delete the vertex array and buffer object
Entity::~Entity() {
    glDeleteBuffers(1, &vbo); // Delete the vertex buffer object
    glDeleteVertexArrays(1, &vao); 
}

// Scaling and translating the entity by the given angle around the given axis

void Entity::scale(const glm::vec3& scale) {
    scaleVector = scale;
    updateTransformMatrix();
}

void Entity::translate(const glm::vec3& translation) {
    translationVector = translation;
    updateTransformMatrix();
}
void Entity::updateMaterial(const Shader& shader, Material material) const {
    switch (material) {
        case Material::PLASTIC: // If the material is plastic, set the material's ambient, diffuse, specular, and shininess values
            shader.setVec3("material.ambient", 0.2f * color);
            shader.setVec3("material.diffuse", 0.8f * color);
            shader.setVec3("material.specular", color);
            shader.setFloat("material.shininess", 5.0f);
            break;
        case Material::HAIR: // If the material is hair, set the material's ambient, diffuse, specular, and shininess values
            shader.setVec3("material.ambient", 0.1f * color);
            shader.setVec3("material.diffuse", color * 0.15f);
            shader.setVec3("material.specular", color * 0.4f);
            shader.setFloat("material.shininess", 300.0f);
            break;
    }
}
void Entity::updateTransformMatrix() { // Update the transform matrix based on the scale, rotation, and translation vectors
    transformMatrix = glm::scale(glm::translate(glm::mat4(1.0f), translationVector) * glm::mat4_cast (scaleVector);
}
