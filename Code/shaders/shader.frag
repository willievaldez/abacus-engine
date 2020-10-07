#version 330 core
out vec4 FragColor;

in vec3 fragVert;
in vec3 fragNormal;
in vec2 TexCoords;

uniform bool usesTexture;
uniform sampler2D texture_diffuse1;
uniform vec3 cameraPos;
uniform vec3 colorOverride;

void main()
{
	vec4 color;
	if (usesTexture)
	{
		color = texture(texture_diffuse1, TexCoords);
	}
	else
	{
		// if there are no textures loaded for the fragment, load the objects color
		//vec3 shiftedFragNormal = fragNormal;
		//shiftedFragNormal = shiftedFragNormal + vec3(1.0f, 1.0f, 1.0f);
		//shiftedFragNormal = shiftedFragNormal / 2.0f;
		//color = vec4(shiftedFragNormal, 1.0f);
		color = vec4(colorOverride, 1.0f);
	}

	//color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	//this block calculates the diffuse color
	//vec3 diffuseColor;
	//vec3 lightDir = vec3(0.0f, -1.0f, 0.0f); // straight down
	//lightDir = lightDir * -1.0f; // invert to get the math right (angle from surface to light)
	//float diff = max(dot(fragNormal, lightDir), 0.1f);
	//diffuseColor = vec3(color) * diff;

	//this block calculates the spec color
	//vec3 specColor;
	//vec3 viewDir = normalize(cameraPos - fragVert);
	//vec3 reflectDir = reflect(-lightDir, fragNormal);
	//float specul = pow(max(dot(viewDir, reflectDir), 0.0), 16) * 0.05f;
	//specColor = vec3(specul, specul, specul);


	//FragColor = vec4(diffuseColor + specColor, 1.0f);

	// attenuation
	float distToLight = distance(fragVert, cameraPos);

	// linear attenuation
	//float attenuation = 1.0f - (distToLight/15.0f);
	//if (attenuation < 0.0f)
	//{
	//	attenuation = 0.0f;
	//}

	// quadratic attenuation
	float attenuation = (pow(distToLight, 4) / -10000.0f) + 1;

	FragColor = color * attenuation;
	FragColor.a = color.a; // maintain alpha value

} 