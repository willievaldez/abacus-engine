#include <GLWrapper/GLObject.h>

#include <GLWrapper/Shader.h>
#include <Utility/Config.h>

#include <glm/gtc/matrix_transform.hpp> // translate
#include <glm/gtc/type_ptr.hpp> // make_mat4
#include <string> // string

std::unordered_map<std::string, Asset*> GLObject::s_assets;

void GLObject::Initialize()
{
	if (!GetConfig().is3D)
	{
		//Asset::setIsometricSkew(0.0f, 0.0f);
		Asset::generateVertexArray();
	}

	Asset::shaderProgram = LoadShaders((INSTALL_DIR + "Code/shaders/shader.vert").c_str(), (INSTALL_DIR + "Code/shaders/shader.frag").c_str());
}

void GLObject::useShaderProgram(const glm::mat4& P, const glm::mat4& V, const glm::vec3& camPos)
{
	Asset::useShaderProgram(P, V, camPos);
}


// ----------------------------- 2D Functions
//const glm::mat4 GLObject::getIsometricSkew()
//{
//	return Asset::isometricSkew;
//}
//
//void GLObject::setIsometricSkew(float skew, float rot)
//{
//	Asset::setIsometricSkew(skew, rot);
//}

Asset* GLObject::GLAsset(const char* textureFile)
{
	if (GLObject::s_assets.find(textureFile) == GLObject::s_assets.end())
	{
		std::string filepath = INSTALL_DIR + "Assets/2D/" + textureFile;
		GLObject::s_assets[textureFile] = new Asset(filepath);
	}

	return GLObject::s_assets[textureFile];
}
// ----------------------------- 2D Functions


GLObject::GLObject()
{
	OBJECT_TYPE = ObjectType::GENERIC;
}

GLObject::GLObject(Asset* asset)
{
	OBJECT_TYPE = ObjectType::GENERIC;

	m_asset = asset;
}

GLObject::GLObject(const char* filepath, const AssimpParsingParams& params)
{
	OBJECT_TYPE = ObjectType::GENERIC;

	if (GetConfig().is3D)
	{
		printf("GLObject filepath: %s\n", filepath);
		m_model = new Model((GLchar*)filepath, params);
	}
	else
	{
		m_asset = GLObject::GLAsset(filepath);
	}
}

GLObject::~GLObject()
{
	if (GetConfig().is3D)
	{
		delete m_model;
	}
}

void GLObject::releaseBuffers()
{
	if (!GetConfig().is3D)
	{
		Asset::releaseBuffers();
	}

	glDeleteProgram(Asset::shaderProgram);
}

void GLObject::Render()
{
	if (GetConfig().is3D)
	{
		m_model->render(Asset::shaderProgram);
	}
	else
	{
		m_asset->Render(m_position);
	}
}

void GLObject::SetPosition(const glm::vec3& pos)
{
	m_direction = glm::normalize(pos - m_position);
	m_position = pos;
}

const glm::vec3& GLObject::GetPosition()
{
	return m_position;
}

const glm::vec3& GLObject::GetDirection()
{
	return m_direction;
}

void GLObject::SetDirection(const glm::vec3& dir)
{
	m_direction = dir;
}