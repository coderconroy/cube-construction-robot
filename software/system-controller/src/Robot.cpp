#include "Robot.h"

Robot::Robot(QObject* parent) : QObject(parent)
{

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

void Robot::setPort(QSerialPort* port)
{
    this->port = port;
    connect(port, &QSerialPort::readyRead, this, &Robot::serialDataReceived);
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

void Robot::requestPressureReading()
{
    // Form packet
    Packet packet;
    packet.setControl(6);

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

void Robot::serialDataReceived()
{
    while (port->bytesAvailable() >= 9)
    {
        QByteArray data = port->read(9);
        Packet packet;
        packet.setControl((uint8_t) data[0]);
        packet.setDataWord(0, (uint8_t) data[1] | ((uint8_t) data[2] << 8));
        packet.setDataWord(1, (uint8_t) data[3] | ((uint8_t) data[4] << 8));
        packet.setDataWord(2, (uint8_t) data[5] | ((uint8_t) data[6] << 8));
        packet.setDataWord(3, (uint8_t) data[7] | ((uint8_t) data[8] << 8));

        switch (packet.getControl())
        {
        case 0x1:
            emit commandCompleted();
            break;
        case 0x6:
            emit log(Message(MessageType::INFO_LOG, "Robot", QString::number(packet.getDataWord(0))));
            break;
        }
    }
}