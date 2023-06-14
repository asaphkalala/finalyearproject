#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	sampler2D diffuseMap;
};

struct Light {
	vec3 position;
	vec3 color;
	float constant;
	float linear;
	float quadratic;
};

in Attributes {
	vec3 fragPosition;
	vec3 normal;
	vec2 texCoords;
} attributeIN;

uniform bool tex = false;
uniform Light light;
uniform vec3 eyePosition;
uniform Material material;

vec4 calculatePointLight(vec4 colorDiffuse, vec4 colorSpecular, vec4 colorAmbient, vec3 eyeDirection, Light light) {
	vec3 lightDirection = normalize(light.position - attributeIN.fragPosition);
	vec3 normal = normalize(attributeIN.normal);
	vec3 reflectDirection = reflect(-lightDirection, normal);

	float distance = length(light.position - attributeIN.fragPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	float diffuseFactor = max(dot(normal, lightDirection), 0.0);
	vec4 diffuse = colorDiffuse * diffuseFactor;

	float specularFactor = pow(max(dot(eyeDirection, reflectDirection), 0.0), material.shininess);
	vec4 specular = colorSpecular * specularFactor;

	vec4 ambient = colorAmbient;

	diffuse *= attenuation;
	specular *= attenuation;

	return diffuse + specular + ambient;
}

void main() { // TODO: add ambient light
	vec3 eyeDirection = normalize(eyePosition - attributeIN.fragPosition);
	vec4 colorDiffuse = vec4(material.diffuse, 1.0);
	vec4 colorSpecular = vec4(material.specular, 1.0);
	vec4 colorAmbient = vec4(material.ambient, 1.0);

	vec4 color = calculatePointLight(colorDiffuse, colorSpecular, colorAmbient, eyeDirection, light);

	if (tex) {
		color *= texture(material.diffuseMap, attributeIN.texCoords);
	}

	gl_FragColor = color;
}

