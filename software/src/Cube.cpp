#include "Cube.h"

Cube::Cube(unsigned int sideLength, QObject* parent) : QObject(parent)
{
	// Initialize member variables
	this->sideLength = sideLength;
	position = glm::vec3(0, 0, 0);
	orientation = glm::vec3(0, 0, 0);
}

void Cube::setPosition(const glm::vec3& position)
{
	this->position = position;
}

void Cube::setOrientation(const glm::vec3& orientation)
{
	this->orientation = orientation;
}

unsigned int Cube::getSideLength()
{
	return sideLength;
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