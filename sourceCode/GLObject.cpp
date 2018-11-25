#include "GLObject.h"

#include "Shader.h"

#include <glm/gtc/matrix_transform.hpp> // translate
#include <glm/gtc/type_ptr.hpp> // make_mat4
#include <string> // string

std::unordered_map<std::string, Asset*> GLObject::assets;

void GLObject::initialize()
{
	Asset::setIsometricSkew(0.19f, -14.0f);
	Asset::generateVertexArray();
	Asset::shaderProgram = LoadShaders((INSTALL_DIR + "sourceCode/shader.vert").c_str(), (INSTALL_DIR + "sourceCode/shader.frag").c_str());
}

void GLObject::useShaderProgram(glm::mat4& P, glm::mat4& V)
{
	Asset::useShaderProgram(P, V);
}

const glm::mat4 GLObject::getIsometricSkew()
{
	return Asset::isometricSkew;
}

void GLObject::setIsometricSkew(float skew, float rot)
{
	Asset::setIsometricSkew(skew, rot);

}

//GLint TextureFromFile(std::string filename)
//{
//	//Generate texture ID and load texture data 
//	GLuint textureID;
//	glGenTextures(1, &textureID);
//	int width, height, nrChannels;
//	printf("filepath %s \n", filename.c_str());
//	unsigned char *image = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
//
//
//	// Parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	// Assign texture to ID
//	glBindTexture(GL_TEXTURE_2D, textureID);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
//	glGenerateMipmap(GL_TEXTURE_2D);
//
//	glBindTexture(GL_TEXTURE_2D, 0);
//	stbi_image_free(image);
//	return textureID;
//}


Asset* GLObject::GLAsset(const char* textureFile)
{
	if (GLObject::assets.find(textureFile) == GLObject::assets.end())
	{
		std::string filepath = INSTALL_DIR + "models/" + textureFile;
		GLObject::assets[textureFile] = new Asset(filepath);
	}

	return GLObject::assets[textureFile];
}

GLObject::GLObject()
{
	OBJECT_TYPE = ObjectType::GENERIC;
	currentState = State::IDLE;
	position = glm::vec3(0.0f);
}

GLObject::GLObject(Asset* asset)
{
	OBJECT_TYPE = ObjectType::GENERIC;
	currentState = State::IDLE;
	position = glm::vec3(0.0f);

	this->asset = asset;
}

GLObject::GLObject(const char* textureFile) : GLObject(GLObject::GLAsset(textureFile)) {}

GLObject::~GLObject()
{

}

void GLObject::releaseBuffers()
{
	Asset::releaseBuffers();
}

void GLObject::draw()
{
	asset->render(position);
}

void GLObject::setPosition(glm::vec3& pos)
{
	position = pos;
}

glm::vec3 GLObject::getPosition()
{
	return position;
}

void GLObject::setState(State state)
{
	currentState = state;
}

const State& GLObject::getState()
{
	return currentState;
}

void GLObject::drawDebugLine(glm::vec3 pt1, glm::vec3 pt2)
{
	Asset::draw_debug_line(pt1, pt2);
}
