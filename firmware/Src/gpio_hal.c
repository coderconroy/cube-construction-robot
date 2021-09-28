#include "gpio_hal.h"

void initialize_gpio_hal()
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

void configure_gpio_pin(GPIO_TypeDef* port, gpio_pin_t pin, gpio_moder_mode_t mode, gpio_otyper_ot_t type, gpio_ospeedr_ospeed_t speed, gpio_pupdr_pupd_t pupd)
{
	port->MODER = (port->MODER & (~(0x3<< (pin * 2)))) | (mode << (pin * 2)); // Port mode
	port->OTYPER = (port->OTYPER & (~(0x1<< pin))) | (type << pin); // Port mode
	port->OSPEEDR = (port->OSPEEDR & (~(0x3<< (pin * 2)))) | (speed << (pin * 2));; // Port output speed
	port->PUPDR = (port->PUPDR & (~(0x3<< (pin * 2)))) | (pupd << (pin * 2)); // Port pull-up/pull-down
}

void gpio_pin_set(GPIO_TypeDef* port, gpio_pin_t pin)
{
	port->BSRR |= 0x1 << pin;
}

void gpio_pin_reset(GPIO_TypeDef* port, gpio_pin_t pin)
{
	port->BSRR |= 0x1 << (pin + 0x10);
}

void gpio_pin_toggle(GPIO_TypeDef* port, gpio_pin_t pin)
{
	// Get pin state
	uint32_t state = port->ODR & (0x1 << pin);

	// Set pin to opposite value
	if (state == (0x1 << pin))
		gpio_pin_reset(port, pin);
	else
		gpio_pin_set(port, pin);
}
