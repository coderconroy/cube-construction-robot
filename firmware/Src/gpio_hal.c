#include "gpio_hal.h"

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
