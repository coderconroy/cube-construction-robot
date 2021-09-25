#include "main.h"
#include "stdbool.h"

// Function prototypes
void initialize_system();
void initialize_gpio();
void initialize_usart();
void usart_transmit(uint8_t*, uint8_t);
void delay(int);

// Variables
uint8_t buffer[10];
uint8_t tx_count = 0;
uint8_t tx_size = 0;
uint8_t* tx_data = 0;
bool tx_busy = false; // Flag to indicate USART data is currently being transmitted

int main(void)
{
	// Configure the system clock, flash memory and power settings
	initialize_system();

	// Configure peripherals
	initialize_gpio();
	initialize_usart();

	for (uint8_t i = 0; i < 10; i++)
		buffer[i] = i;

	while(1)
	{
		GPIOA->BSRR |= GPIO_BSRR_BS_1;
		delay(1000000);
		usart_transmit(buffer, 10);
		GPIOA->BSRR |= GPIO_BSRR_BR_1;
		delay(1000000);
	}

}

void initialize_system()
{
	// Configure HSI as system clock for initialization phase
	RCC->CR |= RCC_CR_HSION; // Enable HSI
	while ((RCC->CR & RCC_CR_HSIRDY) == 0 ); // Wait for HSI to stabilize
	RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_SW)) | RCC_CFGR_SW_HSI; // Set HSI as system clock
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI); // Wait for HSI to be selected as system clock source

	// Configure flash memory
	FLASH->ACR |= FLASH_ACR_LATENCY; // Use 1 wait state to read a word in the NVM

	// Configure internal voltage regulator operation range
	RCC->APB1ENR |= RCC_APB1ENR_PWREN; // Enable power interface clock
	while (PWR->CSR & PWR_CSR_VOSF);  // Wait for regulator to be ready in the selected voltage range
	PWR->CR = (PWR->CR & (~PWR_CR_VOS)) | (0x1 << PWR_CR_VOS_Pos); // Voltage scaling range selection = 1.8 V (range 1)
	while (PWR->CSR & PWR_CSR_VOSF);  // Wait for regulator to be ready in the selected voltage range

	// Initialize HSE
	RCC->CR |= RCC_CR_CSSHSEON; // Enable clock security system on HSE
	RCC->CR |= RCC_CR_HSEON; // Enable HSE
	while ((RCC->CR & RCC_CR_HSERDY) == 0); // Wait for HSE to stabilize

	// Configure peripheral clock dividers
	RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_PPRE2)) | RCC_CFGR_PPRE2_DIV1; // HCLK not divided for APB high-speed prescalar
	RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_PPRE1)) | RCC_CFGR_PPRE1_DIV1; // HCLK not divided for APB low-speed prescalar
	RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_HPRE)) | RCC_CFGR_HPRE_DIV1; // SYSCLK not divided for AHB high-speed prescalar

	// Initialize PLL
	RCC->CR &= ~RCC_CR_PLLON; // Disable PLL
	while (RCC->CR & RCC_CR_PLLRDY); // Wait for PLL to unlock
	RCC->CFGR |= RCC_CFGR_PLLSRC; // Select HSE oscillator clock as PLL input clock
	RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_PLLMUL)) | RCC_CFGR_PLLMUL4; // PLLVCO = PLL clock entry x 4
	RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_PLLDIV)) | RCC_CFGR_PLLDIV2; // PLL clock output = PLLVCO / 2
	RCC->CR |= RCC_CR_PLLON; // Enable PLL
	while ((RCC->CR & RCC_CR_PLLRDY) == 0); // Wait for PLL to lock

	// Configure system clock
	RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_SW)) | RCC_CFGR_SW_PLL; // Set PLL as system clock;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); // Wait for PLL to be selected as system clock source
}

void initialize_gpio()
{
	// Enable GPIO port clocks
	RCC->IOPENR |= RCC_IOPENR_IOPAEN | RCC_IOPENR_IOPBEN; // GPIOA, GPIOB

	// Configure PA1
	GPIOA->MODER |= GPIO_MODER_MODE1_0;
	GPIOA->MODER &= ~(GPIO_MODER_MODE1_1); // General purpose output mode
	GPIOA->OTYPER &= ~ (GPIO_OTYPER_OT_1);  //  Output push-pull
	GPIOA->OSPEEDR  |= GPIO_OSPEEDER_OSPEED1; // Very high speed
	GPIOA->PUPDR &= ~ (GPIO_PUPDR_PUPD0_1);   // No pull-up, no pull-down

	// Configure PB6 (USART TX)
	GPIOB->AFR[0] = (GPIOB->AFR[0] & (~GPIO_AFRL_AFSEL6)) | (0x0 << GPIO_AFRL_AFSEL6_Pos) ; // Alternate function selection = AF0 (USART1_TX)
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT_6; // Output push-pull
	GPIOB->PUPDR = (GPIOB->PUPDR & (~GPIO_PUPDR_PUPD6)) | (0x0 << GPIO_PUPDR_PUPD6_Pos); // No pull-up, pull-down
	GPIOB->OSPEEDR = (GPIOB->OSPEEDR & (~GPIO_OSPEEDER_OSPEED6)) | (0x3 << GPIO_OSPEEDER_OSPEED6_Pos); // Very high output speed
	GPIOB->MODER = (GPIOB->MODER & (~GPIO_MODER_MODE6)) | (0x2 << GPIO_MODER_MODE6_Pos);
}

void initialize_usart()
{
	// Enable USART1 clock
	RCC->APB2ENR |=RCC_APB2ENR_USART1EN;
	// Configure USART1 TX
	USART1->CR1 &= ~ (USART_CR1_M1 |  USART_CR1_M0); // Start bit, 8 data bits, n stop bits
	USART1->BRR = 0x116; // Baud rate = 115200 bits/s
	USART1->CR2 &= ~(USART_CR2_STOP_1 | USART_CR2_STOP_0); // 1 stop bit
	USART1->CR1 |= USART_CR1_UE; // Enable USART
	USART1->CR1 |= USART_CR1_TE; // Enable transmitter
	USART1->CR1 |= USART_CR1_TCIE; // Enable transmission complete interrupt
	USART1->ICR |= USART_ICR_TCCF; // Clear transmission complete interrupt flag

	__disable_irq();
	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_ClearPendingIRQ(USART1_IRQn);
	__enable_irq();
}

void usart_transmit(uint8_t* data, uint8_t size)
{
	// TODO: Handle this error
	// USART is busy transmitting set of data
	if (tx_busy)
		return;

	// No data specified for transmission
	if (size <= 0)
		return;

	// Initialize transmission status variables
	tx_busy = true;
	tx_data = data;
	tx_size = size;
	tx_count = 0;

	// Transmit first byte
	USART1->TDR = tx_data[tx_count++];
}

void delay(int count)
{
	volatile int i = 0;
	while (i < count) i++;
}

void USART1_IRQHandler()
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

}
