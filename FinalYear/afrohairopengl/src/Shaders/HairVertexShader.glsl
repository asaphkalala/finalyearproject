#version 460 core

layout (location = 0) in vec3 inPosition;

out Attributes {
	vec3 fragPosition;
	vec3 tangent;
} attributeOUT;

uniform mat4 model;
uniform uint particlesPerStrand;

void main() // needs to be more efficient
if (gl_VertexID % particlesPerStrand == 0)
{
	vec4 transformedPosition = model * vec4(inPosition, 1.f);
	attributeOUT.fragPosition = vec3(transformedPosition);
	gl_Position = vec4(attributeOUT.fragPosition, transformedPosition.w);
}
else
{
	attributeOUT.fragPosition = inPosition;
	gl_Position = vec4(attributeOUT.fragPosition, 1.f);
}

attributeOUT.tangent = normalize(vec3(model * vec4(1.f, 0.f, 0.f, 0.f)));
