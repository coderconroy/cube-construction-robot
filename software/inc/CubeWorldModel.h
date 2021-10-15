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

	/*!
	* Remove all cubes from the cube world.
	*/
	void clearCubes();

	/*!
	* Getter for the size of the cube world.
	* \return Number of cubes in cube world.
	*/
	int getCubeCount() const;

	/*!
	* Updates the position of the selected cube.
	* \param [in] deltaX Position change along x-axis from current x-axis position.
	* \param [in] deltaZ Position change along z-axis from current z-axis position.
	* \param [in] deltaLayer Position change in layers from current layer position.
	*/
	void updateSelectedCubePosition(const int deltaX, const int deltaZ, const int deltaLayer);

private:
	Cube* selectedCube = Q_NULLPTR; /*! Reference to the cube currently in the selected state */
	unsigned int lastCubeID = 0; /*! Cube identifier of most recently inserted cube */
	QList<Cube*> cubes; /*! List of cubes that have been inserted into the cube world */
};

