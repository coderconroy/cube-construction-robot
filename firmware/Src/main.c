#include "main.h"

// Function prototypes
void initialize_system();
void initialize_gpio();
void initialize_usart();
void delay(int);

int main(void)
{
	// Configure the system clock, flash memory and power settings
	initialize_system();

	// Configure peripherals
	initialize_gpio();
	initialize_usart();

	while(1)
	{
		GPIOA->BSRR |= GPIO_BSRR_BS_1;
		delay(1000000);
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
	RCC->IOPENR |= RCC_IOPENR_IOPAEN; // GPIOA
	RCC->IOPENR |= RCC_IOPENR_IOPBEN; // GPIOB

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
	/* (1) oversampling by 16, 9600 baud */
	/* (2) 8 data bit, 1 start bit, 1 stop bit, no parity */
//	USART1->BRR = 160000 / 96; /* (1) */
//	USART1->CR1 = USART_CR1_TE | USART_CR1_UE; /* (2) */

//	1. Program the M bits in USART_CR1 to define the word length.

//	2. Select the desired baud rate using the USART_BRR register.

//	3. Program the number of stop bits in USART_CR2.

//	4. Enable the USART by writing the UE bit in USART_CR1 register to 1.

//	5. Select DMA enable (DMAT) in USART_CR3 if multibuffer communication is to take
//	place. Configure the DMA register as explained in multibuffer communication.

//	6. Set the TE bit in USART_CR1 to send an idle frame as first transmission.

//	7. Write the data to send in the USART_TDR register (this clears the TXE bit). Repeat this
//	for each data to be transmitted in case of single buffer.

//	8. After writing the last data into the USART_TDR register, wait until TC=1. This indicates
//	that the transmission of the last frame is complete. This is required for instance when
//	the USART is disabled or enters the Halt mode to avoid corrupting the last
//	transmission.

}

void delay(int count)
{
	volatile int i = 0;
	while (i < count) i++;
}
