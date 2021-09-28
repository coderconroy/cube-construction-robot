#include "usart_hal.h"

// USART TX state variables
uint8_t tx_count = 0; // Number of bytes transmitted in transmission operation
uint8_t tx_size = 0; // Total number of bytes to transmit in transmission operation
uint8_t* tx_data = 0; // Bytes to be transmitted
bool tx_busy = false; // Flag to indicate USART data is busy being transmitted

// USART RX state variables
uint8_t rx_buffer[RX_BUFFER_SIZE]; // Buffer to store received bytes
uint8_t rx_count = 0; // Number of bytes available in rx_buffer

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
	__disable_irq();
	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_ClearPendingIRQ(USART1_IRQn);
	__enable_irq();
}

void usart_transmit(uint8_t* data, uint8_t size)
{
	// No data provided for transmission
	if (size <= 0)
		return;

	// Initialize transmission state variables
	tx_busy = true;
	tx_data = data;
	tx_size = size;
	tx_count = 0;

	// Transmit first byte
	USART1->TDR = tx_data[tx_count++];
}

void usart_receive(uint8_t* data, uint8_t size)
{

}

uint8_t bytes_available()
{
	return rx_count;
}

void usart_handle_interrupt()
{
	// Transmission complete interrupt
	if ((USART1->ISR & USART_ISR_TC) == USART_ISR_TC)
	{
		// Clear transmission complete flag
		USART1->ICR |= USART_ICR_TCCF;
		// Transmit Byte
		if (tx_count < tx_size)
			USART1->TDR = tx_data[tx_count++];
		else
			tx_busy = false;
	}
	// Byte received interrupt
	else if ((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
	{
		rx_buffer[rx_count++] = USART1->RDR; // Store byte
	}
	// Receiver overrun error interrupt
	else if ((USART1->ISR & USART_ISR_ORE) == USART_ISR_ORE)
	{

	}
}
