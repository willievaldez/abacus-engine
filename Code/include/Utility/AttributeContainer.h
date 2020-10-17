#pragma once

#include <string>
#include <unordered_map>

class AttributeWrapper
{
protected:
	friend class AttributeContainer;

	virtual void SetAttribute(const std::string& rawVal) = 0;
};

template<typename T>
class TemplatedAttributeWrapper : public AttributeWrapper
{
public:
	TemplatedAttributeWrapper(T* data) : m_data(data) {};

	// the parser is abstracted to a static function to be
	// used in the general-purpose utility func FromString
	static void ParseFromString(T* data, const std::string& rawVal);
protected:

	void SetAttribute(const std::string& rawVal) override
	{
		ParseFromString(m_data, rawVal);
	};

private:
	T* m_data;
};

class AttributeContainer
{
public:
	template<typename T>
	void AddAttribute(const std::string& attributeName, T* obj)
	{
		m_map[attributeName] = new TemplatedAttributeWrapper<T>(obj);
	};

	void SetAttribute(const std::string& key, const std::string& val);


private:
	std::unordered_map<std::string, AttributeWrapper*> m_map;
};

// utility function to use the parse function for any data type
template<typename T>
void FromString(T* data, const std::string& rawVal)
{
	TemplatedAttributeWrapper<T>::ParseFromString(data, rawVal);
};

#define SET_ATTRIBUTE_IMPL(TYPE) void TemplatedAttributeWrapper<TYPE>::ParseFromString(TYPE* m_data, const std::string& rawVal)