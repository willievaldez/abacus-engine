#include <GLWrapper/UniformContainer.h>
#include <GLWrapper/Asset.h>

#include <glm/vec3.hpp> // vec3
#include <glm/gtc/matrix_transform.hpp> // glm::mat4, glm::transform, glm::length

#define SET_UNIFORM_IMPL(TYPE) void UniformContainer::TemplatedUniformWrapper<TYPE>::SetUniformImpl(const std::string& uniformName, const TYPE& val)

void UniformContainer::SetUniforms() const
{
	for (const auto& obj : m_vector)
	{
		obj->SetUniform();
	}
}

// template specializations

SET_UNIFORM_IMPL(glm::mat4)
{
	GLuint uniformIntId = glGetUniformLocation(Asset::shaderProgram, uniformName.c_str());
	glUniformMatrix4fv(uniformIntId, 1, GL_FALSE, &val[0][0]);
}

SET_UNIFORM_IMPL(glm::vec3)
{
	GLuint uniformVec3Id = glGetUniformLocation(Asset::shaderProgram, uniformName.c_str());
	glUniform3fv(uniformVec3Id, 1, &val[0]);
}

SET_UNIFORM_IMPL(int)
{
	GLuint uniformId = glGetUniformLocation(Asset::shaderProgram, uniformName.c_str());
	glUniform1i(uniformId, val);
}

SET_UNIFORM_IMPL(float)
{
	GLuint uniformFltId = glGetUniformLocation(Asset::shaderProgram, uniformName.c_str());
	glUniform1f(uniformFltId, val);
}

SET_UNIFORM_IMPL(bool)
{
	int isTrue = val ? 1 : 0;
	GLuint uniformBoolId = glGetUniformLocation(Asset::shaderProgram, uniformName.c_str());
	glUniform1i(uniformBoolId, isTrue);
}

#undef SET_UNIFORM_IMPL