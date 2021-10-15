#include "Cube.h"

Cube::Cube(QObject* parent) : QObject(parent) 
{
	// Initialize member variables
	state = CubeState::VALID;
}

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

void Cube::read(const QJsonObject& json)
{
	// Read and initialize cube identifier
	if (json.contains("cubeID") && json["cubeID"].isDouble())
		cubeID = json["cubeID"].toInt();

	// Read and initialize cube side length
	if (json.contains("sideLength") && json["sideLength"].isDouble())
		sideLength = json["sideLength"].toInt();

	// Read and initialize cube position
	if (json.contains("position") && json["position"].isArray()) {
		QJsonArray jsonPosition = json["position"].toArray();
		position = glm::vec3(jsonPosition[0].toDouble(), jsonPosition[1].toDouble(), jsonPosition[2].toDouble());
	}

	// Read and initialize cube orientation
	if (json.contains("orientation") && json["orientation"].isArray()) {
		QJsonArray jsonOrientation = json["orientation"].toArray();
		orientation = glm::vec3(jsonOrientation[0].toDouble(), jsonOrientation[1].toDouble(), jsonOrientation[2].toDouble());
	}
}

void Cube::write(QJsonObject& json) const
{
	// Write integer members to JSON object
	json["cubeID"] = (int) cubeID;
	json["sideLength"] = (int) sideLength;

	// Initialize JSON arrays
	QJsonArray jsonPosition = {position.x, position.y, position.z};
	QJsonArray jsonOrientation = {orientation.x, orientation.y, orientation.z};

	// Write JSON arrays to JSON object
	json["position"] = jsonPosition;
	json["orientation"] = jsonOrientation;
}