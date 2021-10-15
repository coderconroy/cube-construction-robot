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
	Cube* newCube = new Cube(64, this);
	newCube->setPosition(position);
	cubes.append(newCube);
	return newCube;
}

void CubeWorldModel::removeCube(const Cube* cube)
{
	// Find cube to remove in cube list
	for (int i = 0; i < cubes.size(); ++i)
	{
		// Remove cube
		if (cubes[i] == cube)
		{
			cubes.removeAt(i);
		}
		break;
	}

	return;
}