#pragma once

#include <QObject>
#include <glm/glm.hpp>

class Cube: public QObject
{
	Q_OBJECT
public:
	/*!
	* Class constructor
	*/
	Cube(QObject* parent);

	void setPosition(const glm::vec3& position);
	void setPose(const glm::vec3& pose);
	glm::vec3 getPosition() const;
	glm::vec3 getPose() const;
private:
	glm::vec3 position; /*! Cartesian position of the cube centre (x, y, z) */
	glm::vec3 pose; /*! Orientation of cube parameterized using Euler angles (phi, theta, psi) */
};

