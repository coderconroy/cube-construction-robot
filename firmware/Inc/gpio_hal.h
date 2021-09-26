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
void configure_gpio_pin(GPIO_TypeDef*, gpio_pin_t, gpio_moder_mode_t, gpio_otyper_ot_t, gpio_ospeedr_ospeed_t, gpio_pupdr_pupd_t);

#endif /* GPIO_HAL_H_ */
