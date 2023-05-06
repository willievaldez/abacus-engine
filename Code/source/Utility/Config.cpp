#include <Utility/Config.h>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>

Config& GetConfig(const std::string& fileName)
{
	static std::unordered_map<std::string, Config> configMap;

	auto foundConfig = configMap.find(fileName);
	if (foundConfig == configMap.end())
	{
		foundConfig = configMap.emplace(fileName, Config()).first;
	}

	Config& config = foundConfig->second;

	if (!config.loaded)
	{
		printf("loading config %s\n", fileName.c_str());
		AttributeContainer attributeContainer = config.GetExpectedAttributes();

		// load the config
		std::string line;
		std::ifstream myfile(INSTALL_DIR + "Assets/" + fileName + ".cfg");
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				std::stringstream lineStream(line);
				std::string key, val;
				getline(lineStream, key, '=');
				getline(lineStream, val, '=');
				printf("%s = %s\n", key.c_str(), val.c_str());
				attributeContainer.SetAttribute(key, val);
			}

			config.loaded = true;
		}
	}

	return config;
}

FROM_STRING_IMPL(MovementType)
{
	if (rawVal == "Keyboard")
	{
		*m_data = MovementType::Keyboard;
	}
	else if (rawVal == "PointAndClick")
	{
		*m_data = MovementType::PointAndClick;
	}
	else if(rawVal == "Controller")
	{
		*m_data = MovementType::Controller;
	}
	else
	{
		printf("Unknown MovementType: %s\n", rawVal.c_str());
		return false;
	}

	return true;
}

FROM_STRING_IMPL(FrustumType)
{
	if (rawVal == "Perspective")
	{
		*m_data = FrustumType::Perspective;
	}
	else if (rawVal == "Orthographic")
	{
		*m_data = FrustumType::Orthographic;
	}
	else
	{
		printf("Unknown MovementType: %s\n", rawVal.c_str());
		return false;
	}

	return true;
}

FROM_STRING_IMPL(CameraBehavior)
{
	static const char* s_cameraBehaviors[] = {
		"Static",
		"PlayerCentered"
	};

	for (int i = 0; i < sizeof(s_cameraBehaviors) / sizeof(char*); ++i)
	{
		if (rawVal == s_cameraBehaviors[i])
		{
			*m_data = (CameraBehavior)i;
			return true;
		}
	}

	printf("Unknown CameraBehavior: %s\n", rawVal.c_str());
	return false;
}

FROM_STRING_IMPL(AttackTargeting)
{
	static const char* s_attackTargets[] = {
		"Forward",
		"Mouse"
	};

	for (int i = 0; i < sizeof(s_attackTargets) / sizeof(char*); ++i)
	{
		if (rawVal == s_attackTargets[i])
		{
			*m_data = (AttackTargeting)i;
			return true;
		}
	}

	printf("Unknown CameraBehavior: %s\n", rawVal.c_str());
	return false;
}