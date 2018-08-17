#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
using namespace std;

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SOIL/SOIL.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

class Model
{
public:

	/*  Functions   */
	// Constructor, expects a filepath to a 3D model.
	Model(GLchar* path);
	Model(GLchar* path, bool invertNormals);
	~Model();

	// Draws the model, and thus all its meshes
	void render(GLuint shader);

private:
	/*  Model Data  */
	vector<Mesh> meshes;
	string directory;
	vector<Texture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.

										/*  Functions   */
										// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string path);
	void loadModel(string path, bool invertNormals);

	// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	//void processNode(aiNode* node, const aiScene* scene);
	//Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene, bool invertNormals);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, bool invertNormals);

	// Checks all material textures of a given type and loads the textures if they're not loaded yet.
	// The required info is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);

	GLuint uProjection, uModel, uView;

};


