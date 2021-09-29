#ifndef MOTOR_H_
#define MOTOR_H_

// Includes
#include "gpio_hal.h"

// GPIO defines
#define X_LIM_Pin GPIO_PIN_1
#define X_LIM_GPIO_Port GPIOC
#define Y_LIM_Pin GPIO_PIN_2
#define Y_LIM_GPIO_Port GPIOC
#define Z_LIM_Pin GPIO_PIN_3
#define Z_LIM_GPIO_Port GPIOC
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

// Type definitions
typedef enum
{
	X_MOTOR = 0x0U,
	Y_MOTOR = 0x1U,
	Z_MOTOR = 0x2U,
	R_MOTOR = 0x3U
} motor_t;

// Function prototypes

/*
 * Place all motors in the active state.
 */
void motor_enable_all();

/*
 * Place all motors in the sleep state.
 */
void motor_disable_all();

/*
 * Trigger motor to take next step. The nature of the step depends on the motor state and configuration.
 *
 * @param motor Target motor for the command
 */
void motor_execute_step(const motor_t motor);

#endif /* MOTOR_H_ */
