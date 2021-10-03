#include "Robot.h"

Robot::Robot(QSerialPort* port)
{
	this->port = port;
}

void Robot::calibrate()
{
    // Form robot calibration packet
    Packet packet;
    packet.setControl(2); // Set calibration control byte

    // Transmit packet
    char buffer[9];
    packet.getTransmissionData(buffer);
    port->write(buffer, packet.getSize());
}

void Robot::sleep()
{

}

void Robot::wake()
{

}

void Robot::setPosition(int xPos, int yPos, int zPos, int rPos)
{

}

void Robot::actuateGripper()
{

}


void Robot::releaseGripper()
{

}


void Robot::resetGripper()
{

}