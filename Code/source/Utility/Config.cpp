#include <Utility/Config.h>

#include <fstream>
#include <sstream>
#include <algorithm>

const Config& GetConfig()
{
	static Config config;

	if (!config.loaded)
	{
		AttributeContainer attributeContainer = config.GetExpectedAttributes();

		// load the config
		std::string line;
		std::ifstream myfile(INSTALL_DIR + "Assets/config.txt");
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				std::stringstream lineStream(line);
				std::string key, val;
				getline(lineStream, key, '=');
				getline(lineStream, val, '=');
				attributeContainer.SetAttribute(key, val);
			}

			config.loaded = true;
		}
	}

	return config;
}

SET_ATTRIBUTE_IMPL(MovementType)
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
	}
}

SET_ATTRIBUTE_IMPL(FrustumType)
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
	}
}