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

FROM_STRING_IMPL(std::string)
{
	*m_data = rawVal;
	return true;
}

FROM_STRING_IMPL(int)
{
	*m_data = std::stoi(rawVal);
	return true;
}

FROM_STRING_IMPL(float)
{
	*m_data = std::stof(rawVal);
	return true;
}

FROM_STRING_IMPL(bool)
{
	std::string lowerCase = rawVal;
	std::transform(rawVal.begin(),
		rawVal.end(),
		lowerCase.begin(),
		::tolower);

	*m_data = lowerCase == "true" ? true : false;
	return true;
}

TO_STRING_IMPL(bool)
{
	return data ? "true" : "false";
}
