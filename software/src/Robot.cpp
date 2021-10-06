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
    transmitPacket(packet);
}

void Robot::sleep()
{
    // Form packet
    Packet packet;
    packet.setControl(1);
    packet.setDataWord(0, 0);

    // Transmit packet
    transmitPacket(packet);
}

void Robot::wake()
{
    // Form packet
    Packet packet;
    packet.setControl(1);
    packet.setDataWord(0, 1);

    // Transmit packet
    transmitPacket(packet);
}

void Robot::setPosition(int xPos, int yPos, int zPos, int rPos)
{
    // Form packet
    Packet packet;
    packet.setControl(3);
    packet.setDataWord(0, xPos);
    packet.setDataWord(1, yPos);
    packet.setDataWord(2, zPos);
    packet.setDataWord(3, rPos);

    // Transmit packet
    transmitPacket(packet);
}

void Robot::actuateGripper()
{
    // Form packet
    Packet packet;
    packet.setControl(4);
    packet.setDataWord(0, 1);

    // Transmit packet
    transmitPacket(packet);
}


void Robot::releaseGripper()
{
    // Form packet
    Packet packet;
    packet.setControl(4);
    packet.setDataWord(0, 2);

    // Transmit packet
    transmitPacket(packet);
}


void Robot::resetGripper()
{
    // Form packet
    Packet packet;
    packet.setControl(4);
    packet.setDataWord(0, 0);

    // Transmit packet
    transmitPacket(packet);
}

void Robot::transmitPacket(const Packet &packet)
{
    char buffer[9];
    packet.getTransmissionData(buffer);
    port->write(buffer, packet.getSize());
}

void Robot::delay()
{
    // Form packet
    Packet packet;
    packet.setControl(5);

    // Transmit packet
    transmitPacket(packet);
}