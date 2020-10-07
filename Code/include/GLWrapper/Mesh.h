#pragma once

#include <vector>
#include <string>
#include <sstream>

using namespace std;

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/types.h>
#include <assimp/scene.h>

struct Vertex {
	glm::vec3 Position;
	glm::vec2 TexCoords;
	glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};


struct Texture {
	GLuint id;
	string type;
	aiString path;
};

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float opacity;
	float shininess;
};

class Mesh {

public:

	Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);
	Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, aiMaterial* material);

	/*  Mesh Data  */
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	Material material;

	/*  Functions  */
	void Draw(GLuint shaderId);
	void Resize(const glm::vec3&, const glm::vec3&);
	void SetupMesh();

private:
	/*  Render data  */
	GLuint VAO, VBO, EBO;
	/*  Functions    */
};