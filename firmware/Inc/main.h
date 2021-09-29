#ifndef MAIN_H_
#define MAIN_H_

// Includes
#include "stm32l072xx.h"
#include "gpio_hal.h"
#include "usart_hal.h"
#include "motor.h"

// GPIO defines
#define LED0_Pin GPIO_PIN_0
#define LED0_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOA
#define V_SERVO_Pin GPIO_PIN_2
#define V_SERVO_GPIO_Port GPIOA
#define V_SENSOR_Pin GPIO_PIN_3
#define V_SENSOR_GPIO_Port GPIOA
#define USART1_TX_Pin GPIO_PIN_6
#define USART1_TX_GPIO_Port GPIOB
#define USART1_RX_Pin GPIO_PIN_7
#define USART1_RX_GPIO_Port GPIOB
#define FAN_Pin GPIO_PIN_8
#define FAN_GPIO_Port GPIOB
#define LIGHT_Pin GPIO_PIN_9
#define LIGHT_GPIO_Port GPIOB

// Constant defines
#define SERVO_PERIOD_MIN  500 // Pulse high time (microseconds)
#define SERVO_PERIOD_MAX  2400 // Pulse high time (microseconds)
#define SERVO_PERIOD_RELEASE SERVO_PERIOD_MAX
#define SERVO_PERIOD_IDLE 2100
#define SERVO_PERIOD_ACTUATE 1000

// Function prototypes
void initialize_system();
void initialize_tim2();
void initialize_tim6();
void initialize_tim7();
void initialize_tim21();
void initialize_tim22();
void initialize_adc();

/**
 * Set vacuum actuator actuation degree by through the PWM pulse width.
 *
 * @param pulse_width PWM pulse width (microseconds).
 */
void vacuum_actuate(uint16_t pulse_width);

void error_handler();

#endif /* MAIN_H_ */
