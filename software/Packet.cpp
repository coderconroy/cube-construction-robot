#include "Packet.h"

Packet::Packet() 
{
	// Initialize member variables
	control = 0x0;
	for (uint8_t i = 0; i < 4; i++)
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


uint16_t Packet::getDataWord(const uint8_t word) const
{
	return data[word];
}


void Packet::setDataWord(const uint8_t word, const uint16_t value)
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