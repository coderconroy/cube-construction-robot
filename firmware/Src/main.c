#include "main.h"

void DelayTime(int count)
{
	volatile int i = 0;
	while (i < count) i++;
}

int main(void)
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

	// Enable clock for GPIOA
	RCC->IOPENR |= RCC_IOPENR_IOPAEN;
	// Configure PA1
	GPIOA->MODER |= GPIO_MODER_MODE1_0;
	GPIOA->MODER &= ~(GPIO_MODER_MODE1_1); // General purpose output mode
	GPIOA->OTYPER &= ~ (GPIO_OTYPER_OT_1);  //  Output push-pull
	GPIOA->OSPEEDR  |= GPIO_OSPEEDER_OSPEED1; // Very high speed
	GPIOA->PUPDR &= ~ (GPIO_PUPDR_PUPD0_1);   // No pull up, no pull down

	while(1)
	{
		GPIOA->BSRR |= GPIO_BSRR_BS_1;
		DelayTime(1000000);
		GPIOA->BSRR |= GPIO_BSRR_BR_1;
		DelayTime(1000000);
	}
}


