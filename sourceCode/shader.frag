#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
	float opacity;
};

uniform Material material;


in vec3 fragVert;
in vec3 fragNormal;
in vec2 TexCoords;
in vec3 fragColor;

void main()
{
    FragColor = vec4(fragColor, 1.0f);
} 