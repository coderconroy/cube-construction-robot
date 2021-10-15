#include "CubeWorldModel.h"

CubeWorldModel::CubeWorldModel(unsigned int cubeSideLength, unsigned int cubeMargin, QObject* parent) : QObject(parent) 
{
	// Initialize member variables
	this->cubeSideLength = cubeSideLength;
	this->cubeMargin = cubeMargin;

	// Check input is valid
	if (cubeSideLength % 2 != 0)
		emit log(Message(MessageType::ERROR_LOG, "Cube World Model", "Cube side length must be a multiple of 2"));
};

CubeWorldModel::~CubeWorldModel()
{
	// Free dynamically allocated memory of cube objects
	for (int i = 0; i < cubes.size(); i++)
		delete cubes[i];

	// Reset pointers to deallocated cubes
	cubes.clear();
	selectedCube = Q_NULLPTR;
}

const QList<Cube*>* CubeWorldModel::getCubes() const
{
	return &cubes;
}

const Cube* CubeWorldModel::insertCube()
{
	// Create new cube
	Cube* newCube = new Cube(++lastCubeID, cubeSideLength, this);
	newCube->setPosition(glm::vec3(0, cubeSideLength / 2, 0));

	// Update selected cube
	if (selectedCube != Q_NULLPTR)
		selectedCube->setState(CubeState::VALID);
	selectedCube = newCube;
	newCube->setState(CubeState::SELECTED);

	// Add cube to cube world list
	cubes.append(newCube);

	return newCube;
}

void CubeWorldModel::removeCube(const Cube* cube)
{
	// Find cube to remove in cube list
	for (int i = 0; i < cubes.size(); ++i)
	{
		// Remove cube
		if (cubes[i]->getCubeID() == cube->getCubeID())
		{
			// If cube to be removed is the selected cube reset the selected cube reference
			if (selectedCube != Q_NULLPTR && cube->getCubeID() == selectedCube->getCubeID())
				selectedCube = Q_NULLPTR;

			// Remove cube from cube world
			cubes.removeAt(i);
			break;
		}
	}

	if (cubes.size() == 0)
		lastCubeID = 0;

	return;
}

void CubeWorldModel::clearCubes()
{
	// Free dynamically allocated memory of cube objects
	for (int i = 0; i < cubes.size(); i++)
		delete cubes[i];

	// Reset pointers to deallocated cubes
	cubes.clear();
	selectedCube = Q_NULLPTR;

	lastCubeID = 0;
}

void CubeWorldModel::selectCube(const Cube* cube)
{
	// Unselect currently selected cube
	if (selectedCube != Q_NULLPTR)
	{
		selectedCube->setState(CubeState::VALID);
		selectedCube = Q_NULLPTR;
	}

	// Check if a cube was specified
	if (cube == Q_NULLPTR)
		return;

	// Find cube to select in cube list
	for (int i = 0; i < cubes.size(); ++i)
	{
		// Select cube
		if (cubes[i]->getCubeID() == cube->getCubeID())
		{
			selectedCube = cubes[i];
			cubes[i]->setState(CubeState::SELECTED);
			break;
		}
	}

	return;
}

int CubeWorldModel::getCubeCount() const
{
	return cubes.size();
}

int CubeWorldModel::getCubeLayer(const Cube& cube) const
{
	unsigned int sideLength = cube.getSideLength();
	int bottomYPos = (cube.getPosition().y - sideLength / 2); // Y position of bottom cube face

	// Check if cube is in a valid layer vertical position
	if (bottomYPos % sideLength != 0)
	{
		emit log(Message(MessageType::ERROR_LOG, "Cube World Model", "Invalid vertical cube position in model"));
		return -1;
	}

	// Compute cube layer
	int layer = bottomYPos / sideLength;
	return layer;
}

void CubeWorldModel::updateSelectedCubePosition(int deltaX, int deltaZ, int deltaLayer)
{
	// Check if there is currently a cube selected
	if (selectedCube == Q_NULLPTR)
		return;

	// Check cube does not exceed construction vertical boundaries
	if (deltaLayer != 0)
	{
		int selectedCubeLayer = getCubeLayer(*selectedCube);
		if (selectedCubeLayer <= 0 && deltaLayer < 0)
			deltaLayer = 0;
		else if (selectedCubeLayer >= (numLayers - 1) && deltaLayer > 0)
			deltaLayer = 0;
	}

	// Update the position of the selected cube
	glm::vec3 position = selectedCube->getPosition();
	position = glm::vec3(position.x + deltaX, position.y + deltaLayer * (int) cubeSideLength, position.z + deltaZ);
	selectedCube->setPosition(position);
}

void CubeWorldModel::updateSelectedCubeOrientation(int angleSteps)
{
	// Check if there is currently a cube selected
	if (selectedCube == Q_NULLPTR)
		return;

	// Update the orientation of the selected cube
	glm::vec3 orientation = selectedCube->getOrientation();
	orientation.y += glm::radians(angleSteps * rotationAngleResolution);
	selectedCube->setOrientation(orientation);
}

void CubeWorldModel::read(const QJsonObject& json)
{
	// Read and cube list
	if (json.contains("cubes") && json["cubes"].isArray()) {
		QJsonArray jsonCubes = json["cubes"].toArray();
		clearCubes();
		for (int i = 0; i < jsonCubes.size(); ++i) {
			QJsonObject jsonCube = jsonCubes[i].toObject();
			Cube* cube = new Cube(this);
			cube->read(jsonCube);
			cubes.append(cube);
		}
	}

	// Read and initialize cube side length
	if (json.contains("cubeSideLength") && json["cubeSideLength"].isDouble())
		cubeSideLength = json["cubeSideLength"].toInt();

	// Read and initialize cube margin
	if (json.contains("cubeMargin") && json["cubeMargin"].isDouble())
		cubeMargin = json["cubeMargin"].toInt();

	// Read and initialize last cube identifier
	if (json.contains("lastCubeID") && json["lastCubeID"].isDouble())
		lastCubeID = json["lastCubeID"].toInt();
}

void CubeWorldModel::write(QJsonObject& json) const
{
	// Write integer members to JSON object
	json["cubeSideLength"] = (int) cubeSideLength;
	json["cubeMargin"] = (int) cubeMargin;
	json["lastCubeID"] = (int) lastCubeID;

	// Initialize cube list JSON array
	QJsonArray jsonCubes;
	for (int i = 0; i < cubes.size(); ++i)
	{
		QJsonObject jsonCube;
		cubes[i]->write(jsonCube);
		jsonCubes.append(jsonCube);
	}

	// Write cube list JSON array to JSON object
	json["cubes"] = jsonCubes;
}