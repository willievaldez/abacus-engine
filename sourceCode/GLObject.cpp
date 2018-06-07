#include "GLObject.h"


GLObject::GLObject(std::vector<GLuint> indices, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals)
{
	position = glm::vec3(0.0f);
	number_of_indices = indices.size();

	//sets up a VBO vector thats formated as x,y,z,xn,yn,zn
	std::vector<glm::vec3> VBOvector;
	for (int i = 0; i < vertices.size(); i++) {
		VBOvector.push_back(vertices[i]);
		VBOvector.push_back(normals[i]);
	}

	// Create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, VBOvector.size() * sizeof(glm::vec3), &VBOvector.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices.front(), GL_STATIC_DRAW);


	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		6 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glEnableVertexAttribArray(0); //enable the var

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat))); // load normals into normal variable

	glEnableVertexAttribArray(1); //enable the var

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}

GLObject::GLObject(const char* path) {
	this->model = new Model((GLchar*)path);
}

GLObject::~GLObject()
{
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	if (this->model)
	{
		delete this->model;
	}
}

void GLObject::render(GLuint shaderProgram) {
	glm::mat4 toWorld = glm::translate(glm::mat4(1.0f), position);

	GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, number_of_indices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//this->model->render(shaderProgram);
}

void GLObject::move(glm::vec3 velocity) {
	position += velocity;
}