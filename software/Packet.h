#pragma once
#include <stdint.h>

/*!
* Defines the data structure which used to package data for communication between the robotic system 
* and the PC software by means of serial communication. A packet consists of a control byte which indicates
* the source and the purpose of the packet. The data bytes provide subsequent information whose meaning is
* determined by the control byte.
*/
class Packet
{
public:
	/*!
	* Class construtor.
	*/
	Packet();

	/*!
	* Get packet control byte.
	* @return Control byte.
	*/
	uint8_t getControl() const;

	/*!
	* Set packet control byte.
	* \param [in] Control byte.
	*/
	void setControl(const uint8_t control);

	/*!
	* Get packet data word (with word size of 16 bits)
	* \param [in] word Index of data word to be retrieved.
	* \return Data word.
	*/
	uint16_t getDataWord(const int word) const;

	/*!
	* Set packet data word (with word size of 16 bits)
	* \param [in] word Index of data word to be set.
	* \param [in] value Value of data word.
	*/
	void setDataWord(const int word, const uint16_t value);

	/*!
	* Get packet data (with word size of 16 bits) for all 4 data words.
	* \return [in] Data words
	*/
	const uint16_t* getData() const;

	/*!
	* Set packet data (with word size of 16 bits) for all 4 data words.
	* A deep copy is made of data argument array.
	* \param [in] Source of data words
	*/
	void setData(const uint16_t* data);

	/*!
	* Get packet in form of char array structured for UART transmission.
	* \param [out] Detination array for packet information.
	*/
	void getTransmissionData(char* const data) const;

	/*!
	* Get the number of bytes in the packet.
	* \return Packet byte size.
	*/
	int getSize() const;

private:
	uint8_t control; /*! Packet control byte */
	uint16_t data[4]; /*! Packet data words */
};