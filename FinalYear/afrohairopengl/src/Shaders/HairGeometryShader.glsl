#version 460 core
#define VERTICES_BETWEEN 10

// vertex data different for all executions of this shader.

layout (lines) in;
layout(line_strip, max_vertices = VERTICES_BETWEEN) out;

in Attributes { 
	vec3 fragPosition;
	vec3 tangent;
} attributeIN[];

out Attributes {
	vec3 fragPosition;
	vec3 tangent;
} attributeOUT;

uniform mat4 projection;
uniform mat4 view;
uniform float curlRadius = 0.05f; 

void main(void)
{
	if (curlRadius != 0.f) // if curlRadius is 0, drawing a straight line
	{
		const float segmentLength = length(gl_in[0].gl_Position - gl_in[1].gl_Position) / (VERTICES_BETWEEN - 1); // length of a segment between two vertices
		
		// starting and ending position of the hair
		const vec3 startPosition = gl_in[0].gl_Position.xyz; 
		const vec3 endPosition = gl_in[1].gl_Position.xyz; 
		
		const vec3 hairDirection = normalize(endPosition - startPosition); // direction of the hair from start to end
		
		const vec3 uDirection = normalize(cross(vec3(hairDirection), vec3(0.f, 1.f, 0.f))); // direction of the hair in the plane
		const vec3 vDirection = normalize(cross(vec3(hairDirection), uDirection));
		float angleStep = 360.f / (VERTICES_BETWEEN-1); // angle step between two vertices

		uint i = 0; // index of the vertex
		vec3 nextPosition; // position of the next vertex
		nextPosition = startPosition + hairDirection * segmentLength * i; 
		nextPosition += curlRadius * uDirection * cos(radians(angleStep) * i); // curling the hair
		nextPosition += curlRadius * vDirection * sin(radians(angleStep) * i); // curling the hair

		for (i = 1; i <= VERTICES_BETWEEN; ++i) // drawing the hair
		{
			attributeOUT.fragPosition = nextPosition;
			gl_Position = vec4(nextPosition, 1.f);
			gl_Position = projection * view * gl_Position; // transforming the vertex to the screen space
			
			nextPosition = startPosition + hairDirection * segmentLength * i; // calculating the next vertex
			nextPosition += curlRadius * uDirection * cos(radians(angleStep) * i); // curling the hair
			nextPosition += curlRadius * vDirection * sin(radians(angleStep) * i); // curling the hair
			
			attributeOUT.tangent = normalize(nextPosition - attributeOUT.fragPosition); // calculating the tangent

			EmitVertex();
		}

		gl_Position = vec4(nextPosition, 1.f); // drawing the last vertex
		gl_Position = projection * view * gl_Position;
		attributeOUT.fragPosition = nextPosition;
		EmitVertex();
		EndPrimitive(); 
	} 
	else  // drawing a straight line
	{
		gl_Position = projection * view * vec4(gl_in[0].gl_Position.xyz, 1.f);
		attributeOUT.fragPosition = attributeIN[0].fragPosition;
		attributeOUT.tangent = normalize(attributeIN[1].fragPosition - attributeIN[0].fragPosition); // calculating the tangent of the line
		EmitVertex();

		gl_Position = projection * view * vec4(gl_in[1].gl_Position.xyz, 1.f);
		attributeOUT.fragPosition = attributeIN[1].fragPosition;
		attributeOUT.tangent = normalize(attributeIN[1].fragPosition - attributeIN[0].fragPosition); 
		EmitVertex();

		EndPrimitive();
	}
}



