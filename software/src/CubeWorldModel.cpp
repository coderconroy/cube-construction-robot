#include "CubeWorldModel.h"

CubeWorldModel::CubeWorldModel(QObject* parent) : QObject(parent) {};

CubeWorldModel::~CubeWorldModel()
{
	// Free dynamically allocated memory of cube objects
	for (int i = 0; i < cubes.size(); i++)
		delete cubes[i];

	cubes.clear();
	selectedCube = Q_NULLPTR;
}

const QList<Cube*>* CubeWorldModel::getCubes() const
{
	return &cubes;
}

const Cube* CubeWorldModel::insertCube(glm::vec3& position)
{
	// Create new cube
	unsigned int cubeSize = 64;
	Cube* newCube = new Cube(++lastCubeID, cubeSize, this);
	newCube->setPosition(position);

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
	cubes.clear();
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

void CubeWorldModel::updateSelectedCubePosition(const int deltaX, const int deltaZ, const int deltaLayer)
{
	if (selectedCube == Q_NULLPTR)
		return;

	glm::vec3 position = selectedCube->getPosition();
	position = glm::vec3(position.x + deltaX, position.y, position.z + deltaZ);
	selectedCube->setPosition(position);
}