#include "Hair.h"
#include <iostream>
#include <glm/gtc/random.hpp>
#include "glm/gtc/quaternion.hpp"
#include "PathConfig.h"
#include "OBJ_Loader.h"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>


// This is the constructor for the Hair class that sets the strand count, hair length, and curl radius
Hair::Hair(uint32_t _strandCount, float hairLength, float hairCurlRadius) : strandCount(_strandCount), hairLength(hairLength),
				curlRadius(hairCurlRadius), drawAfro("HairComputeShader.glsl") 
{
	drawAfro.use();
	drawAfro.setUint("hairData.strandCount", strandCount);
	drawAfro.setFloat("hairData.particleMass", 0.1f);
	drawAfro.setFloat("force.gravity", gravity);
	drawAfro.setFloat("frictionCoeffic", frictionFactor);
	drawAfro.setFloat("vdampCoeffic", vdampCoeffic);
	constructModel();
}

Hair::~Hair() // Destructor for Hair class
{
	glDeleteBuffers(1, &velocityArrayBuffer);
	glDeleteBuffers(1, &volumeDensities);
	glDeleteBuffers(1, &volumeVelocities);
}
 
void Hair::constructModel() 	
{
	for (auto& e : ellipsoids)
		e = std::make_unique<Sphere>(50, 30, ellipsoidsRadius);

// These epsilon values are used to prevent the hair from intersecting with the head
	ellipsoids[0]->translate(glm::vec3(0.000000f, -0.388153f, 0.191956f));
	ellipsoids[0]->scale(glm::vec3(2.368103f, 2.519852f, 2.818405f));

	ellipsoids[1]->translate(glm::vec3(0.000000f, -1.074509f, 1.604706f));
	ellipsoids[1]->rotate(-20.f, glm::vec3(1.f, 0.f, 0.f));
	ellipsoids[1]->scale(glm::vec3(0.559738f, 0.620941f, 0.421112f));

	ellipsoids[2]->translate(glm::vec3(0.000000, -0.717041, 0.566460));
	ellipsoids[2]->rotate(-30.f, glm::vec3(1.f, 0.f, 0.f));
	ellipsoids[2]->scale(glm::vec3(2.058214, 3.539791, 1.799336));

	ellipsoids[3]->translate(glm::vec3(0.000000f, -2.556824f, -0.068329f)); 
	ellipsoids[3]->rotate(20.f, glm::vec3(1.f, 0.f, 0.f));
	ellipsoids[3]->scale(glm::vec3(1.798798f, 1.282593f, 1.661377f));

	ellipsoids[4]->translate(glm::vec3(-0.015701f, -1.032532f, 0.122619f));
	ellipsoids[4]->scale(glm::vec3(2.357361f, 3.127426f, 2.326767f));

// Head transformation settings to prevent hair from intersecting with the head
	const glm::vec3 headTranslation(0.f, -3.f, 0.f); 
	const glm::vec3 headScale(0.2f); 
	glm::mat4 headTransform = glm::scale(glm::translate(glm::mat4(1.f), headTranslation) * glm::mat4_cast(headRotation), headScale); 
	headColor = glm::vec3(0.85f, 0.48f, 0.2f); // Head color set to brown

	objl::Loader loader;
	std::vector<float> headData;
	if (loader.LoadFile(TEXTURE_FOLDER + "FemaleHead/FemaleHead.obj"))
	{
		headData.reserve(loader.LoadedVertices.size() * 6 * 3); // 3 position component and 3 normal components
		for (auto& vertex : loader.LoadedVertices)
		{
			// Transforms the vertex position and normal data so that it can be used to create the head VAO and VBO
			glm::vec3 transformedVertex = headTransform * glm::vec4(vertex.Position.X, vertex.Position.Y, vertex.Position.Z, 1.f);
			vertex.Position.X = transformedVertex.x;
			vertex.Position.Y = transformedVertex.y;
			vertex.Position.Z = transformedVertex.z;

// Transforms the vertex normal data used to create the head VAO and VBO
			glm::vec3 transformedNormal = glm::inverse(glm::transpose(headTransform)) * glm::vec4(vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z, 1.f);
			vertex.Normal.X = transformedNormal.x;
			vertex.Normal.Y = transformedNormal.y;
			vertex.Normal.Z = transformedNormal.z;

// Pushes back the vertex position and normal data so that it can be used to create the head VAO and VBO
			headData.pushBack(vertex.Position.X); 
			headData.pushBack(vertex.Position.Y);
			headData.pushBack(vertex.Position.Z);
			headData.pushBack(vertex.Normal.X);
			headData.pushBack(vertex.Normal.Y);
			headData.pushBack(vertex.Normal.Z);
		}

// Creates the head VAO and VBO using the transformed vertex position and normal data
		glCreateVertexArrays(1, &headVao);
		glGenBuffers(1, &headVbo); 
		glGenBuffers(1, &headEbo);

		glBindVertexArray(headVao); // Binds the head VAO
		
		glBindBuffer(GL_ARRAY_BUFFER, headVbo);
		glBufferData(GL_ARRAY_BUFFER, headData.size() * sizeof(float), headData.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, headEbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, loader.LoadedIndices.size() * sizeof(GLuint), loader.LoadedIndices.data(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
		glBindVertexArray(GL_NONE);
		indexCount = loader.LoadedIndices.size();
	}
	else 
	{
		std::cout << "File doesn't exist" << std::endl; // Prints out an error message if the file doesn't exist
	}

	std::vector<float> data; // Vector to store the hair data so that it can be used to create the hair VAO and VBO
	data.reserve(maximumStrandCount * particlesPerStrand * 3);

	float segmentLength = hairLength / (particlesPerStrand - 1);

	drawAfro.setFloat("curlRadius", curlRadius);
	drawAfro.setFloat("hairData.segmentLength", hairLength / (particlesPerStrand - 1));
	drawAfro.setUint("hairData.particlesPerStrand", particlesPerStrand);
	drawAfro.setFloat("ellipsoidRadius", ellipsoidsRadius);
	uint32_t counter = 0;
	for (uint32_t i = 0; i < loader.LoadedVertices.size(); i += 10)
	{
		const auto& vertex = loader.LoadedVertices[i];
		if ((vertex.Position.Y > -1.f && vertex.Position.Z < 0.f) || (vertex.Position.Y > -0.5f && vertex.Position.Z < 0.7f) || (vertex.Position.Y >= 0.5f && vertex.Position.Z < 1.7f))
		{
			++counter;
			if (counter >= maximumStrandCount - 1) break;
			for (uint32_t j = 0; j < particlesPerStrand; ++j)
			{
				glm::vec3 particle(vertex.Position.X, vertex.Position.Y, vertex.Position.Z);
				particle += glm::normalize(particle) * (float)j * segmentLength;
				data.pushBack(particle.x);
				data.pushBack(particle.y);
				data.pushBack(particle.z);
			}
		}
	}

	const int strandsOnHair = counter;
	for (; counter < maximumStrandCount; ++counter) 
	{
		int randomNumber = glm::linearRand(0, strandsOnHair - 1) * particlesPerStrand * 3;
		glm::vec3 firstCoords(data[randomNumber], data[randomNumber + 1], data[randomNumber + 2]);
		randomNumber += particlesPerStrand * 3;
		glm::vec3 secondCoords(data[randomNumber], data[randomNumber + 1], data[randomNumber + 2]);
		glm::vec3 coordsBetween = secondCoords + (firstCoords - secondCoords) * 0.5f;
		for (uint32_t j = 0; j < particlesPerStrand; ++j)
		{
			glm::vec3 particle = coordsBetween + glm::normalize(coordsBetween) * (float)j * segmentLength;
			data.pushBack(particle.x);
			data.pushBack(particle.y);
			data.pushBack(particle.z);
		}
	}

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo);

	// Velocities
	data.clear();
	data.reserve(maximumStrandCount * particlesPerStrand * 3);
	for (uint32_t i = 0; i < maximumStrandCount * particlesPerStrand * 3; ++i)
		data.pushBack(0.f);

	glGenBuffers(1, &velocityArrayBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityArrayBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityArrayBuffer);

	// Indices
	data.clear();
	data.reserve(maximumStrandCount * particlesPerStrand);
	for (uint32_t i = 0; i < maximumStrandCount * particlesPerStrand; ++i)
		data.pushBack(i);

	glGenBuffers(1, &indexArrayBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, indexArrayBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, indexArrayBuffer);



	vGridSize *= 3;	// 3 floats per voxel so that we can store velocity in the same buffer
	glGenBuffers(1, &volumeVelocities);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, volumeVelocities);
	glBufferData(GL_SHADER_STORAGE_BUFFER, vGridSize, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, volumeVelocities);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, GL_NONE);
}

void Hair::setGravity(float strength)
{
	gravity = strength;
	parameterSet = true;
}

void Hair::increaseStrandCount()
{
	strandCount = glm::clamp<int>(strandCount + 100, 0, maximumStrandCount);
	parameterSet = true;
	std::cout << "Strand count: " << strandCount << '\n';
}

void Hair::decreaseStrandCount()
{
	strandCount = glm::clamp<int>(strandCount - 100, 0, maximumStrandCount);
	parameterSet = true;
	std::cout << "Strand count: " << strandCount << '\n';
}

void Hair::increaseVelocityDamping()
{
	vdampCoeffic = glm::clamp(vdampCoeffic + 0.01f, 0.f, 1.f);
	parameterSet = true;
	std::cout << "Velocity damping coefficient: " << vdampCoeffic << '\n';
}

void Hair::decreaseVelocityDamping()
{
	vdampCoeffic = glm::clamp(vdampCoeffic - 0.01f, 0.f, 1.f);
	parameterSet = true;
	std::cout << "Velocity damping coefficient: " << vdampCoeffic << '\n';
}

void Hair::increaseCurlRadius()
{
	curlRadius = glm::clamp(curlRadius + 0.001f, 0.f, 0.05f);
	strandWidth = curlRadius * 50.f;
}

void Hair::decreaseCurlRadius()
{
	curlRadius = glm::clamp(curlRadius - 0.001f, 0.f, 0.05f);
	strandWidth = curlRadius * 50.f;
}

void Hair::setWind(const glm::vec3& direction, float strength)
{
	wind = glm::vec4(direction.x, direction.y, direction.z, glm::clamp(strength, 0.f, 1.f));
	parameterSet = true;
}

void Hair::setFrictionFactor(float friction)
{
	frictionFactor = glm::clamp<float>(friction, 0.f, 1.f);
	parameterSet = true;
	std::cout << "Friction factor: " << frictionFactor << std::endl;
}

void Hair::setStiffness(float stiffness)
{
	stiffnessFactor = glm::clamp<float>(stiffness, 0.f, 1.f);
	parameterSet = true;
	std::cout << "Stiffness factor: " << stiffnessFactor << std::endl;
}


void Hair::applyPhysics(float deltaTime, float runningTime) 
{ 
	// Apply a curling force to particles
    float angle = 2.f * glm::pi<float>() * runningTime; // 2pi radians per second
    glm::vec3 axis = glm::normalize(glm::vec3(0.f, 1.f, 0.f)); // creating a curling force around the y-axis

	// Apply a curling force to hair stands by applying a force to each particle in the strand
    for (uint32_t i = 0; i < particlesPerStrand; i++) {
        glm::vec3& velocity = velocities[i];
        glm::vec3& position = particles[i];

        // Compute the perpendicular component of the velocity so that we can apply a force perpendicular to the velocity
        glm::vec3 perp = velocity - glm::dot(velocity, axis) * axis;

        // Apply a force proportional to the perpendicular velocity
        glm::vec3 force = curlRadius * glm::length(perp) * glm::cross(perp, axis);
        velocity += force * deltaTime;
    }
}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, volumeDensities);
	int* densities = (int*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	uint32_t volumeSize = 11 * 11 * 11;
	for (uint32_t i = 0; i < volumeSize; ++i)
	{
		densities[i] = 0;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, volumeVelocities);
	int* velocities = (int*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

	for (uint32_t i = 0; i < volumeSize * 3; i++)
	{
		velocities[i] = 0;	
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, GL_NONE);

	drawAfro.use();
	if (parameterSet)
	{
		drawAfro.setFloat("force.gravity", gravity);
		drawAfro.setVec4("force.wind", wind);
		drawAfro.setFloat("frictionCoeffic", frictionFactor);
		drawAfro.setFloat("vdampCoeffic", vdampCoeffic);
		parameterSet = false;
	}

	for (uint32_t i = 0; i < ellipsoids.size(); ++i)
	{
		drawAfro.setMat4("ellipsoids[" + std::to_string(i) + "]", transformMatrix * ellipsoids[i]->getTransformMatrix());
	}

	// this is for the hair strands only when they are not attached to ellipsoids and are free to move.
	drawAfro.setMat4("model", transformMatrix);
	drawAfro.setUint("hairData.strandCount", strandCount);
	drawAfro.setFloat("hairData.strandLength", strandLength);
	drawAfro.setFloat("hairData.strandWidth", strandWidth);
	drawAfro.setFloat("hairData.curlRadius", curlRadius);
	drawAfro.setFloat("hairData.stiffnessFactor", stiffnessFactor);
	drawAfro.setFloat("hairData.frictionFactor", frictionFactor);
	drawAfro.setUint("state", 0);
	GLuint locgroupCountX = drawAfro.getLocalWorkGroupsCount().x;
	GLuint globalWorkGroupCount = strandCount / locgroupCountX;
	if (strandCount % locgroupCountX != 0)
	{
		globalWorkGroupCount += 1;
	}



	drawAfro.setglobgroupworkCount(globalWorkGroupCount);
	drawAfro.dispatch();
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	globalWorkGroupCount = strandCount * particlesPerStrand / locgroupCountX;
	if ((strandCount * particlesPerStrand) % locgroupCountX != 0)
	{
		globalWorkGroupCount += 1;
	}
	drawAfro.setglobgroupworkCount(globalWorkGroupCount);
	drawAfro.setUint("state", 1);
	drawAfro.dispatch();
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	drawAfro.setUint("state", 2);
	drawAfro.dispatch();
}