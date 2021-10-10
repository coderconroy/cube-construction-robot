#include "CubeWorldModel.h"

CubeWorldModel::CubeWorldModel(QObject* parent) : QObject(parent) {};

CubeWorldModel::~CubeWorldModel()
{
	// Free dynamically allocated memory of cube objects
	for (int i = 0; i < cubes.size(); i++)
		delete cubes[i];

	cubes.clear();
}

const std::vector<Cube*>* CubeWorldModel::getCubes() const
{
	return &cubes;
}

void CubeWorldModel::insertCube(glm::vec3& position)
{
	Cube* newCube = new Cube(64, this);
	newCube->setPosition(position);
	cubes.push_back(newCube);
}