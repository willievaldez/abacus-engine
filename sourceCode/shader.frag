#version 330 core
out vec4 FragColor;

in vec2 fragTexCoords;

uniform sampler2D tex;


void main()
{
	if (fragTexCoords.x != 0.0f || fragTexCoords.y != 0.0f)
	{
		FragColor = texture(tex, fragTexCoords);
	}
	else
	{
		FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	
} 