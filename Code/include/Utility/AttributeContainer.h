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
	// used in the general-purpose utility func FromString and ToString
	static bool ParseFromString(T* data, const std::string& rawVal);
	static std::string ToString(const T& data);
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
bool FromString(T* data, const std::string& rawVal)
{
	return TemplatedAttributeWrapper<T>::ParseFromString(data, rawVal);
};

template<typename T>
std::string ToString(const T& data)
{
	return TemplatedAttributeWrapper<T>::ToString(data);
};

#define FROM_STRING_IMPL(TYPE) bool TemplatedAttributeWrapper<TYPE>::ParseFromString(TYPE* m_data, const std::string& rawVal)
#define TO_STRING_IMPL(TYPE) std::string TemplatedAttributeWrapper<TYPE>::ToString(const TYPE& data)