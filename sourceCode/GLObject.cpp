#include "GLObject.h"

#include "Shader.h"

#include <glm/gtc/matrix_transform.hpp> // translate
#include <glm/gtc/type_ptr.hpp> // make_mat4
#include <string> // string

#define STB_IMAGE_IMPLEMENTATION

#include <std_image.h>

GLuint GLObject::VAO, GLObject::VBO, GLObject::EBO, GLObject::shaderProgram;
std::unordered_map<std::string, GLint> GLObject::assets;
float GLObject::tileSize;
glm::mat4 GLObject::isometricSkew;

void GLObject::setTileSize(float tileSize)
{
	float skewArray[16] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.19f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	isometricSkew = glm::make_mat4(skewArray) * glm::rotate(glm::mat4(1.0f), glm::radians(-22.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	GLObject::tileSize = tileSize;

	float tilePt = tileSize / 2.0f;

	std::vector<glm::vec3> tilePoints = { glm::vec3(-tilePt,tilePt, 0.0f), glm::vec3(tilePt,tilePt, 0.0f), glm::vec3(tilePt,-tilePt, 0.0f), glm::vec3(-tilePt,-tilePt, 0.0f) };
	std::vector<glm::vec2> textureCoords = { glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f) };
	std::vector<GLuint> tileIndices = { 0, 1, 2, 0, 2, 3 };

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
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, VBOvector.size() * sizeof(GLfloat), &VBOvector.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
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

	shaderProgram = LoadShaders((INSTALL_DIR + "sourceCode/shader.vert").c_str(), (INSTALL_DIR + "sourceCode/shader.frag").c_str());
}

void GLObject::useShaderProgram(glm::mat4& P, glm::mat4& V)
{
	// Use the shader of programID
	glUseProgram(shaderProgram);
	GLuint MatrixID = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &V[0][0]);
	MatrixID = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &P[0][0]);
}

GLint TextureFromFile(std::string filename)
{
	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height, nrChannels;
	printf("filepath %s \n", filename.c_str());
	unsigned char *image = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);


	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image);
	return textureID;
}


GLint GLObject::Asset(const char* textureFile)
{
	if (GLObject::assets.find(textureFile) == GLObject::assets.end())
	{
		GLObject::assets[textureFile] = TextureFromFile(INSTALL_DIR + "models/" + textureFile);
	}

	return GLObject::assets[textureFile];
}

GLObject::GLObject()
{
	OBJECT_TYPE = ObjectType::GENERIC;
	position = glm::vec3(0.0f);
	renderTexture = false;
}

GLObject::GLObject(GLint texID)
{
	OBJECT_TYPE = ObjectType::GENERIC;
	position = glm::vec3(0.0f);
	textureID = texID;
	renderTexture = true;
}

GLObject::GLObject(const char* textureFile) : GLObject(GLObject::Asset(textureFile)) {}

GLObject::~GLObject()
{

}

void GLObject::releaseBuffers()
{
	glDeleteVertexArrays(1, &GLObject::VAO);
	glDeleteBuffers(1, &GLObject::VBO);
	glDeleteBuffers(1, &GLObject::EBO);

	glDeleteProgram(shaderProgram);
}

void GLObject::render()
{
	glm::vec4 isometricPositionVec4 = GLObject::isometricSkew * glm::vec4(position, 1.0f);
	glm::vec3 isometricPosition = isometricPositionVec4;
	glm::mat4 toWorld = glm::translate(glm::mat4(1.0f), isometricPosition);

	GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);

	if (!renderTexture)
	{
		GLuint texBool = glGetUniformLocation(shaderProgram, "useTex");
		glUniform1i(texBool, false);

		GLuint colorId = glGetUniformLocation(shaderProgram, "color");
		glUniform3fv(colorId, 1, &color[0]);
	}
	else
	{
		GLuint texBool = glGetUniformLocation(shaderProgram, "useTex");
		glUniform1i(texBool, true);

		glBindTexture(GL_TEXTURE_2D, textureID);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	GLuint texBool = glGetUniformLocation(shaderProgram, "useTex");
	glUniform1i(texBool, false);

}

void GLObject::render(float disp)
{

	glm::vec4 isometricPositionVec4 = GLObject::isometricSkew * glm::vec4(position, 1.0f);
	glm::vec3 isometricPosition = isometricPositionVec4;
	glm::mat4 toWorld = glm::translate(glm::mat4(1.0f), isometricPosition);

	GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);

	if (!renderTexture)
	{
		GLuint texBool = glGetUniformLocation(shaderProgram, "useTex");
		glUniform1i(texBool, false);

		GLuint colorId = glGetUniformLocation(shaderProgram, "color");
		glUniform3fv(colorId, 1, &color[0]);
	}
	else
	{
		GLuint texBool = glGetUniformLocation(shaderProgram, "useTex");
		glUniform1i(texBool, true);

		glBindTexture(GL_TEXTURE_2D, textureID);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	GLuint texBool = glGetUniformLocation(shaderProgram, "useTex");
	glUniform1i(texBool, false);

}

void GLObject::drawDestinationFlag(glm::vec3& dest)
{
	glBindVertexArray(VAO);

	glm::vec4 isometricPositionVec4 = GLObject::isometricSkew * glm::vec4(dest, 1.0f);
	glm::vec3 isometricPosition = isometricPositionVec4;

	glm::mat4 toWorld = glm::scale(glm::translate(glm::mat4(1.0f), isometricPosition), glm::vec3(0.5f, 0.5f, 1.0f));
	GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);

	GLuint texBool = glGetUniformLocation(shaderProgram, "useTex");
	glUniform1i(texBool, true);

	glBindTexture(GL_TEXTURE_2D, GLObject::Asset("pixelflag.png"));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void GLObject::drawSelectedMarker(bool green)
{
	glBindVertexArray(VAO);

	glm::vec4 isometricPositionVec4 = GLObject::isometricSkew * glm::vec4(position, 1.0f);
	glm::vec3 isometricPosition = isometricPositionVec4;
	glm::mat4 toWorld = glm::scale(glm::translate(glm::mat4(1.0f), isometricPosition), glm::vec3(0.9f, 0.9f, 1.0f));
	GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);

	GLuint texBool = glGetUniformLocation(shaderProgram, "useTex");
	glUniform1i(texBool, true);

	if (green)
	{
		glBindTexture(GL_TEXTURE_2D, GLObject::Asset("greentarget.png"));
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, GLObject::Asset("redtarget.png"));
	}

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void GLObject::setPosition(glm::vec3& pos)
{
	position = pos;
}

glm::vec3 GLObject::getPosition()
{
	return position;
}

void GLObject::setTextureID(GLint id)
{
	textureID = id;
}