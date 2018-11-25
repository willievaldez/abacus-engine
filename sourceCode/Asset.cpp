#include "Asset.h"

#define STB_IMAGE_IMPLEMENTATION
#include <std_image.h> // stbi_load


#include <glm/gtc/type_ptr.hpp> // make_mat4
#include <vector> // std::vector

#include "Config.h"

glm::mat4 Asset::isometricSkew;
GLuint Asset::VBO, Asset::VAO, Asset::EBO, Asset::shaderProgram;
GLuint lineVAO, lineVBO;

void Asset::setIsometricSkew(float skew, float rotation)
{
	float skewArray[16] = {
	1.0f, 0.0f, 0.0f, 0.0f,
	skew, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
	};

	Asset::isometricSkew = glm::make_mat4(skewArray) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Asset::generateVertexArray()
{
	float tilePt = config::tileSize / 2.0f;

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


	// debug line init
	glLineWidth(5.0f);
	std::vector<glm::vec3> debugPoints = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };
	glGenVertexArrays(1, &lineVAO);
	glGenBuffers(1, &lineVBO);
	glBindVertexArray(lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), &debugPoints.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	// end debug line init
}

void Asset::useShaderProgram(glm::mat4& P, glm::mat4& V)
{
	glUseProgram(Asset::shaderProgram);
	GLuint MatrixID = glGetUniformLocation(Asset::shaderProgram, "view");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &V[0][0]);
	MatrixID = glGetUniformLocation(Asset::shaderProgram, "projection");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &P[0][0]);
}

void Asset::releaseBuffers()
{
	glDeleteVertexArrays(1, &Asset::VAO);
	glDeleteBuffers(1, &Asset::VBO);
	glDeleteBuffers(1, &Asset::EBO);

	// debug lines
	glDeleteVertexArrays(1, &lineVAO);
	glDeleteBuffers(1, &lineVBO);

	glDeleteProgram(Asset::shaderProgram);
}

GLint Asset::getTextureID()
{
	return textureID;
}

void Asset::render(glm::vec3& position)
{
	// TODO: given width and height, move tile up in the y axis to line the bottom of the asset up with the bottom of the asset.
	glm::vec4 isometricPositionVec4 = isometricSkew * glm::vec4(position, 1.0f);
	glm::vec3 isometricPosition = isometricPositionVec4;
	float areaNormalizer = std::sqrt(height * width); // used to maintain a 1u area
	glm::mat4 toWorld = glm::scale(glm::translate(glm::mat4(1.0f), isometricPosition), glm::vec3(width/areaNormalizer, height/areaNormalizer, 1.0f));

	GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

Asset::Asset(std::string& filename)
{
	//Generate texture ID and load texture data 
	glGenTextures(1, &textureID);
	int nrChannels;
	printf("filepath %s \n", filename.c_str());
	unsigned char *image = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

	if (image == NULL)
	{
		printf("error loading image: %s\n", filename.c_str());
		return;
	}

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
}

Asset::~Asset()
{

}

void Asset::draw_debug_line(glm::vec3 raw_pt1, glm::vec3 raw_pt2)
{
	glm::vec4 isometricPositionVec4 = isometricSkew * glm::vec4(raw_pt1, 1.0f);
	glm::vec3 pt1 = isometricPositionVec4;
	isometricPositionVec4 = isometricSkew * glm::vec4(raw_pt2, 1.0f);
	glm::vec3 pt2 = isometricPositionVec4;

	glm::mat4 toWorld(1.0f);

	// determine translation
	toWorld = glm::translate(toWorld, pt1);

	// determine rotation using cosine formula
	glm::vec3 displacement = pt2 - pt1;
	float scale = glm::length(displacement);
	glm::vec3 scaledUpVector = glm::vec3(0.0f, 1.0f, 0.0f) * scale;

	float cosTheta = glm::dot(scaledUpVector, displacement) / (scale * scale);
	float theta = std::acos(cosTheta);
	if (pt1.x < pt2.x) theta = theta * -1.0f;
	toWorld = glm::rotate(toWorld, theta, glm::vec3(0.0f, 0.0f, 1.0f));

	// determine scale
	toWorld = glm::scale(toWorld, glm::vec3(scale, scale, scale));

	// draw the line
	GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);
	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}