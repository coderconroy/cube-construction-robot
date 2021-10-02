#ifndef USART_HAL_H_
#define USART_HAL_H_

// Includes
#include "stm32l072xx.h"
#include "packet.h"

// Defines
#define TX_BUFFER_SIZE 256 // Size of buffer to store bytes waiting to be transmitted
#define RX_BUFFER_SIZE 256 // Size of buffer to store received bytes before they are read

// Function prototypes

/**
 * Configure all the registers necessary for USART operation.
 */
void initialize_usart_hal();

/**
 * Transmit bytes via UART.
 *
 *@param [in] data Source bytes to be transmitted.
 *@param [in] size Number of bytes to transmit.
 */
void usart_transmit(const uint8_t* data, const uint8_t size);


/**
 * Read bytes via UART.
 * If \p size is greater than the number of bytes available, the remaining bytes will be read as 0x0.
 *
 *@param [out] data Destination of received bytes.
 *@param [in] size Number of bytes to transmit.
 */
void usart_receive(uint8_t* const data, const uint8_t packet);

/**
 * Transmit packet via UART.
 *
 *@param [in] data Source packet to be transmitted.
 */
void usart_transmit_packet(const packet_t* packet);

/**
 * Read packet via UART.
 * If no packet is available, an empty packet is returned with all fields initialized to 0x0.
 *
 *@param [out] data Destination of received bytes.
 */
void usart_receive_packet(packet_t* const data);

/**
 * Get the number of received bytes available.
 *
 * @return Number of bytes available.
 */
uint8_t usart_bytes_available();

/**
 * Get the number of bytes that are in queue for waiting for transmission.
 *
 * @return Number of bytes waiting.
 */
uint8_t usart_bytes_waiting();

/**
 * Get the number of received packets available.
 *
 * @return Number of packets available.
 */
uint8_t usart_packets_available();

/**
 * Get the number of packets that are in queue for waiting for transmission.
 *
 * @return Number of packets waiting.
 */
uint8_t usart_packets_waiting();


/**
 * Handles all interrupts associated with the USART. This must be placed in the USART ISR.
 */
void usart_handle_interrupt();

#endif /* USART_HAL_H_ */
