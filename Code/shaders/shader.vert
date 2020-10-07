#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec3 fragVert;
out vec3 fragNormal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// test variables for animation
uniform int animationWidth;
uniform int animationHeight;
uniform int animationFrame;

void main()
{
	fragVert = vec3(model * vec4(aPos, 1.0f));
	fragNormal = normalize(mat3(transpose(inverse(model))) * aNormal);
	TexCoords = aTexCoords;
	float frameOffset = float(animationHeight) / float(animationWidth);
	TexCoords.x = TexCoords.x * frameOffset; // frame zero
	TexCoords.x = TexCoords.x + (frameOffset * float(animationFrame)); // frame x

	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}