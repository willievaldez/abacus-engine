#pragma once

#include <string>
#include <vector>
#include <glad/glad.h> // gl function calls
#include <GLFW/glfw3.h> // openGL calls (GLFW), GLint, GLuint

class UniformContainer
{
public:
	template<typename T>
	void AddObject(const std::string& uniformName, const T& obj)
	{
		m_vector.push_back(new TemplatedUniformWrapper<T>(uniformName, obj));
	};

	void SetUniforms() const;

	template<typename T>
	static void SetUniform(const std::string& uniformName, const T& val)
	{
		TemplatedUniformWrapper<T>::SetUniformImpl(uniformName, val);
	};

private:
	class UniformWrapper
	{
	public:
		UniformWrapper(const std::string& uniformName) : m_uniformName(uniformName) {};
		virtual void SetUniform() const = 0;
	protected:
		std::string m_uniformName;
	};

	template<typename T>
	class TemplatedUniformWrapper : public UniformWrapper
	{
	public:
		TemplatedUniformWrapper(const std::string& uniformName, const T& data)
			: UniformWrapper(uniformName)
			, m_data(data) {};

		void SetUniform() const override
		{
			SetUniformImpl(m_uniformName, m_data);
		};
	private:
		friend class UniformContainer;
		static void SetUniformImpl(const std::string& uniformName, const T& val);

		const T& m_data;
	};

	std::vector<UniformWrapper*> m_vector;
};