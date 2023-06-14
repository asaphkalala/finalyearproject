#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texCoords;

out Attributes {
	vec3 position;
	vec3 normal;
	vec2 texCoords;
} attributeOUT;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
	mat3 normalMatrix;
	attributeOUT.position = vec3(model * vec4(in_position, 1.0));
	if (model[0][0] != model[1][1] || model[0][0] != model[2][2] || model[1][1] != model[2][2]) 
	{
		normalMatrix = mat3(transpose(inverse(model)));
		attributeOUT.normal = normalize(normalMatrix * in_normal);
	}
	else
	{
		normalMatrix = mat3(model);
		attributeOUT.normal = normalize(normalMatrix * in_normal);
	}

	attributeOUT.texCoords = in_texCoords;
	gl_Position = projection * view * vec4(attributeOUT.position, 1.0);
}