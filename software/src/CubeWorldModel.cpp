#include "CubeWorldModel.h"

CubeWorldModel::CubeWorldModel(QObject* parent) : QObject(parent) {};

CubeWorldModel::~CubeWorldModel()
{
	// Free dynamically allocated memory of cube objects
	for (int i = 0; i < cubes.size(); i++)
		delete cubes[i];

	cubes.clear();
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
			cubes.removeAt(i);
			break;
		}
	}

	return;
}

void CubeWorldModel::clearCubes()
{
	cubes.clear();
}

int CubeWorldModel::getCubeCount() const
{
	return cubes.size();
}