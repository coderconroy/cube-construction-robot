#ifndef GPIO_HAL_H_
#define GPIO_HAL_H_

// Includes
#include "stm32l072xx.h"

// GPIO type definitions
typedef enum
{
	GPIO_PIN_0 = 0x0U,
	GPIO_PIN_1 = 0x1U,
	GPIO_PIN_2 = 0x2U,
	GPIO_PIN_3 = 0x3U,
	GPIO_PIN_4 = 0x4U,
	GPIO_PIN_5 = 0x5U,
	GPIO_PIN_6 = 0x6U,
	GPIO_PIN_7 = 0x7U,
	GPIO_PIN_8 = 0x8U,
	GPIO_PIN_9 = 0x9U,
	GPIO_PIN_10 = 0xAU,
	GPIO_PIN_11 = 0xBU,
	GPIO_PIN_12 = 0xCU,
	GPIO_PIN_13 = 0xDU,
	GPIO_PIN_14 = 0xEU,
	GPIO_PIN_15 = 0xFU
} gpio_pin_t;

typedef enum
{
	GPIO_PIN_LOW = 0x0U,
	GPIO_PIN_HIGH = 0x1U
} gpio_pin_state_t;

typedef enum
{
	INPUT = 0x0U,
	GENERAL_PURPOSE = 0x1U,
	ALTERNATE_FUNCTION = 0x2U,
	ANALOG = 0x3U
} gpio_moder_mode_t;

typedef enum
{
	PUSH_PULL = 0x0U,
	OPEN_DRAIN = 0x1U
} gpio_otyper_ot_t;

typedef enum
{
	LOW = 0x0U,
	MEDIUM = 0x1U,
	HIGH = 0x2U,
	VERY_HIGH = 0x3U
} gpio_ospeedr_ospeed_t;

typedef enum
{
	NO_PUPD = 0x0,
	PULL_UP = 0x1,
	PULL_DOWN = 0x2
} gpio_pupdr_pupd_t;

// Function prototypes

/**
 * Configure all the registers necessary for GPIO operation.
 */
void initialize_gpio_hal();

/**
 * Configure individual GPIO pin settings.
 *
 * @param [in] port GPIO port of pin
 * @param [in] pin GPIO pin number
 * @param [in] mode GPIO pin mode
 * @param [in] type GPIO pin output type
 * @param [in] speed GPIO pin output speed
 * @param [in] pupd Pull-up pull-down resistor configuration
 */
void configure_gpio_pin(GPIO_TypeDef* const port, const gpio_pin_t pin, const gpio_moder_mode_t mode, const gpio_otyper_ot_t type,
		const gpio_ospeedr_ospeed_t speed, const gpio_pupdr_pupd_t pupd);

/*
 * Set GPIO pin output to logic high.
 *
 * @param [in] port GPIO port of pin
 * @param [in] pin GPIO pin number
 */
void gpio_pin_set(GPIO_TypeDef* const port, const gpio_pin_t pin);

/*
 * Set GPIO pin output to logic low.
 *
 * @param [in] port GPIO port of pin
 * @param [in] pin GPIO pin number
 */
void gpio_pin_reset(GPIO_TypeDef* const port, const gpio_pin_t pin);

/*
 * Set GPIO pin logic state to the opposite of the current logic state.
 *
 * @param [in] port GPIO port of pin
 * @param [in] pin GPIO pin number
 */
void gpio_pin_toggle(GPIO_TypeDef* const port, const gpio_pin_t pin);

/*
 * Set GPIO pin logic state.
 *
 * @param [in] port GPIO port of pin
 * @param [in] pin GPIO pin number
 * @param [in] state Desired GPIO pin state
 */
void gpio_pin_write(GPIO_TypeDef* const port, const gpio_pin_t pin, const gpio_pin_state_t state);

/**
 * Get the current input logic state of the GPIO pin.
 *
 * @param [in] port GPIO port of pin
 * @param [in] pin GPIO pin number
 */
const gpio_pin_state_t gpio_pin_read(GPIO_TypeDef* const port, const gpio_pin_t pin);

#endif /* GPIO_HAL_H_ */
