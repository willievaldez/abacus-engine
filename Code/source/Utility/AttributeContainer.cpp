#include <Utility/AttributeContainer.h>
#include <algorithm>

void AttributeContainer::SetAttribute(const std::string& key, const std::string& val)
{
	auto foundAttr = m_map.find(key);
	if (foundAttr != m_map.end())
	{
		foundAttr->second->SetAttribute(val);
	}
	else
	{
		printf("Unknown Attribute: %s with value %s\n", key.c_str(), val.c_str());
	}
}

SET_ATTRIBUTE_IMPL(std::string)
{
	*m_data = rawVal;
}

SET_ATTRIBUTE_IMPL(int)
{
	*m_data = std::stoi(rawVal);
}

SET_ATTRIBUTE_IMPL(float)
{
	*m_data = std::stof(rawVal);
}

SET_ATTRIBUTE_IMPL(bool)
{
	std::string lowerCase = rawVal;
	std::transform(rawVal.begin(),
		rawVal.end(),
		lowerCase.begin(),
		::tolower);

	*m_data = lowerCase == "true" ? true : false;
}