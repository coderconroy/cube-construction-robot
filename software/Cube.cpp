#include "Cube.h"

Cube::Cube(QObject* parent) : QObject(parent)
{
	position = glm::vec3(0, 0, 0);
	pose = glm::vec3(0, 0, 0);
}

void Cube::setPosition(const glm::vec3& position)
{
	this->position = position;
}

void Cube::setPose(const glm::vec3& pose)
{
	this->pose = pose;
}

glm::vec3 Cube::getPosition() const
{
	return position;
}

glm::vec3 Cube::getPose() const
{
	return pose;
}