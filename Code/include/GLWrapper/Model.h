#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GLWrapper/Mesh.h>

struct AssimpParsingParams
{
	bool invertNormals = false;
	bool generateNormals = false;
	bool fixCoordinateSystem = false;
	bool generateSmoothNormals = false;
	bool resize = true;
};


class Model
{
public:

	/*  Functions   */
	// Constructor, expects a filepath to a 3D model.
	Model(GLchar* path, const AssimpParsingParams&);
	~Model();

	// Draws the model, and thus all its meshes
	void render(GLuint shader);

private:
	/*  Model Data  */
	std::vector<Texture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammaCorrection;
	bool m_hasTexture = false;

										/*  Functions   */
										// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(std::string path, const AssimpParsingParams&);

	// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene, const AssimpParsingParams&);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, const AssimpParsingParams&);

	// Checks all material textures of a given type and loads the textures if they're not loaded yet.
	// The required info is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	void UpdateBounds(const glm::vec3&);

	glm::mat4 toWorld;

	glm::vec3 min = glm::vec3(100.0f, 100.0f, 100.0f); // TODO: better initial value
	glm::vec3 max = glm::vec3(-100.0f, -100.0f, -100.0f);
};

