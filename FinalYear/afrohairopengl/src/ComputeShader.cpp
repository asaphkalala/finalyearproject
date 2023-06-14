#include "ComputeShader.h"
#include <iostream>

ComputeShader::ComputeShader(const std::string& shaderFile)
{
	GLuint shaderID = glCreateShader(GL_COMPUTE_SHADER);
	attachShader(shaderFile, shaderID);
	linkProgram();
	glDeleteShader(shaderID);
}

void ComputeShader::dispatch() const
{
	glDispatchCompute(globalGroupX, globalGroupY, globalGroupZ);
}

glm::ivec3 ComputeShader::getMaxLocalWorkGroups() const
{
	glm::ivec3 values; // Get the maximum local work group size
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &values.x);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &values.y);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &values.z);
	return values;
}

glm::ivec3 ComputeShader::getLocalWorkGroupsCount() const
{
	glm::ivec3 values; // Get the local work group count
	glGetProgramiv(programID, GL_COMPUTE_WORK_GROUP_SIZE, &values.x);
	return values;
}

glm::ivec3 ComputeShader::getMaxGlobalWorkGroups() const
{
	glm::ivec3 values; // Get the maximum global work group count
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &values.x);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &values.y);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &values.z);
	return values;
}

GLuint ComputeShader::getMaxWorkGroupInvocations() const
{
	GLint value;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &value);
	return value;
}

void ComputeShader::setglobgroupworkCount(GLuint groupX, GLuint groupY, GLuint groupZ)
{
	if (groupX != 0)
		globalGroupX = groupX;

	if (groupY != 0)
		globalGroupY = groupY;

	if (groupZ != 0)
		globalGroupZ = groupZ;
}
