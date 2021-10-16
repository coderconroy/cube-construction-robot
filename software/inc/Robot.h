#pragma once

#include "Packet.h"
#include "Logger.h"
#include <QSerialPort>

/*!
* Defines an interface for interfacing with the robotic subsystem that abstracts the serial communication mechanism.
*/
class Robot: public QObject
{
	Q_OBJECT
public:
	/*!
	* Class constructor.
	* \param [in] parent Parent \class QObject.
	*/
	Robot(QObject* parent);

	/*!
	* Calibrate the reference positions for the X, Y and Z axes. The robot will perform a calibration sequence to find these positions using the
	* limit switches on each of the axes.
	*/
	void calibrate();

	/*!
	* Places all of the robotic subsystem motors in sleep mode. Note the robot must be recalibrated after performing this step.
	*/
	void sleep();

	/*!
	* Places all of the robotic subsystem motors in active mode.
	*/
	void wake();

	/*!
	* Set the serial port over which a connection with the robot has already been established.
	* \param[in] port Serial port 
	*/
	void setPort(QSerialPort* port);

	/*!
	* Instructs the robot to move the end effector to specified position and orientation.
	* \param [in] xPos X axis position (in full steps)
	* \param [in] yPos Y axis position (in full steps)
	* \param [in] zPos Z axis position (in full steps)
	* \param [in] rPos R axis (rotation about Z axis) position (in full steps)
	*/
	void setPosition(int xPos, int yPos, int zPos, int rPos);

	/*!
	* Set the vacuum actuator to ACTUATE positon. This generates a pressure lower than atmospheric pressure for the suction cup to grip an object.
	*/
	void actuateGripper();

	/*!
	* Set the vacuum actuator to RELEASE positon. This generates a pressure slightly higher than atmospheric pressure to ensure the object is
	* completely released.
	*/
	void releaseGripper();

	/*!
	* Set the vacuum actuator to the IDLE position. This is the default position of the actuator when not in the process of releasing or gripping an
	* object.
	*/
	void resetGripper();

	/*!
	* Pause briefly before continuing with next action
	*/
	void delay();

signals:
	/*!
	* Signal generated when robot indicates the most recent command has been completed.
	*/
	void commandCompleted() const;

	/*!
	* Generated when a message is logged by an \class Robot instance.
	*/
	void log(Message message) const;

private:
	QSerialPort* port = Q_NULLPTR; /*! Serial port for communication with robot */

	/*!
	* Transmit the packet over the serial port instance.
	* \param [in] packet Packet to transmit.
	*/
	void transmitPacket(const Packet &packet);

	/*!
	* Check if a packet has been received.
	*/
	void serialDataReceived();
};

