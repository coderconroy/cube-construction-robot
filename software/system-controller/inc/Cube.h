#pragma once

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <glm/glm.hpp>

/*!
* Enumeration for representing the state of the cube in an external context.
*/
enum class CubeState
{
	VALID,	  // Cube is not selected and does not intersect any other cube's margin space.
	SELECTED, // Cube is selected ready for position and orientation updates.
	COLLISION // Cube is not selected but intersectes at least one other cubes's margin space.
};

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
	* \param [in] parent Parent \class QObject.
	*/
	Cube(QObject* parent);

	/*!
	* Class constructor.
	* \param [in] cubeID Unique cube identifier.
	* \param [in] sideLength Length of the cube edges in terms of steps.
	* \param [in] parent Parent \class QObject.
	*/
	Cube(const unsigned int cubeID, const unsigned int sideLength, QObject* parent);

	/*!
	* Cube state setter.
	* \param [in] state Cube state.
	*/
	void setState(const CubeState& state);

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
	* Cube ID getter.
	* \return Cube ID
	*/
	unsigned int getCubeID() const;

	/*!
	* Cube side length getter.
	* \return Length of the cube edges in terms of steps.
	*/
	unsigned int getSideLength() const;

	/*!
	* Cube state getter.
	* \return Cube state.
	*/
	CubeState getState() const;

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

	/*!
	* Getter for the cube's rotation about the x-axis (roll) in radians.
	* \return phi
	*/
	float getRoll() const;

	/*!
	* Getter for the cube's rotation about the y-axis (pitch) in radians.
	* \return theta
	*/
	float getPitch() const;

	/*!
	* Getter for the cube's rotation about the z-axis (yaw) in radians.
	* \return psi
	*/
	float getYaw() const;

	/*!
	* Load cube from a JSON object.
	* \param [in] json JSON object to load the cube from.
	*/
	void read(const QJsonObject& json);

	/*!
	* Write the cube to a JSON object.
	* \param [out] json JSON object to write the cube to.
	*/
	void write(QJsonObject& json) const;

private:
	CubeState state; /*! State of the cube in an external context */
	unsigned int cubeID; /*! Unique cube identifier */
	unsigned int sideLength; /*! Length of the cube edges in terms of steps */
	glm::vec3 position; /*! Cartesian position of the cube centre (x, y, z) in steps */
	glm::vec3 orientation; /*! Orientation of cube parameterized using Euler angles (phi, theta, psi) in radians */
};

