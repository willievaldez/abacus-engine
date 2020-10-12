#include <GLWrapper/Asset.h>
#include <Config.h>

#include <std_image.h> // stbi_load
#include <glm/gtc/type_ptr.hpp> // make_mat4
#include <vector> // std::vector
#include <iostream>

//glm::mat4 Asset::isometricSkew;
GLuint Asset::VBO, Asset::VAO, Asset::EBO, Asset::shaderProgram;

//void Asset::setIsometricSkew(float skew, float rotation)
//{
//	float skewArray[16] = {
//	1.0f, 0.0f, 0.0f, 0.0f,
//	skew, 1.0f, 0.0f, 0.0f,
//	0.0f, 0.0f, 1.0f, 0.0f,
//	0.0f, 0.0f, 0.0f, 1.0f
//	};
//
//	Asset::isometricSkew = glm::make_mat4(skewArray) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
//}

void Asset::generateVertexArray()
{
	float tilePt = GetConfig().tileSize / 2.0f;

	std::vector<glm::vec3> tilePoints = { glm::vec3(tilePt,tilePt, 0.0f), glm::vec3(tilePt,-tilePt, 0.0f), glm::vec3(-tilePt,-tilePt, 0.0f), glm::vec3(-tilePt,tilePt, 0.0f) };
	std::vector<glm::vec2> textureCoords = { glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f) };
	std::vector<GLuint> tileIndices = { 0, 3, 1, 3, 1, 2 };

	//sets up a VBO vector thats formated as x,y,z,Tx,Ty
	std::vector<GLfloat> VBOvector;
	for (int i = 0; i < tilePoints.size(); i++) {
		VBOvector.push_back(tilePoints[i].x);
		VBOvector.push_back(tilePoints[i].y);
		VBOvector.push_back(tilePoints[i].z);

		VBOvector.push_back(textureCoords[i].x);
		VBOvector.push_back(textureCoords[i].y);
	}

	// Create buffers/arrays
	glGenVertexArrays(1, &Asset::VAO);
	glGenBuffers(1, &Asset::VBO);
	glGenBuffers(1, &Asset::EBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(Asset::VAO);

	glBindBuffer(GL_ARRAY_BUFFER, Asset::VBO);
	glBufferData(GL_ARRAY_BUFFER, VBOvector.size() * sizeof(GLfloat), &VBOvector.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Asset::EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, tileIndices.size() * sizeof(GLuint), &tileIndices.front(), GL_STATIC_DRAW);


	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		5 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glEnableVertexAttribArray(0); //enable the var

	glVertexAttribPointer(1,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		2, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		5 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)(3 * sizeof(GLfloat))); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glEnableVertexAttribArray(1); //enable the var

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}

void Asset::useShaderProgram(const glm::mat4& P, const glm::mat4& V, const glm::vec3& camPos)
{
	glUseProgram(Asset::shaderProgram);
	GLuint MatrixID = glGetUniformLocation(Asset::shaderProgram, "view");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &V[0][0]);

	MatrixID = glGetUniformLocation(Asset::shaderProgram, "projection");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &P[0][0]);

	GLuint uCam = glGetUniformLocation(Asset::shaderProgram, "cameraPos");
	glUniform3f(uCam, camPos.x, camPos.y, camPos.z);
}


Asset::Asset(std::string& filename)
{
	// add animation
	glGenTextures(1, &m_textureID);
	int nrChannels;
	printf("animation path %s \n", filename.c_str());
	stbi_set_flip_vertically_on_load(true);
	unsigned char* animation = stbi_load(filename.c_str(), &m_width, &m_height, &nrChannels, 0);

	if (animation == NULL)
	{
		printf("error loading image: %s\n", filename.c_str());
		return;
	}

	m_numFrames = m_width / m_height;

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, animation);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(animation);

}

Asset::~Asset()
{

}

void Asset::releaseBuffers()
{
	glDeleteVertexArrays(1, &Asset::VAO);
	glDeleteBuffers(1, &Asset::VBO);
	glDeleteBuffers(1, &Asset::EBO);
}

GLint Asset::getTextureID()
{
	return m_textureID;
}

void Asset::Render(const glm::vec3& position, const UniformContainer& uniforms)
{
	glm::mat4 toWorld = glm::translate(glm::mat4(1.0f), position);
	GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);

	GLuint usesTexId = glGetUniformLocation(shaderProgram, "usesTexture");
	glUniform1i(usesTexId, 1);

	GLuint texWidthId = glGetUniformLocation(shaderProgram, "animationWidth");
	glUniform1i(texWidthId, m_width);

	GLuint texHeightId = glGetUniformLocation(shaderProgram, "animationHeight");
	glUniform1i(texHeightId, m_height);

	uniforms.SetUniforms();

	glBindTexture(GL_TEXTURE_2D, m_textureID);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Asset::DrawStatusBar(const glm::vec3& position, float percentage)
{
	glBindVertexArray(VAO);

	GLuint texBool = glGetUniformLocation(shaderProgram, "usesTexture");
	glUniform1i(texBool, false);

	GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
	GLuint colorId = glGetUniformLocation(shaderProgram, "colorOverride");

	glm::vec3 healthBarPosition(position.x, position.y - GetConfig().tileSize / 1.5f, position.z);
	glm::mat4 toWorld = glm::scale(glm::translate(glm::mat4(1.0f), healthBarPosition), glm::vec3(0.9f, 0.05f, 1.0f));
	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);
	glm::vec3 red(1.0f, 0.0f, 0.0f);
	glUniform3fv(colorId, 1, &red[0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	healthBarPosition.x -= 0.9f - (0.9f * percentage);
	healthBarPosition.z += 0.01f;
	toWorld = glm::scale(glm::translate(glm::mat4(1.0f), healthBarPosition), glm::vec3(0.9f * percentage, 0.05f, 1.0f));
	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);
	glm::vec3 green(0.0f, 1.0f, 0.0f);
	glUniform3fv(colorId, 1, &green[0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

// template specializations

 // mat4
//template <>
//void Asset::SetUniform<glm::mat4>(const std::string& uiformName, const glm::mat4& val) const
//{
//	GLuint uniformIntId = glGetUniformLocation(shaderProgram, uiformName.c_str());
//	glUniformMatrix4fv(uniformIntId, 1, GL_FALSE, &val[0][0]);
//}
//
//// vec3
//void Asset::SetUniform<glm::vec3>(const std::string& uiformName, const glm::vec3& val) const
//{
//	GLuint uniformVec3Id = glGetUniformLocation(shaderProgram, uiformName.c_str());
//	glUniform3fv(uniformVec3Id, 1, &val[0]);
//}
//
//// int
//void Asset::SetUniform(const std::string& uiformName, const int& val) const
//{
//	GLuint uniformId = glGetUniformLocation(shaderProgram, uiformName.c_str());
//	glUniform1i(uniformId, val);
//}
//
//// float
//void Asset::SetUniform<float>(const std::string& uiformName, const float& val) const
//{
//	GLuint uniformFltId = glGetUniformLocation(shaderProgram, uiformName.c_str());
//	glUniform1f(uniformFltId, val);
//}
//
//// bool (I know, weird)
//void Asset::SetUniform<bool>(const std::string& uiformName, const bool& val) const
//{
//	int isTrue = val ? 1 : 0;
//	GLuint uniformBoolId = glGetUniformLocation(shaderProgram, uiformName.c_str());
//	glUniform1i(uniformBoolId, isTrue);
//}
