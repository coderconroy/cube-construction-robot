#include "usart_hal.h"

// USART TX state variables
uint8_t tx_buffer[TX_BUFFER_SIZE]; // Bytes to be transmitted
uint8_t tx_read_index = 0; // Index of next byte to be transmitted in TX buffer
uint8_t tx_write_index = 0; // Index of position in TX buffer next byte will be written to


// USART RX state variables
uint8_t rx_buffer[RX_BUFFER_SIZE]; // Buffer to store received bytes
uint8_t rx_read_index = 0; // Index of next byte to be read in RX buffer
uint8_t rx_write_index = 0; // Index of position in RX buffer next byte will be written to

void initialize_usart_hal()
{
	// Enable USART1 peripheral clock
	RCC->APB2ENR |=RCC_APB2ENR_USART1EN;

	// Configure USART1
	USART1->CR1 &= ~ (USART_CR1_M1 |  USART_CR1_M0); // Start bit, 8 data bits, n stop bits
	USART1->BRR = 0x116; // Baud rate = 115200 bits/s
	USART1->CR2 &= ~(USART_CR2_STOP_1 | USART_CR2_STOP_0); // 1 stop bit
	USART1->CR1 |= USART_CR1_UE; // Enable USART
	USART1->CR1 |= USART_CR1_TE; // Enable transmitter
	USART1->CR1 |= USART_CR1_RE; // Enable receiver
	USART1->CR1 |= USART_CR1_TCIE; // Enable transmission complete interrupt
	USART1->CR1 |= USART_CR1_RXNEIE; // Enable byte received interrupt
	USART1->ICR |= USART_ICR_TCCF; // Clear transmission complete interrupt flag
	USART1->RQR |= USART_RQR_RXFRQ;// Clear byte received interrupt flag

	// Enable USART1 interrupt line in NVIC
	// NOTE: The interrupt priority must be 0 as the interrupt handler assumes it cannot be interrupted
	__disable_irq();
	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_ClearPendingIRQ(USART1_IRQn);
	__enable_irq();
}

void usart_transmit(const uint8_t* data, const uint8_t size)
{
	// No data provided for transmission
	if (size <= 0)
		return;

	// Copy data to TX buffer
	for (uint8_t i = 0; i < size; i++)
	{
		tx_buffer[tx_write_index] = data[i];
		tx_write_index= (tx_write_index + 1) % TX_BUFFER_SIZE;
	}

	// Transmit first byte if there are no previous bytes ahead in the transmission queue
	if (usart_bytes_waiting() == size)
		USART1->TDR = tx_buffer[tx_read_index];
}

void usart_receive(uint8_t* const data, const uint8_t size)
{
	for (uint8_t i = 0; i < size; i++)
	{
		// Copy bytes from TX buffer to read destination
		if (usart_bytes_available() > 0)
			data[i] = rx_buffer[rx_read_index];
		else
			data[i] = 0x0;
		rx_read_index = (rx_read_index + 1) % RX_BUFFER_SIZE;
	}
}

uint8_t usart_bytes_available()
{
	return (((int16_t) rx_write_index) - rx_read_index) % RX_BUFFER_SIZE;
}

uint8_t usart_bytes_waiting()
{
	return (((int16_t) tx_write_index) - tx_read_index) % TX_BUFFER_SIZE;
}

void usart_handle_interrupt()
{
	// Transmission complete interrupt
	if ((USART1->ISR & USART_ISR_TC) == USART_ISR_TC)
	{
		// Clear transmission complete flag
		USART1->ICR |= USART_ICR_TCCF;

		// Update transmission state variables to reflect completed transmission
		tx_read_index = (tx_read_index + 1) % TX_BUFFER_SIZE;

		// Transmit byte
		if (usart_bytes_waiting())
			USART1->TDR = tx_buffer[tx_read_index];
	}
	// Byte received interrupt
	else if ((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
	{
		rx_buffer[rx_write_index] = USART1->RDR; // Store byte
		rx_write_index = (rx_write_index + 1) % RX_BUFFER_SIZE;
	}
	// Receiver overrun error interrupt
	else if ((USART1->ISR & USART_ISR_ORE) == USART_ISR_ORE)
	{

	}
}
