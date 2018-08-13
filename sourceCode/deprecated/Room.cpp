#include "Room.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>

Room::Room(const char* filepath, glm::vec3 center)
{
	this->center = center;
	isCopy = false;
	minBound = glm::vec2(100.0f, 100.0f);
	maxBound = glm::vec2(0.0f);

	std::string line;
	std::ifstream myfile(INSTALL_DIR + "Models/Rooms/" + filepath);
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			auto delimiter = line.find(", ");
			float xFloat = std::stof(line.substr(0, delimiter));
			float yFloat = std::stof(line.substr(delimiter+1, line.length()));

			if (xFloat < minBound.x)
			{
				minBound.x = xFloat;
			}
			else if (xFloat > maxBound.x)
			{
				maxBound.x = xFloat;
			}
			if (yFloat < minBound.y)
			{
				minBound.y = yFloat;
			}
			else if (yFloat > maxBound.y)
			{
				maxBound.y = yFloat;
			}

			glm::vec3 borderIndex(xFloat, yFloat, 0.0f);

			border.push_back(borderIndex);
			border.push_back(borderIndex);
		}
		myfile.close();

		border.pop_back();
		border.erase(border.begin());
	}
	else std::cout << "Unable to open file: " << std::string(INSTALL_DIR + "Rooms/" + filepath);

	glLineWidth(10.0f);

	// Create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, border.size() * sizeof(glm::vec3), &border.front(), GL_STATIC_DRAW);


	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glEnableVertexAttribArray(0); //enable the var

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	// debug line init
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

Room::Room(Room* copy, glm::vec3 center)
{
	border = copy->border;
	this->center = center;

	VBO = copy->VBO;
	VAO = copy->VAO;
	lineVBO = copy->lineVBO;
	lineVAO = copy->lineVAO;
	minBound = copy->minBound;
	maxBound = copy->maxBound;

	isCopy = true;
}

Room::~Room()
{
	// Properly de-allocate all resources once they've outlived their purpose
	if (!isCopy)
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &lineVAO);
		glDeleteBuffers(1, &lineVBO);
	}

}

void Room::render(GLuint shaderProgram)
{
	glm::mat4 toWorld = glm::translate(glm::mat4(1.0f), center);

	GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);

	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, border.size());
	glBindVertexArray(0);

}

void Room::collidesWithBorder(glm::vec3 pos, glm::vec3& offset)
{
	glm::vec3 outcomePos = pos + offset;

	for (int i = 0; i < border.size(); i+=2)
	{
		float distToPoint1 = glm::length(outcomePos - border[i]);
		float distToPoint2 = glm::length(outcomePos - border[i+1]);
		glm::vec3 borderWall = border[i] - border[i + 1];
		glm::vec3 unitVectorBorder = glm::normalize(border[i] - border[i + 1]);

		if (distToPoint1 <= glm::length(borderWall) && distToPoint2 <= glm::length(borderWall)) {
			// find projection onto border of border pt to player, then calc distance to projection
			glm::vec3 unitVectorBorder = glm::normalize(border[i] - border[i + 1]);
			glm::vec3 borderProjection = unitVectorBorder * glm::dot(outcomePos - border[i], unitVectorBorder);
			float distToBorder = glm::length(outcomePos - (border[i] + borderProjection));

			if (distToBorder < 0.2f)
			{
				// set offset to projection of offset onto border line
				offset = unitVectorBorder * glm::dot(offset, unitVectorBorder);
				outcomePos = pos + offset;
			}
		}
	}
}

void Room::draw_debug_line(glm::vec3 pt1, glm::vec3 pt2, GLuint shaderProgram)
{
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

bool Room::isWithinBounds(glm::vec3 pos)
{
	if (pos.x > minBound.x && pos.x < maxBound.x)
	{
		if (pos.y > minBound.y && pos.y < maxBound.y)
			return true;
	}

	return false;
}

bool linesIntersect(glm::vec3, glm::vec3, glm::vec3, glm::vec3);

// if it can create a smart path to the destination, returns true and modifies dest with waypoint
// if it cannot, return false;
bool Room::smartPath(const glm::vec3& currentPos, glm::vec3& dest)
{
	for (int i = 0; i < border.size(); i += 2)
	{
		if (linesIntersect(currentPos, dest, border[i], border[i+1]))
			return false;
	}

	return true;

}


bool linesIntersect(glm::vec3 lineAStart, glm::vec3 lineAStop, glm::vec3 lineBStart, glm::vec3 lineBStop)
{
	glm::vec2 startA, startB, stopA, stopB;

	if (lineAStart.x < lineAStop.x)
	{
		startA.x = lineAStart.x;
		startA.y = lineAStart.y;
		stopA.x = lineAStop.x;
		stopA.y = lineAStop.y;
	}
	else
	{
		stopA.x = lineAStart.x;
		stopA.y = lineAStart.y;
		startA.x = lineAStop.x;
		startA.y = lineAStop.y;
	}

	if (lineBStart.x < lineBStop.x)
	{
		startB.x = lineBStart.x;
		startB.y = lineBStart.y;
		stopB.x = lineBStop.x;
		stopB.y = lineBStop.y;
	}
	else
	{
		stopB.x = lineBStart.x;
		stopB.y = lineBStart.y;
		startB.x = lineBStop.x;
		startB.y = lineBStop.y;
	}

	glm::vec2 vectorA = stopA - startA;
	glm::vec2 vectorB = stopB - startB;

	if (vectorA.x == 0.0f)
	{

	}
	else if (vectorB.x == 0.0f)
	{

	}
	else
	{
		float lineASlope = vectorA.y / vectorA.x;
		float lineBSlope = vectorB.y / vectorB.x;
		float yIntLineA = startA.y - (lineASlope * startA.x);
		float yIntLineB = startB.y - (lineBSlope * startB.x);

		glm::vec2 intercept(0.0f);

		intercept.x = (yIntLineB - yIntLineA) / (lineASlope - lineBSlope);
		intercept.y = (lineASlope * intercept.x) + yIntLineA;


		if (intercept.x >= startA.x && intercept.x <= stopA.x)
		{
			if ((lineASlope >= 0 && intercept.y >= startA.y && intercept.y <= stopA.y)
				|| (lineASlope <= 0 && intercept.y >= stopA.y && intercept.y <= startA.y))
			{
				if ((lineBSlope >= 0 && intercept.y >= startB.y && intercept.y <= stopB.y)
					|| (lineBSlope <= 0 && intercept.y >= stopB.y && intercept.y <= startB.y))
				{
					printf("LineA: (%f,%f), (%f,%f)\n", startA.x, startA.y, stopA.x, stopA.y);
					printf("LineB: (%f,%f), (%f,%f)\n", startB.x, startB.y, stopB.x, stopB.y);
					printf("Line A: y = %f x + %f\n", lineASlope, yIntLineA);
					printf("Line B: y = %f x + %f\n", lineBSlope, yIntLineB);
					printf("intercept: (%f, %f)\n", intercept.x, intercept.y);

					return true;
				}

			}
		}
	}



	return false;

}