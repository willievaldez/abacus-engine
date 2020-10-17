#version 330 core
out vec4 FragColor;

in vec3 fragVert;
in vec3 fragNormal;
in vec2 TexCoords;

uniform bool usesTexture;
uniform sampler2D texture_diffuse1;
uniform vec3 colorOverride;

struct PointLight
{
  vec3 pos;
  float intensity;
  float radius;
};

uniform int numLights;
uniform PointLight lights[128];

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
		color = vec4(colorOverride, 1.0f);
	}

	float attenuation = 0.0f;
	for(int i = 0; i < numLights; i++)
	{
		PointLight light = lights[i];
		float distToLight = distance(fragVert, light.pos);

		// binary attenuation
		//if (distToLight < light.radius)
		//{
		//	attenuation += light.intensity;
		//}

		// logarithmic attenuation
		float logFactor = 50.0f;
		float withinRadius = logFactor - (logFactor * distToLight / light.radius);
		if (withinRadius > 0.0f)
		{
			float logAttenuation = (log2(withinRadius)) / (log2(logFactor));
			if (logAttenuation < 0.0f)
			{
				logAttenuation = 0.0f;
			}
			if (logAttenuation * light.intensity > attenuation)
			{
				attenuation = logAttenuation * light.intensity;
			}
		}


		// linear attenuation
		//float attenuation = 1.0f - (distToLight/15.0f);
		//if (attenuation < 0.0f)
		//{
		//	attenuation = 0.0f;
		//}

		// quadratic attenuation
		//float attenuation = (pow(distToLight, 4) / -10000.0f) + 1;

    }
	if (attenuation > 1.0f)
	{
		attenuation = 1.0f;
	}

	FragColor = color * attenuation;
	FragColor.a = color.a; // maintain alpha value
} 