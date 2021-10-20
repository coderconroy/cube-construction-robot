#include "Packet.h"

Packet::Packet() 
{
	// Initialize member variables
	control = 0x0;
	for (int i = 0; i < 4; i++)
		data[i] = 0;
}

uint8_t Packet::getControl() const
{
	return control;
}


void Packet::setControl(const uint8_t control)
{
	this->control = control;
}


uint16_t Packet::getDataWord(const int word) const
{
	return data[word];
}


void Packet::setDataWord(const int word, const uint16_t value)
{
	data[word] = value;
}


const uint16_t* Packet::getData() const
{
	return data;
}


void Packet::setData(const uint16_t* data)
{
	// Make deep copy of input data
	for (uint8_t i = 0; i < 4; i++)
		this->data[i] = data[i];
}

void Packet::getTransmissionData(char* const data) const
{
	// Copy packet data to destination array
	data[0] = control;
	data[1] = this->data[0] & 0xFF;
	data[2] = this->data[0] >> 8;
	data[3] = this->data[1] & 0xFF;
	data[4] = this->data[1] >> 8;
	data[5] = this->data[2] & 0xFF;
	data[6] = this->data[2] >> 8;
	data[7] = this->data[3] & 0xFF;
	data[8] = this->data[3] >> 8;
}

int Packet::getSize() const
{
	return 9;
}