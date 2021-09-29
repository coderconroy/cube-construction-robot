#include "main.h"
#include "stdbool.h"



int main(void)
{
	// Configure the system clock, flash memory and power settings
	initialize_system();

	// Configure peripherals
	initialize_gpio_hal();
	initialize_usart_hal();
	initialize_adc();
	initialize_tim2();
	initialize_tim6();
	initialize_tim7();
	initialize_tim21();
	initialize_tim22();

	motor_calibrate();

	while(1)
	{

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

void initialize_adc()
{
	// Enable ADC peripheral clock
	 RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	 // Calibrate ADC
	 ADC1->CR |= ADC_CR_ADCAL; // Start ADC calibration
	 while ((ADC1->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL); // Wait for ADC calibration to complete

	 // Configure ADC
	 ADC1->CHSELR |= ADC_CHSELR_CHSEL3; // Enable channel 3 for conversion
	 ADC1->IER |= ADC_IER_EOCIE; // Enable end of conversion interrupt
	 ADC1->ISR |= ADC_ISR_ADRDY; // Set ADC to not ready
	 ADC1->CR |= ADC_CR_ADEN; // Enable ADC
	 while ((ADC1->ISR & ADC_ISR_ADRDY) != ADC_ISR_ADRDY); // Wait for ADC to be ready

	// Enable ADC interrupt line in NVIC
	__disable_irq();
	NVIC_EnableIRQ(ADC1_COMP_IRQn);
	NVIC_SetPriority(ADC1_COMP_IRQn, 0);
	__enable_irq();
}

void initialize_tim2()
{
	// Enable TIM2 peripheral clock
	 RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	 // Configure TIM2 for PWM
	 TIM2->PSC = 31; // Clock prescalar
	 TIM2->ARR = 19999; // Auto reload value
	 TIM2->CCR3 = 1000; // Channel 3 compare value
	 TIM2->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 ; // PWM mode 1
	 TIM2->CCMR2 |= TIM_CCMR2_OC3PE; // Enable output compare 3 preload
	 TIM2->CCER |= TIM_CCER_CC3E; // Compare 3 output enable
	 TIM2->CR1 |= TIM_CR1_CEN; // Enable timer
	 TIM2->EGR = TIM_EGR_UG; // Enable update generation
}

void initialize_tim6()
{
	// Enable TIM6 peripheral clock
	 RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

	// Configure TIM6
	TIM6->PSC = (uint16_t) 31; // Set prescalar
	TIM6->ARR = (uint16_t) 199; // Set auto-reload register value
	TIM6->DIER |= TIM_DIER_UIE; // Enable update interrupt
	TIM6->CR1 |= TIM_CR1_CEN; // Enable counter

	// Enable TIM6 interrupt line in NVIC
	__disable_irq();
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
	NVIC_SetPriority(TIM6_DAC_IRQn, 0);
	__enable_irq();
}

void initialize_tim7()
{
	// Enable TIM7 peripheral clock
	 RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

	// Configure TIM7
	TIM7->PSC = (uint16_t) 31; // Set prescalar
	TIM7->ARR = (uint16_t) 199; // Set auto-reload register value
	TIM7->DIER |= TIM_DIER_UIE; // Enable update interrupt
	TIM7->CR1 |= TIM_CR1_CEN; // Enable counter

	// Enable TIM7 interrupt line in NVIC
	__disable_irq();
	NVIC_EnableIRQ(TIM7_IRQn);
	NVIC_SetPriority(TIM7_IRQn, 0);
	__enable_irq();
}

void initialize_tim21()
{
	// Enable TIM21 peripheral clock
	 RCC->APB2ENR |= RCC_APB2ENR_TIM21EN;

	// Configure TIM21
	TIM21->PSC = (uint16_t) 31; // Set prescalar
	TIM21->ARR = (uint16_t) 99; // Set auto-reload register value
	TIM21->DIER |= TIM_DIER_UIE; // Enable update interrupt
	TIM21->CR1 |= TIM_CR1_CEN; // Enable counter

	// Enable TIM21 interrupt line in NVIC
	__disable_irq();
	NVIC_EnableIRQ(TIM21_IRQn);
	NVIC_SetPriority(TIM21_IRQn, 0);
	__enable_irq();
}

void initialize_tim22()
{
	// Enable TIM22 peripheral clock
	 RCC->APB2ENR |= RCC_APB2ENR_TIM22EN;

	// Configure TIM22
	TIM22->PSC = (uint16_t) 31; // Set prescalar
	TIM22->ARR = (uint16_t) 199; // Set auto-reload register value
	TIM22->DIER |= TIM_DIER_UIE; // Enable update interrupt
	TIM22->CR1 |= TIM_CR1_CEN; // Enable counter

	// Enable TIM22 interrupt line in NVIC
	__disable_irq();
	NVIC_EnableIRQ(TIM22_IRQn);
	NVIC_SetPriority(TIM22_IRQn, 0);
	__enable_irq();
}

void USART1_IRQHandler()
{
	usart_handle_interrupt();
}

void TIM6_DAC_IRQHandler()
{
	// TIM6 update interrupt
	if ((TIM6->SR & TIM_SR_UIF) == TIM_SR_UIF)
	{
		TIM6->SR &= ~TIM_SR_UIF; // Clear interrupt flag
		motor_x_execute_step();
	}
}

void TIM7_IRQHandler()
{
	// TIM7 update interrupt
	if ((TIM7->SR & TIM_SR_UIF) == TIM_SR_UIF)
	{
		TIM7->SR &= ~TIM_SR_UIF; // Clear interrupt flag
		motor_y_execute_step();
	}
}

void TIM21_IRQHandler()
{
	// TIM21 update interrupt
	if ((TIM21->SR & TIM_SR_UIF) == TIM_SR_UIF)
	{
		TIM21->SR &= ~TIM_SR_UIF; // Clear interrupt flag
		motor_z_execute_step();
	}
}

void TIM22_IRQHandler()
{
	// TIM22 update interrupt
	if ((TIM22->SR & TIM_SR_UIF) == TIM_SR_UIF)
	{
		TIM22->SR &= ~TIM_SR_UIF; // Clear interrupt flag
		motor_r_execute_step();
	}
}

uint16_t value;
void ADC_COMP_IRQHandler()
{
	// ADC end of conversion interrupt
	if ((ADC1->ISR & ADC_ISR_EOC) == ADC_ISR_EOC)
	{
		value = ADC1->DR; // Read ADC (clears EOC interrupt flag)
		usart_transmit((uint8_t*) &value, 2);
	}
}

// General error handling
void error_handler()
{
	__disable_irq();
	while (1);
}
