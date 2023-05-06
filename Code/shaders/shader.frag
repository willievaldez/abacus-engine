#version 330 core
out vec4 FragColor;

in vec3 fragVert;
in vec3 fragNormal;
in vec2 TexCoords;

uniform vec3 debugHighlight;

uniform bool usesTexture;
uniform sampler2D texture_diffuse1;
uniform vec3 colorOverride;

uniform float logFactor;
uniform int attenuationType;

struct PointLight
{
  vec3 pos;
  float intensity;
  float radius;
  float ambientRadius;
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

	float attenuation = 1.0f;
	for(int i = 0; i < numLights; i++)
	{
		PointLight light = lights[i];
		float distToLight = distance(fragVert, light.pos);

		if (attenuationType == 0)
		{
			// binary attenuation
			if (distToLight < light.radius)
			{
				attenuation += light.intensity;
			}
		}
		else if (attenuationType == 1)
		{
			// linear attenuation
			float linearAttenuation = 1.0f - (distToLight/light.radius);
			if (linearAttenuation < 0.0f)
			{
				linearAttenuation = 0.0f;
			}
			attenuation += linearAttenuation;
		}
		else if (attenuationType == 2)
		{
			// logarithmic attenuation
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
		}

		if (attenuation <= 0.0f && light.ambientRadius > 0.0f && distToLight < (light.radius+light.ambientRadius))
		{
			if (usesTexture)
			{
				// logarithmic attenuation
				float withinRadius = logFactor - (logFactor * distToLight / (light.radius+light.ambientRadius));
				if (withinRadius > 0.0f)
				{
					float logAttenuation = (log2(withinRadius)) / (log2(logFactor)) * 0.05f;
					if (logAttenuation < 0.0f)
					{
						logAttenuation = 0.0f;
					}
					if (logAttenuation * light.intensity > attenuation)
					{
						attenuation = logAttenuation * light.intensity;
					}
				}
			}
		}
    }
	if (attenuation > 1.0f)
	{
		attenuation = 1.0f;
	}

	FragColor = (color * attenuation) + vec4(debugHighlight, 0.0);
	FragColor.a = color.a; // maintain alpha value
} 