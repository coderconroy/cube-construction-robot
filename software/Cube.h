#pragma once

#include <QObject>
#include <glm/glm.hpp>

/*!
* Represents a construction cube in terms of its size, 3D centroid position and orientation.
* The position of the cube is represented using Cartesian coordinates with respect to the world frame.
* The local cube frame assumes a right-hand coordinate system. The orientation of the cube is parameterized
* using Euler angles where phi, theta and psi correspond to rotation about the x, y and z axes respectively.
*/
class Cube: public QObject
{
	Q_OBJECT
public:
	/*!
	* Class constructor.
	* \param [in] sideLength Length of the cube edges in terms of steps.
	* \param [in] parent Parent widget.
	*/
	Cube(unsigned int& sideLength, QObject* parent);

	/*!
	* Cube position setter.
	* \param [in] position Cartesian position of the cube centre (x, y, z) in steps.
	*/
	void setPosition(const glm::vec3& position);

	/*!
	* Cube orientation setter.
	* \param [in] orientation Orientation of cube parameterized using Euler angles (phi, theta, psi) in radians.
	*/
	void setOrientation(const glm::vec3& orientation);

	/*!
	* Cube side length getter.
	* \return Length of the cube edges in terms of steps.
	*/
	unsigned int getSideLength();

	/*!
	* Cube position getter.
	* \return Cartesian position of the cube centre (x, y, z) in steps.
	*/
	glm::vec3 getPosition() const;

	/*!
	* Cube orientation setter.
	* \return Orientation of cube parameterized using Euler angles (phi, theta, psi) in radians.
	*/
	glm::vec3 getOrientation() const;

private:
	unsigned int sideLength; /*! Length of the cube edges in terms of steps */
	glm::vec3 position; /*! Cartesian position of the cube centre (x, y, z) in steps */
	glm::vec3 orientation; /*! Orientation of cube parameterized using Euler angles (phi, theta, psi) in radians */
};

