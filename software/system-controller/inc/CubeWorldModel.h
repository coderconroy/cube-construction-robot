#pragma once

#include "Cube.h"
#include "Logger.h"
#include <QObject>
#include <QList>
#include <QJsonObject>
#include <glm/glm.hpp>

class CubeWorldModel : public QObject
{
	Q_OBJECT
public:
	/*!
	* Class constructor.
	* \param [in] parent Parent \class QObject.
	* \param [in] sideLength Side length of cubes in this model in steps.
	* \param [in] cubeMargin Size of margin in steps between cubes in the horizonal plane.
	*/
	CubeWorldModel(unsigned int cubeSideLength, unsigned int cubeMargin, QObject* parent);

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
	*/
	const Cube* insertCube();

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
	* Update currently selected cube.
	* \param [in] cube Cube to select.
	*/
	void selectCube(const Cube* cube);

	/*!
	* Getter for the size of the cube world.
	* \return Number of cubes in cube world.
	*/
	int getCubeCount() const;

	/*!
	* Get the layer number the cube is placed in. Layers are zero-indexed.
	* \param [in] cube Cube layer information is requested about.
	* \return Layer the cube is found in.
	*/
	int getCubeLayer(const Cube& cube) const;

	/*!
	* Updates the position of the selected cube.
	* \param [in] deltaX Position change along x-axis from current x-axis position in steps.
	* \param [in] deltaZ Position change along z-axis from current z-axis position in steps.
	* \param [in] deltaLayer Position change in layers from current layer position in steps.
	*/
	void updateSelectedCubePosition(int deltaX, int deltaZ, int deltaLayer);

	/*!
	* Updates the orientation of the selected cube.
	* \param [in] angleSteps Cube rotation change about the vertical y-axis in steps of the rotation angle resolution.
	*/
	void updateSelectedCubeOrientation(int angleSteps);

	/*!
	* Load cube world model from a JSON object.
	* \param [in] json JSON object to load the cube world model from.
	*/
	void read(const QJsonObject& json);

	/*!
	* Write the cube world model to a JSON object.
	* \param [out] json JSON object to write the cube world model to.
	*/
	void write(QJsonObject& json) const;

signals:
	/*!
	* Signal generated when a message is logged by an \class OpenGLView instance.
	*/
	void log(Message message) const;

private:
	unsigned int cubeSideLength; /*! Side length of cubes in this model in steps */
	unsigned int cubeMargin; /*! Size of margin in steps between cubes in the horizonal plane */
	unsigned int numLayers = 6; /*! Number of vertical layers cubes may be placed in */
	float rotationAngleResolution = 1.8; /*! Resolution of one rotation step about the vertical y-axis in degrees */
	Cube* selectedCube = Q_NULLPTR; /*! Reference to the cube currently in the selected state */
	unsigned int lastCubeID = 0; /*! Cube identifier of most recently inserted cube */
	QList<Cube*> cubes; /*! List of cubes that have been inserted into the cube world */
};

