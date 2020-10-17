#pragma once

#include <string>
#include <Utility/AttributeContainer.h>

#define INSTALL_DIR std::string("../../")

enum class MovementType {
	Keyboard,
	PointAndClick,
	Controller
};

enum class FrustumType {
	Perspective,
	Orthographic
};

struct Config
{
	bool loaded = false;

#define ConfigKey(type, varName, defaultVal) type varName = defaultVal;
#include <Utility/ConfigKeys.inl>
#undef ConfigKey

	AttributeContainer GetExpectedAttributes()
	{
		AttributeContainer attributeContainer;
#define ConfigKey(type, varName, defaultVal) attributeContainer.AddAttribute(#varName, &varName)
#include <Utility/ConfigKeys.inl>
#undef ConfigKey
		return attributeContainer;
	};
};

const Config& GetConfig();
