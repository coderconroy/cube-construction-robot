#include "main.h"

// USART TX state variables
uint8_t tx_count = 0;
uint8_t tx_size = 0;
uint8_t* tx_data = 0;
bool tx_busy = false; // Flag to indicate USART data is currently being transmitted

// USART RX state variables
uint8_t rx_buffer[255]; // Buffer to store received bytes
uint8_t rx_count = 0; // Number of bytes in rx_buffer

// General
uint8_t* buffer =(uint8_t*) "Hello\n";

int main(void)
{
	// Configure the system clock, flash memory and power settings
	initialize_system();

	// Configure peripherals
	initialize_gpio();
	initialize_usart();
	initialize_tim2();
	initialize_adc();

	while(1)
	{
		gpio_pin_toggle(LED0_GPIO_Port, LED0_Pin);
		gpio_pin_reset(LED1_GPIO_Port, LED1_Pin);
		delay(1000000);

		ADC1->CR |= ADC_CR_ADSTART; // Start conversion

		gpio_pin_toggle(LED0_GPIO_Port, LED0_Pin);
		gpio_pin_set(LED1_GPIO_Port, LED1_Pin);
		delay(1000000);

		// Echo bytes
		usart_transmit(rx_buffer, rx_count);
		rx_count = 0;
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
	// Enable GPIO port peripheral clocks
	RCC->IOPENR |= RCC_IOPENR_IOPAEN | RCC_IOPENR_IOPBEN; // GPIOA, GPIOB

	configure_gpio_pin(GPIOA, GPIO_PIN_0, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PA0 (LED0)
	configure_gpio_pin(GPIOA, GPIO_PIN_1, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PA1 (LED1)

	// Configure PA2 (V_SERVO)
	GPIOA->AFR[0] = (GPIOA->AFR[0] & (~GPIO_AFRL_AFSEL2)) | (0x2 << GPIO_AFRL_AFSEL2_Pos) ; // Alternate function selection = AF2 (TIM2_CH3)
	configure_gpio_pin(GPIOA, GPIO_PIN_2, ALTERNATE_FUNCTION, PUSH_PULL, LOW, NO_PUPD);

	configure_gpio_pin(GPIOA, GPIO_PIN_3, ANALOG, PUSH_PULL, LOW, NO_PUPD); // Configure PA3 (V_SENSOR)

	// Configure PB6 (USART TX)
	GPIOB->AFR[0] = (GPIOB->AFR[0] & (~GPIO_AFRL_AFSEL6)) | (0x0 << GPIO_AFRL_AFSEL6_Pos) ; // Alternate function selection = AF0 (USART1_TX)
	configure_gpio_pin(GPIOB, GPIO_PIN_6, ALTERNATE_FUNCTION, PUSH_PULL, VERY_HIGH, NO_PUPD);

	// Configure PB7 (USART RX)
	GPIOB->AFR[0] = (GPIOB->AFR[0] & (~GPIO_AFRL_AFSEL7)) | (0x0 << GPIO_AFRL_AFSEL7_Pos) ; // Alternate function selection = AF0 (USART1_RX)
	configure_gpio_pin(GPIOB, GPIO_PIN_7, ALTERNATE_FUNCTION, PUSH_PULL, VERY_HIGH, NO_PUPD);
}

void initialize_usart()
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

void initialize_tim2()
{
	// Enable TIM2 peripheral clock
	 RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	 // Configure TIM2 for PWM
	 TIM2->PSC = 31; // Clock prescalar
	 TIM2->ARR = 20000; // Auto reload value
	 TIM2->CCR3 = 1000; // Channel 3 compare value
	 TIM2->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 ; // PWM mode 1
	 TIM2->CCMR2 |= TIM_CCMR2_OC3PE; // Enable output compare 3 preload
	 TIM2->CCER |= TIM_CCER_CC3E; // Compare 3 output enable
	 TIM2->CR1 |= TIM_CR1_CEN; // Enable timer
	 TIM2->EGR = TIM_EGR_UG; // Enable update generation
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

void usart_transmit(uint8_t* data, uint8_t size)
{
	// TODO: Handle this error
	// USART is busy transmitting set of data
	if (tx_busy)
	{
		return;
//		error_handler();
	}

	// No data specified for transmission
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
	// Byte received interrupt
	else if ((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
	{
		rx_buffer[rx_count++] = USART1->RDR; // Store byte
	}
	// Receiver overrun error interrupt
	else if ((USART1->ISR & USART_ISR_ORE) == USART_ISR_ORE)
	{
		error_handler();
	}
}

void ADC_COMP_IRQHandler()
{
	// ADC end of conversion interrupt
	if ((ADC1->ISR & ADC_ISR_EOC) == ADC_ISR_EOC)
	{
		uint16_t value = ADC1->DR; // Read ADC (clears EOC interrupt flag)
		usart_transmit((uint8_t*) &value, 2);
	}
}

// General error handling
void error_handler()
{
	__disable_irq();
	while (1);
}
