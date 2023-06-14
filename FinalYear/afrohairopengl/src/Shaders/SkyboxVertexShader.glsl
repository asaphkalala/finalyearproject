#version 330 core

layout (location = 0) in vec3 inPosition; // The position variable has attribute position 0

out vec3 direction;

uniform mat4 projection; // Projection matrix 
uniform mat4 view;

void main() // The entry point for our vertex shader 
{
	direction = inPosition;
	vec4 pos = projection * mat4(mat3(view)) * vec4(inPosition, 1.0); // Use the position variable in the vertex shader as an input vertex attribute. A vertex attribute is a 1D vector of any type.
	gl_Position = pos.xyww;
}