#version 330 core
out vec4 FragColor;

in vec4 fragColor;
in vec2 fragTexCoords;

uniform sampler2D tex;
uniform bool useTex;


void main()
{
	if (useTex)
	{
		FragColor = texture(tex, fragTexCoords);
	}
	else
	{
	    FragColor = fragColor;
	}
} 