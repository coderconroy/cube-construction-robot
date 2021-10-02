#ifndef PACKET_H_
#define PACKET_H_

// Defines
#define PACKET_SIZE 9 // Number of bytes in each packet

// Type definitions
typedef struct
{
	uint8_t control;
	uint16_t data[4];
} packet_t;

#endif /* PACKET_H_ */
