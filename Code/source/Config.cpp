#include <Config.h>

#include <fstream>
#include <sstream>
#include <algorithm>

bool ParseBool(const std::string& val)
{
	if (std::tolower(val[0]) == 't')
	{
		return true;
	}
	else
	{
		return false;
	}
}

const Config& GetConfig()
{
	static Config config;

	if (!config.loaded)
	{
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

				if (key == "is3D")
				{
					config.is3D = ParseBool(val);
				}
				else if (key == "tileSize")
				{
					config.tileSize = std::stof(val);
				}
				else if (key == "windowWidth")
				{
					config.windowWidth = std::stoi(val);
				}
				else if (key == "windowHeight")
				{
					config.windowHeight = std::stoi(val);
				}
				else if (key == "useSteam")
				{
					config.useSteam = ParseBool(val);
				}
				else if (key == "useVR")
				{
					config.useVR = ParseBool(val);
					config.is3D |= config.useVR;
				}
				else if (key == "level")
				{
					config.level = val;
				}
				else
				{
					printf("unrecognized config: %s", key.c_str());
				}
			}

			config.loaded = true;
		}
	}
	// Code to load and set the configuration variables
	return config;
}