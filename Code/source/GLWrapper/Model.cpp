#include <GLWrapper/Window.h>
#include <GLWrapper/Model.h>
#include <Utility/Config.h>

#include <std_image.h> // stbi_load
#include <unordered_map>

GLint TextureFromFile(const char *path, const std::string &directory, bool gamma=false)
{
	std::string filename = directory + '/' + path;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format = GL_RGBA;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		printf("Texture failed to load at path: %s\n", filename.c_str());
		stbi_image_free(data);
	}

	return textureID;
}

Model::Model(GLchar* path, const AssimpParsingParams& params)
{
	loadModel(path, params);
}

Model::~Model()
{
	//for (Mesh mesh : this->meshes)
	//{

	//}
}

// Draws the model, and thus all its meshes
void Model::render(GLuint shaderProgram)
{
	toWorld = glm::mat4(1.0f);

	GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);

	GLuint usesTexId = glGetUniformLocation(shaderProgram, "usesTexture");
	glUniform1i(usesTexId, m_hasTexture ? 1 : 0);

	for (GLuint i = 0; i < this->meshes.size(); i++)
		meshes[i].Draw(shaderProgram);
}

void Model::loadModel(std::string path, const AssimpParsingParams& params)
{
	path = INSTALL_DIR + "Assets/3D" + path;
	// Read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// Check for errors
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		printf("ERROR::ASSIMP:: %s\n", importer.GetErrorString());
		return;
	}

	// Retrieve the directory path of the filepath
	this->directory = path.substr(0, path.find_last_of('/'));
	if (this->directory == path)
	{
		this->directory = path.substr(0, path.find_last_of('\\'));
	}
	printf("directory (%s) set from path: %s\n", directory.c_str(), path.c_str());

	// Process ASSIMP's root node recursively
	this->processNode(scene->mRootNode, scene, params);

	for (auto& mesh : meshes)
	{
		if (params.resize)
		{
			mesh.Resize(min, max);
		}
		mesh.SetupMesh();
	}

}

void Model::processNode(aiNode* node, const aiScene* scene, const AssimpParsingParams& params)
{
	// Process each mesh located at the current node
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		// The node object only contains indices to index the actual objects in the scene. 
		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(processMesh(mesh, scene, params));
	}
	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, params);
	}

}

glm::vec3 FixCoordinateSystem(const aiVector3D& original, bool fix)
{
	glm::vec3 vector;
	vector.x = original.x;
	if (fix)
	{
		vector.y = original.z;
		vector.z = -1.0f * original.y;
	}
	else
	{
		vector.y = original.y;
		vector.z = original.z;
	}

	return vector;
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, const AssimpParsingParams& params)
{
	// Data to fill
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	std::unordered_map<float, std::unordered_map<float, std::unordered_map<float, std::vector<glm::vec3>>>> duplicatedPoints;
	std::vector<glm::vec3> uniquePoints;
	std::vector<size_t> vertexToIndexMappings;
	bool generateNormals = params.generateNormals;

	// Walk through each of the mesh's vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// Positions
		vertex.Position = FixCoordinateSystem(mesh->mVertices[i], params.fixCoordinateSystem);
		if (params.resize) UpdateBounds(vertex.Position);
		
		if (params.generateSmoothNormals)
		{
			// add vertex to unique set of vertices, track mapping to new set
			size_t index = 0;
			bool found = false;
			while (index < uniquePoints.size())
			{
				if (uniquePoints[index] == vertex.Position)
				{
					found = true;
					break;
				}

				index++;
			}
			if (!found)
			{
				index = uniquePoints.size();
				uniquePoints.push_back(vertex.Position);
			}
			vertexToIndexMappings.push_back(index);
		}

		// Normals
		if (mesh->mNormals)
		{
			if (!generateNormals)
			{
				vertex.Normal = FixCoordinateSystem(mesh->mNormals[i], false);
				if (params.invertNormals) vertex.Normal *= -1.0f;
				if (params.generateSmoothNormals) duplicatedPoints[vertex.Position.x][vertex.Position.y][vertex.Position.z].push_back(vertex.Normal);
			}
		}
		else
		{
			generateNormals = true; // if normals are not found, auto generate them
			printf("Normals not found for asset: %s. Generating", directory.c_str());
		}

		// Texture Coordinates
		if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			m_hasTexture = true;
			glm::vec2 vec;
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		// tangent
		if (mesh->mTangents)
		{
			vertex.Tangent = FixCoordinateSystem(mesh->mTangents[i], params.fixCoordinateSystem);
		}

		// bitangent
		if (mesh->mBitangents)
		{
			vertex.Bitangent = FixCoordinateSystem(mesh->mBitangents[i], params.fixCoordinateSystem);
		}

		vertices.push_back(vertex);
	}

	std::vector<std::vector<glm::vec3>> calculatedNormals; // outermost vector: for each vertex. inner: for each face that uses the vertex (usually 1)
	calculatedNormals.resize(vertices.size());

	// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		glm::vec3 calculatedNormal;
		if (generateNormals)
		{
			glm::vec3 a = vertices[face.mIndices[0]].Position;
			glm::vec3 b = vertices[face.mIndices[1]].Position;
			glm::vec3 c = vertices[face.mIndices[2]].Position;

			calculatedNormal = glm::normalize(glm::cross(b - a, c - a));
		}

		// Retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);

			if (generateNormals)
			{
				glm::vec3 pos = vertices[face.mIndices[j]].Position;
				duplicatedPoints[pos.x][pos.y][pos.z].push_back(calculatedNormal);
				calculatedNormals[face.mIndices[j]].push_back(calculatedNormal);
			}
		}
	}


	if (generateNormals)
	{
		for (int i = 0; i < calculatedNormals.size(); i++)
		{
			glm::vec3 avgNormal(0.0f, 0.0f, 0.0f);
			for (glm::vec3 norm : calculatedNormals[i])
			{
				avgNormal += norm;
			}

			avgNormal /= calculatedNormals[i].size();

			vertices[i].Normal = avgNormal;
		}
	}


	if (params.generateSmoothNormals)
	{
		// iterate through duplicatedPoints
		// flatten all vertices with the same coordinates into a single vertex
		// TODO: if angle between two vectors is two high, dont merge vertices
		// calculate normal as avg of all normals using this vertex
		// change indices of faces to match newly created vertices
		std::vector<Vertex> newVertices;
		std::vector<GLuint> newIndices;
		for (glm::vec3& point : uniquePoints)
		{
			Vertex vertex;

			// set positon
			vertex.Position = point;

			// set normal
			std::vector<glm::vec3> normalsForVertex = duplicatedPoints[point.x][point.y][point.z];
			glm::vec3 avgNormal(0.0f, 0.0f, 0.0f);
			for (glm::vec3 norm : normalsForVertex)
			{
				avgNormal += norm;
			}
			avgNormal /= normalsForVertex.size();
			vertex.Normal = avgNormal;

			// set tan/bit-tan/tex(?)

			newVertices.push_back(vertex);
		}

		for (int i = 0; i < indices.size(); i++)
		{
			newIndices.push_back((GLuint)vertexToIndexMappings[indices[i]]);
		}

		printf("vertices sizes - before: %d, after: %d\n", (int)vertices.size(), (int)newVertices.size());
		printf("indices sizes - before: %d, after: %d\n", (int)indices.size(), (int)newIndices.size());

		vertices = newVertices;
		indices = newIndices;
	}

	// Process textures
	std::vector<Texture> textures;
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// Diffuse: texture_diffuseN
	// Specular: texture_specularN
	// Normal: texture_normalN

	// 1. Diffuse maps
	std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. Specular maps
	std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	// return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures);
}


std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		GLboolean skip = false;
		for (GLuint j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // If texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

void Model::UpdateBounds(const glm::vec3& input)
{
	if (input.x < min.x)
	{
		min.x = input.x;
	}
	else if (input.x > max.x)
	{
		max.x = input.x;
	}

	if (input.y < min.y)
	{
		min.y = input.y;
	}
	else if (input.y > max.y)
	{
		max.y = input.y;
	}

	if (input.z < min.z)
	{
		min.z = input.z;
	}
	else if (input.z > max.z)
	{
		max.z = input.z;
	}
}