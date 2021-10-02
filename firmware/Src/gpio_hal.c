#include "gpio_hal.h"

void initialize_gpio_hal()
{
	// Enable GPIO port peripheral clocks
	RCC->IOPENR |= RCC_IOPENR_IOPAEN | RCC_IOPENR_IOPBEN | RCC_IOPENR_IOPCEN | RCC_IOPENR_IOPDEN;

	// Configure GPIO pins not using alternate functions
	configure_gpio_pin(GPIOA, GPIO_PIN_3, ANALOG, PUSH_PULL, LOW, NO_PUPD); // Configure PA3 (V_SENSOR)

	configure_gpio_pin(GPIOA, GPIO_PIN_0, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PA0 (LED0)
	configure_gpio_pin(GPIOA, GPIO_PIN_1, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PA1 (LED1)

	configure_gpio_pin(GPIOB, GPIO_PIN_8, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PB8 (FAN)
	configure_gpio_pin(GPIOB, GPIO_PIN_9, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PB9 (LIGHT)

	configure_gpio_pin(GPIOC, GPIO_PIN_1, INPUT, PUSH_PULL, LOW, PULL_UP); // Configure PC1 (X_LIM)
	configure_gpio_pin(GPIOC, GPIO_PIN_2, INPUT, PUSH_PULL, LOW, PULL_UP); // Configure PC2 (Y_LIM)
	configure_gpio_pin(GPIOC, GPIO_PIN_3, INPUT, PUSH_PULL, LOW, PULL_UP); // Configure PC3 (Z_LIM)

	configure_gpio_pin(GPIOB, GPIO_PIN_5, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PB5 (X_MOT_M0)
	configure_gpio_pin(GPIOB, GPIO_PIN_4, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PB4 (X_MOT_M1)
	configure_gpio_pin(GPIOB, GPIO_PIN_3, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PB3 (X_MOT_M2)
	configure_gpio_pin(GPIOD, GPIO_PIN_2, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PD2 (X_MOT_RESET)
	configure_gpio_pin(GPIOC, GPIO_PIN_12, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PC12 (X_MOT_SLEEP)
	configure_gpio_pin(GPIOC, GPIO_PIN_11, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PC11 (X_MOT_STEP)
	configure_gpio_pin(GPIOC, GPIO_PIN_10, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PC10 (X_MOT_DIR)
	configure_gpio_pin(GPIOA, GPIO_PIN_15, INPUT, PUSH_PULL, LOW, NO_PUPD); // Configure PA15 (X_MOT_FAULT)

	configure_gpio_pin(GPIOA, GPIO_PIN_12, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PA12 (Y_MOT_M0) (non-functional)
	configure_gpio_pin(GPIOA, GPIO_PIN_11, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PA11 (Y_MOT_M1) (non-functional)
	configure_gpio_pin(GPIOA, GPIO_PIN_10, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PA10 (Y_MOT_M2)
	configure_gpio_pin(GPIOA, GPIO_PIN_9, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PA9 (Y_MOT_RESET)
	configure_gpio_pin(GPIOA, GPIO_PIN_8, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PA8 (Y_MOT_SLEEP)
	configure_gpio_pin(GPIOC, GPIO_PIN_9, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PC9 (Y_MOT_STEP)
	configure_gpio_pin(GPIOC, GPIO_PIN_8, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PC8 (Y_MOT_DIR)
	configure_gpio_pin(GPIOC, GPIO_PIN_7, INPUT, PUSH_PULL, LOW, NO_PUPD); // Configure PC7 (Y_MOT_FAULT)

	configure_gpio_pin(GPIOC, GPIO_PIN_6, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PC6 (Z_MOT_M1)
	configure_gpio_pin(GPIOB, GPIO_PIN_15, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PB15 (Z_MOT_M0)
	configure_gpio_pin(GPIOB, GPIO_PIN_14, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PB14 (Z_MOT_M2)
	configure_gpio_pin(GPIOB, GPIO_PIN_13, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PB13 (Z_MOT_RESET)
	configure_gpio_pin(GPIOB, GPIO_PIN_12, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PB12 (Z_MOT_SLEEP)
	configure_gpio_pin(GPIOB, GPIO_PIN_11, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PB11 (Z_MOT_STEP)
	configure_gpio_pin(GPIOB, GPIO_PIN_10, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PB10 (Z_MOT_DIR)
	configure_gpio_pin(GPIOB, GPIO_PIN_2, INPUT, PUSH_PULL, LOW, NO_PUPD); // Configure PB2 (Z_MOT_FAULT)

	configure_gpio_pin(GPIOB, GPIO_PIN_1, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PB1 (R_MOT_M0)
	configure_gpio_pin(GPIOB, GPIO_PIN_0, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PB0 (R_MOT_M1)
	configure_gpio_pin(GPIOC, GPIO_PIN_5, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PC5 (R_MOT_M2)
	configure_gpio_pin(GPIOC, GPIO_PIN_4, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PC4 (R_MOT_RESET)
	configure_gpio_pin(GPIOA, GPIO_PIN_7, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PA7 (R_MOT_SLEEP)
	configure_gpio_pin(GPIOA, GPIO_PIN_6, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PA6 (R_MOT_STEP)
	configure_gpio_pin(GPIOA, GPIO_PIN_5, GENERAL_PURPOSE, PUSH_PULL, LOW, NO_PUPD); // Configure PA5 (R_MOT_DIR)
	configure_gpio_pin(GPIOA, GPIO_PIN_4, INPUT, PUSH_PULL, LOW, NO_PUPD); // Configure PA4 (R_MOT_FAULT)

	// Configure PA2 (V_SERVO)
	GPIOA->AFR[0] = (GPIOA->AFR[0] & (~GPIO_AFRL_AFSEL2)) | (0x2 << GPIO_AFRL_AFSEL2_Pos) ; // Alternate function selection = AF2 (TIM2_CH3)
	configure_gpio_pin(GPIOA, GPIO_PIN_2, ALTERNATE_FUNCTION, PUSH_PULL, LOW, NO_PUPD);

	// Configure PB6 (USART TX)
	GPIOB->AFR[0] = (GPIOB->AFR[0] & (~GPIO_AFRL_AFSEL6)) | (0x0 << GPIO_AFRL_AFSEL6_Pos) ; // Alternate function selection = AF0 (USART1_TX)
	configure_gpio_pin(GPIOB, GPIO_PIN_6, ALTERNATE_FUNCTION, PUSH_PULL, VERY_HIGH, NO_PUPD);

	// Configure PB7 (USART RX)
	GPIOB->AFR[0] = (GPIOB->AFR[0] & (~GPIO_AFRL_AFSEL7)) | (0x0 << GPIO_AFRL_AFSEL7_Pos) ; // Alternate function selection = AF0 (USART1_RX)
	configure_gpio_pin(GPIOB, GPIO_PIN_7, ALTERNATE_FUNCTION, PUSH_PULL, VERY_HIGH, NO_PUPD);
}

void configure_gpio_pin(GPIO_TypeDef* const port, const gpio_pin_t pin, const gpio_moder_mode_t mode, const gpio_otyper_ot_t type,
		const gpio_ospeedr_ospeed_t speed, const gpio_pupdr_pupd_t pupd)
{
	port->MODER = (port->MODER & (~(0x3<< (pin * 2)))) | (mode << (pin * 2)); // Port mode
	port->OTYPER = (port->OTYPER & (~(0x1<< pin))) | (type << pin); // Port output type
	port->OSPEEDR = (port->OSPEEDR & (~(0x3<< (pin * 2)))) | (speed << (pin * 2));; // Port output speed
	port->PUPDR = (port->PUPDR & (~(0x3<< (pin * 2)))) | (pupd << (pin * 2)); // Port pull-up/pull-down
}

void gpio_pin_set(GPIO_TypeDef* const port, const gpio_pin_t pin)
{
	port->BSRR |= 0x1 << pin;
}

void gpio_pin_reset(GPIO_TypeDef* const port, const gpio_pin_t pin)
{
	port->BSRR |= 0x1 << (pin + 0x10);
}

void gpio_pin_toggle(GPIO_TypeDef* const port, const gpio_pin_t pin)
{
	// Get pin state
	gpio_pin_state_t state = gpio_output_pin_read(port, pin);

	// Set pin to opposite value
	if (state == GPIO_PIN_LOW)
		gpio_pin_set(port, pin);
	else
		gpio_pin_reset(port, pin);
}

void gpio_pin_write(GPIO_TypeDef* const port, const gpio_pin_t pin, const gpio_pin_state_t state)
{
	if (state == GPIO_PIN_LOW)
		gpio_pin_reset(port, pin);
	else
		gpio_pin_set(port, pin);
}

const gpio_pin_state_t gpio_input_pin_read(GPIO_TypeDef* const port, const gpio_pin_t pin)
{
	gpio_pin_state_t state;

	if ((port->IDR & (0x1 << pin)) == (GPIO_PIN_LOW << pin))
		state = GPIO_PIN_LOW;
	else
		state = GPIO_PIN_HIGH;

	return state;
}

const gpio_pin_state_t gpio_output_pin_read(GPIO_TypeDef* const port, const gpio_pin_t pin)
{
	gpio_pin_state_t state;

	if ((port->ODR & (0x1 << pin)) == (GPIO_PIN_LOW << pin))
		state = GPIO_PIN_LOW;
	else
		state = GPIO_PIN_HIGH;

	return state;
}
