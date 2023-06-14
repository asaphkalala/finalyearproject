#version 330 core
// Vertex shader
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};


in Attributes {
	vec3 fragPosition;
	vec3 tangent;
} attributeIN;

uniform Light light;
uniform vec3 eyePosition;
uniform Material material;

float attenuation(in Light light) 
{
	float distance = length(light.position - attributeIN.fragPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance * light.quadratic * distance * distance);
	return attenuation;
}

vec4 calculatePointLight() 
{
	// ambient
	vec3 ambientComponent = material.ambient; 

	// diffuse
	vec3 lightDirection = normalize(light.position - attributeIN.fragPosition);
	float lightAngle = acos(abs(dot(lightDirection, attributeIN.tangent)));
	vec3 diffuseComponent = material.diffuse * light.color * sin(lightAngle);

	// specular
	vec3 eyeDirection = normalize(eyePosition - attributeIN.fragPosition);
	float eyeAngle = acos(abs(dot(eyeDirection, attributeIN.tangent)));
	vec3 specularComponent = material.specular * light.color * pow(cos(lightAngle - eyeAngle), material.shininess);

	vec4 result = vec4((ambientComponent + diffuseComponent + specularComponent) * attenuation(light), 1.f);

	return result;
}

void main() 
{
	gl_FragColor = calculatePointLight();
}