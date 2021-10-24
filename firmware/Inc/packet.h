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

typedef enum
{
	PACKET_WAKE = 0x1,
	PACKET_CALIBRATE = 0x2,
	PACKET_MOTOR = 0x3,
	PACKET_VACUUM = 0x4,
	PACKET_DELAY = 0x5,
	PACKET_PRESSURE = 0x6
} packet_type_t;

#endif /* PACKET_H_ */
