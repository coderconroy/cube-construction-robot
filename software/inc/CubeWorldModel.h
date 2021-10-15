#pragma once

#include "Cube.h"
#include <QObject>
#include <QList>
#include <glm/glm.hpp>

class CubeWorldModel : public QObject
{
	Q_OBJECT
public:
	/*!
	* Class constructor.
	* \param [in] parent Parent \class QObject.
	*/
	CubeWorldModel(QObject* parent);

	/*!
	* Class destructor. Frees the memory dynamically allocated to the cubes.
	*/
	~CubeWorldModel();

	/*!
	* Getter for the list of cubes that have been inserted into the cube world.
	* \return List of cubes.
	*/
	const QList<Cube*>* getCubes() const;

	/*!
	* Insert cube into the cube world at the specified position relative to the world origin.
	* The inserted cube has a yaw of 0 radians about the z-axis.
	* \param [in] position Cartesian position of the cube centre (x, y, z) in steps.
	*/
	const Cube* insertCube(glm::vec3& position);

	/*!
	* Remove cube from the cube world.
	* \param [in] cube Cube to be removed
	*/
	void removeCube(const Cube* cube);

private:
	QList<Cube*> cubes; /*! List of cubes that have been inserted into the cube world */
};

