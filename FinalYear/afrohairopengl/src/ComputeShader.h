#pragma once
#include "Shader.h"

class ComputeShader : public Shader { // Class to represent a compute shader
public:
	ComputeShader(const std::string& shaderFile);
	~ComputeShader() override = default;
	void dispatch() const;
	void dispatch(GLuint groupX, GLuint groupY, GLuint groupZ) const; // Method to dispatch the compute shader

	// Method to get the maximum local work groups
	glm::ivec3 getMaxLocalWorkGroups() const; 
	glm::ivec3 getLocalWorkGroupsCount() const;
	glm::ivec3 getMaxGlobalWorkGroups() const;
	GLuint getMaxWorkGroupInvocations() const;
	void setglobgroupworkCount(GLuint groupX = 1, GLuint groupY = 1, GLuint groupZ = 1); // Method to set the global work group count

private:// Global work group count for the compute shader so that it can be set from outside the class.
	GLuint globalGroupX = 1; 
	GLuint globalGroupY = 1;
	GLuint globalGroupZ = 1;
};