#include "Cube.h"

Cube::Cube(const unsigned int cubeID, const unsigned int sideLength, QObject* parent) : QObject(parent)
{
	// Initialize member variables
	state = CubeState::VALID;
	this->cubeID = cubeID;
	this->sideLength = sideLength;
	position = glm::vec3(0, 0, 0);
	orientation = glm::vec3(0, 0, 0);
}

void Cube::setState(const CubeState& state)
{
	this->state = state;
}

void Cube::setPosition(const glm::vec3& position)
{
	this->position = position;
}

void Cube::setOrientation(const glm::vec3& orientation)
{
	this->orientation = orientation;
}

unsigned int Cube::getCubeID() const
{
	return cubeID;
}

unsigned int Cube::getSideLength() const
{
	return sideLength;
}

CubeState Cube::getState() const
{
	return state;
}

glm::vec3 Cube::getPosition() const
{
	return position;
}

glm::vec3 Cube::getOrientation() const
{
	return orientation;
}

float Cube::getRoll() const
{
	return orientation[0];
}

float Cube::getPitch() const
{
	return orientation[1];
}

float Cube::getYaw() const
{
	return orientation[2];
}