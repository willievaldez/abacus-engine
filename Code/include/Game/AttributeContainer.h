#pragma once

#include <string>
#include <unordered_map>
#include <glad/glad.h> // gl function calls
#include <GLFW/glfw3.h> // openGL calls (GLFW), GLint, GLuint

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
	class AttributeWrapper
	{
	public:
		virtual void SetAttribute(const std::string& rawVal) = 0;
	};

	template<typename T>
	class TemplatedAttributeWrapper : public AttributeWrapper
	{
	public:
		TemplatedAttributeWrapper(T* data) : m_data(data) {};

		void SetAttribute(const std::string& rawVal) override;
	private:
		T* m_data;
	};

	std::unordered_map<std::string, AttributeWrapper*> m_map;
};

#define SET_ATTRIBUTE_IMPL(TYPE) void AttributeContainer::TemplatedAttributeWrapper<TYPE>::SetAttribute(const std::string& rawVal)