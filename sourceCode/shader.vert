#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 texCoords;

out vec2 fragTexCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	fragTexCoords = texCoords;

	gl_Position = projection * view * model * vec4(vertex, 1.0f);
}