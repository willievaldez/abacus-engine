#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 texCoords;

out vec4 fragColor;
out vec2 fragTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 color;

void main()
{
	fragTexCoords = texCoords;
	fragColor = vec4(color, 1.0f);

	gl_Position = projection * view * model * vec4(vertex, 1.0f);
}