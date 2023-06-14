#pragma once // Including guard 
#include "Entity.h"

// This class is a wrapper around a sphere and provides some useful functions for drawing 
class Sphere : public Entity {
public:
	Sphere(unsigned int countSectoring, unsigned int countStacking, float radius); // Constructor for Sphere class
	
	~Sphere() override; 

	// Method to draw the sphere 
	void draw() const override;
		// Method to get the radius of the sphere
		float gettingRadius() const { return radius; }; 

private:
	void constructModel(uint32_t countSectoring, uint32_t countStacking); // Method to construct the sphere model
	
	// Vertex array object, vertex buffer object, handling no. of vertices and indices in the sphere model and radius of the sphere
	GLuint ebo = 0U; // Element buffer object 
	float radius = 1.f; 
	GLuint indicesCount = 0; 
};

