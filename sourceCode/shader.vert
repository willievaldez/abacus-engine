#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 fragVert;
out vec3 fragNormal;
out vec2 TexCoords;
out vec3 fragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	fragColor = normal;
    gl_Position = projection * view * model * vec4(vertex, 1.0f);
}