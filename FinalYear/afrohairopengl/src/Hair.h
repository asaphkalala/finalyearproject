#pragma once
#include "Entity.h"
#include "ComputeShader.h"
#include <memory>
#include <vector>
#include <array>
#include "Sphere.h"
#include "Window.h"

class Hair : public Entity {
public: // Constructor for Hair class
	Hair(uint32_t _strandCount = 5000U,  // Number of strands set to 5000
	float hairLength = 3.f, // Length of the hair set to 3
	float hairCurliness = 0.0f); // Curliness of the hair set to 0

	// Destructor for Hair class
	~Hair();
	void draw() const override;
	void drawHead() const; 
	void applyPhysics(float deltaTime, float runningTime); // Applying physics to the hair using the compute shader 
	void setGravity(float strength); // Method to set the gravity strength 

	// Increases and decreases strand count by 1000 clamped in range [0, 10000]
	void increaseStrandCount(); 
	void decreaseStrandCount(); 

	// Increases and decreases hair length by 0.1 clamped in range [0, 10]
	void increaseVelocityDamping();
	void decreaseVelocityDamping();

	// Increases and decreases hair curliness by 0.01 clamped in range [0, 0.1]
	float getCurlRadius() const { return curlRadius; }
	float getFrictionFactor() const { return frictionFactor; }
	uint32_t particleperStrand() const { return particlesPerStrand; }
	const std::array<std::unique_ptr<Sphere>, 7>& getEllipsoids() const { return ellipsoids; } 
	
	// Increases curl radius by 0.01 clamped in range [0, 0.05]
	void increaseCurlRadius(); 

	// Decreases curl radius by 0.01 clamped in range [0, 0.05]
	void decreaseCurlRadius();
	
	// Sets friction factor clamped in range [0, 1] 
	void setFrictionFactor(float friction);

private:
	GLuint velocityArrayBuffer = GL_NONE;		// Shader storage buffer object for velocities
	GLuint volumeDensities = GL_NONE;
	GLuint volumeVelocities = GL_NONE;

	uint32_t strandCount;
	float curlRadius = 0.0f; // Radius of the curl 
	ComputeShader drawAfro; //`Compute shader for physics calculations

	GLuint strandArrayBuffer = GL_NONE; // Shader storage buffer object for strands
	GLuint strandVertexArray = GL_NONE; // Vertex array object for strands
	GLuint strandElementArrayBuffer = GL_NONE; // Element buffer object for strands
	GLuint strandIndexCount = 0; // Number of indices in the strand model

	// Hair variables
	uint32_t particlesPerStrand = 15; // Number of particles per strand set to 15
	float gravity = -9.81f; // Gravity set to -9.81
	bool parameterSet = false; // Boolean to check if settings have changed
	const uint32_t maximumStrandCount = 20000U; // 	Maximum number of strands set to 20000 	
	float frictionFactor = 0.02f; // Friction factor set to 0.02

	// Method to construct the hair model
	void constructModel(); 
	float strandWidth = 0.25f; // Width of the strand set to 0.25
	float hairLength = 1.f; // Length of the hair set to 1
	float vdampCoeffic = 0.9f; // Velocity damping coefficient set to 0.9

	// Head variables: Vertex array object, vertex buffer object, element buffer object, index count and head color
	glm::vec3 headColor;
	GLuint headVbo = GL_NONE; 
	GLuint headVao = GL_NONE; 
	GLuint headEbo = GL_NONE; 
	uint32_t indexCount = 0;
	std::array<std::unique_ptr<Sphere>, 7> ellipsoids;
	float ellipsoidsRadius = 0.5f;
};