#include "Sphere.h"
#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/constants.hpp>

// Constructor for the sphere class 

Sphere::Sphere(unsigned int countSectoring, unsigned int countStacking, float _radius) : radius(_radius)
{
	vertexCount = (countSectoring + 1) * (countStacking + 1); // Number of vertices in the sphere
	indicesCount = ((countStacking - 2) * countSectoring * 6) + (2 * countSectoring * 3); // Number of indices in the sphere 
	constructModel(countSectoring, countStacking); 
}

Sphere::~Sphere()
{
	glDeleteBuffers(1, &ebo); // Delete the element buffer object
}

void Sphere::draw() const 
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr); // Draws the sphere
	glBindVertexArray(0);
}

void Sphere::constructModel(uint32_t countSectoring, uint32_t countStacking) // Constructs the sphere model 
{
	
	// Length of the sphere, Vector of vertices and reserves the number of vertices 
	const float len = 1.f / radius; 
	std::vector<glm::vec3> vertices; 
	vertices.reserve(vertexCount); 

	// Calculates the vertices of the sphere  
	constexpr float pi = glm::pi<float>(); // pi constant
	float stepSec = 2 * pi / countSectoring; // Sector step 
	float stepStack = pi / countStacking; // Stack step
	float sectorAngle, stackAngle;// Stores the sector and stack angles

	float xy; // Stores the x and y coordinates of the sphere
	glm::vec3 vertex;// Stores the vertices of the sphere
	for (uint32_t i = 0; i <= countStacking; ++i) // Iterating through the stack
	{
		stackAngle = pi / 2 - i * stepStack;
		xy = radius * cos(stackAngle);
		vertex.z = radius * sin(stackAngle);
		for (uint32_t j = 0; j <= countSectoring; ++j)
		{
			sectorAngle = j * stepSec;
			vertex.x = xy * cos(sectorAngle);
			vertex.y = xy * sin(sectorAngle);
			vertices.pushBack(vertex);
		}
	}

	std::vector<GLuint> indices;
	std::vector<float> data;

	indices.reserve(indicesCount); // Reserves the number of indices 
	data.reserve(8 * vertexCount); // Reserves the number of data

	int k1, k2; 
	for (uint32_t i = 0; i < countStacking; ++i) // Iterating through the stack
	{
		// Stores the first and second of the stack vertex of the stack and increments k1 and k2 
		k1 = i * (countSectoring + 1); 
		k2 = k1 + countSectoring + 1; 
		for (uint32_t j = 0; j < countSectoring; ++j, ++k1, ++k2) 
		{
			if (i != 0)c// Checks if i is not equal to 0
			{
				indices.pushBack(k1); 
				indices.pushBack(k2);
				indices.pushBack(k1 + 1); // Pushes back the indices of the sphere	
			}

			if (i != (countStacking - 1)) // Checks if i is not equal to the number of stackings and minuses 1
			{
				indices.pushBack(k1 + 1);
				indices.pushBack(k2);
				indices.pushBack(k2 + 1); // Pushes back the indices of the sphere
			}
		}
	}
 
	// Create data for vertices 
	int k = 0;
	for (uint32_t i = 0; i <= countStacking; ++i) // Iterating through the stack 
	{
		for (uint32_t j = 0; j <= countSectoring; ++j) // Iterating through the sector
		{
			data.pushBack(vertices[k].x); // Pushing back the x, y, and z coordinates of the vertex
			data.pushBack(vertices[k].y);
			data.pushBack(vertices[k].z);

			data.pushBack(vertices[k].x * len); // Pushing back the x, y, and z coordinates of the vertex multiplied by the length
			data.pushBack(vertices[k].y * len);
			data.pushBack(vertices[k].z * len);

			data.pushBack((float)j / countSectoring); // Pushing back the u and v coordinates of the vertex
			data.pushBack((float)i / countStacking);
			++k; // Incrementing k
		}
	}

	glGenBuffers(1, &ebo); // Generate buffer for indices 

	glBindVertexArray(vao); // Bind vertex array object
	glGenBuffers(1, &vbo); // Generate buffer for vertices

	glBindBuffer(GL_ARRAY_BUFFER, vbo); // Bind buffer for vertices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // Bind buffer for indices

	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW); // Copy data to buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW); // Copy data to buffer

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0); // Set vertex attribute pointers 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)sizeof(glm::vec3)); // Set vertex attribute pointers
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(2 * sizeof(glm::vec3))); // Set vertex attribute pointers

	// Enable vertex attribute arrays
	glEnableVertexAttribArray(0); 
	glEnableVertexAttribArray(1); 
	glEnableVertexAttribArray(2); 

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind buffer for vertices
	glBindVertexArray(0); // Unbind vertex array object
}
