#ifndef USART_HAL_H_
#define USART_HAL_H_

// Includes
#include "stm32l072xx.h"
#include "stdbool.h"

// Defines
#define RX_BUFFER_SIZE 255 // Size of buffer to store received bytes before they are read

// Function prototypes

/**
 * Configures all the registers necessary for USART operation.
 */
void initialize_usart_hal();

/**
 * Transmit bytes via UART.
 *
 *@param [in] data Source bytes to be transmitted.
 *@param [in] size Number of bytes to transmit.
 */
void usart_transmit(uint8_t* data, uint8_t size);


/**
 * Read bytes via UART.
 * If \p size is greater than the number of bytes available, the remaining bytes will be read as 0x0.
 *
 *@param [out] data Destination of received bytes.
 *@param [in] size Number of bytes to transmit.
 */
void usart_receive(uint8_t* data, uint8_t size);

/**
 * Get the number of received bytes available.
 *
 * @return Number of bytes available.
 */
uint8_t bytes_available();

/**
 * Handles all interrupts associated with the USART. This must be placed in the USART ISR.
 */
void usart_handle_interrupt();

#endif /* USART_HAL_H_ */
