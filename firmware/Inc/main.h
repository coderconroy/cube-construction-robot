#ifndef MAIN_H_
#define MAIN_H_

#include "stm32l072xx.h"
#include "gpio_hal.h"
#include "stdbool.h"

#define X_LIM_Pin GPIO_PIN_1
#define X_LIM_GPIO_Port GPIOC
#define Y_LIM_Pin GPIO_PIN_2
#define Y_LIM_GPIO_Port GPIOC
#define Z_LIM_Pin GPIO_PIN_3
#define Z_LIM_GPIO_Port GPIOC
#define LED0_Pin GPIO_PIN_0
#define LED0_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOA
#define V_SERVO_Pin GPIO_PIN_2
#define V_SERVO_GPIO_Port GPIOA
#define V_SENSOR_Pin GPIO_PIN_3
#define V_SENSOR_GPIO_Port GPIOA
#define R_MOT_FAULT_Pin GPIO_PIN_4
#define R_MOT_FAULT_GPIO_Port GPIOA
#define R_MOT_DIR_Pin GPIO_PIN_5
#define R_MOT_DIR_GPIO_Port GPIOA
#define R_MOT_STEP_Pin GPIO_PIN_6
#define R_MOT_STEP_GPIO_Port GPIOA
#define R_MOT_SLEEP_Pin GPIO_PIN_7
#define R_MOT_SLEEP_GPIO_Port GPIOA
#define R_MOT_RESET_Pin GPIO_PIN_4
#define R_MOT_RESET_GPIO_Port GPIOC
#define R_MOT_M2_Pin GPIO_PIN_5
#define R_MOT_M2_GPIO_Port GPIOC
#define R_MOT_M1_Pin GPIO_PIN_0
#define R_MOT_M1_GPIO_Port GPIOB
#define R_MOT_M0_Pin GPIO_PIN_1
#define R_MOT_M0_GPIO_Port GPIOB
#define Z_MOT_FAULT_Pin GPIO_PIN_2
#define Z_MOT_FAULT_GPIO_Port GPIOB
#define Z_MOT_DIR_Pin GPIO_PIN_10
#define Z_MOT_DIR_GPIO_Port GPIOB
#define Z_MOT_STEP_Pin GPIO_PIN_11
#define Z_MOT_STEP_GPIO_Port GPIOB
#define Z_MOT_SLEEP_Pin GPIO_PIN_12
#define Z_MOT_SLEEP_GPIO_Port GPIOB
#define Z_MOT_RESET_Pin GPIO_PIN_13
#define Z_MOT_RESET_GPIO_Port GPIOB
#define Z_MOT_M2_Pin GPIO_PIN_14
#define Z_MOT_M2_GPIO_Port GPIOB
#define Z_MOT_M0_Pin GPIO_PIN_15
#define Z_MOT_M0_GPIO_Port GPIOB
#define Z_MOT_M1_Pin GPIO_PIN_6
#define Z_MOT_M1_GPIO_Port GPIOC
#define Y_MOT_FAULT_Pin GPIO_PIN_7
#define Y_MOT_FAULT_GPIO_Port GPIOC
#define Y_MOT_DIR_Pin GPIO_PIN_8
#define Y_MOT_DIR_GPIO_Port GPIOC
#define Y_MOT_STEP_Pin GPIO_PIN_9
#define Y_MOT_STEP_GPIO_Port GPIOC
#define Y_MOT_SLEEP_Pin GPIO_PIN_8
#define Y_MOT_SLEEP_GPIO_Port GPIOA
#define Y_MOT_RESET_Pin GPIO_PIN_9
#define Y_MOT_RESET_GPIO_Port GPIOA
#define Y_MOT_M2_Pin GPIO_PIN_10
#define Y_MOT_M2_GPIO_Port GPIOA
#define Y_MOT_M1_Pin GPIO_PIN_11
#define Y_MOT_M1_GPIO_Port GPIOA
#define Y_MOT_M0_Pin GPIO_PIN_12
#define Y_MOT_M0_GPIO_Port GPIOA
#define X_MOT_FAULT_Pin GPIO_PIN_15
#define X_MOT_FAULT_GPIO_Port GPIOA
#define X_MOT_DIR_Pin GPIO_PIN_10
#define X_MOT_DIR_GPIO_Port GPIOC
#define X_MOT_STEP_Pin GPIO_PIN_11
#define X_MOT_STEP_GPIO_Port GPIOC
#define X_MOT_SLEEP_Pin GPIO_PIN_12
#define X_MOT_SLEEP_GPIO_Port GPIOC
#define X_MOT_RESET_Pin GPIO_PIN_2
#define X_MOT_RESET_GPIO_Port GPIOD
#define X_MOT_M2_Pin GPIO_PIN_3
#define X_MOT_M2_GPIO_Port GPIOB
#define X_MOT_M1_Pin GPIO_PIN_4
#define X_MOT_M1_GPIO_Port GPIOB
#define X_MOT_M0_Pin GPIO_PIN_5
#define X_MOT_M0_GPIO_Port GPIOB
#define USART1_TX_Pin GPIO_PIN_6
#define USART1_TX_GPIO_Port GPIOB
#define USART1_RX_Pin GPIO_PIN_7
#define USART1_RX_GPIO_Port GPIOB
#define FAN_Pin GPIO_PIN_8
#define FAN_GPIO_Port GPIOB
#define LIGHT_Pin GPIO_PIN_9
#define LIGHT_GPIO_Port GPIOB

// Function prototypes
void initialize_system();
void initialize_gpio();
void initialize_usart();
void usart_transmit(uint8_t*, uint8_t);
void delay(int);
void error_handler();

#endif /* MAIN_H_ */
