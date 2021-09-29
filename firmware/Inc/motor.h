#ifndef MOTOR_H_
#define MOTOR_H_

// Includes
#include "gpio_hal.h"
#include "stdbool.h"

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

// Motor direction defines
#define X_LEFT GPIO_PIN_LOW
#define X_RIGHT GPIO_PIN_HIGH
#define Y_FORWARD GPIO_PIN_LOW
#define Y_BACKWARD GPIO_PIN_HIGH
#define Z_UP GPIO_PIN_LOW
#define Z_DOWN GPIO_PIN_HIGH
#define R_CLOCKWISE GPIO_PIN_LOW
#define R_COUNTERCLOCKWISE GPIO_PIN_HIGH

// Motor range defines (max steps for each axis (1/32 microstepping)
#define X_MAX_POS 65000U
#define Y_MAX_POS 79000U
#define Z_MAX_POS 160000U
#define Z_MIN_POS 9000U
#define R_MAX_POS 5000 // Clockwise
#define R_MIN_POS - R_MAX_POS // Anti-clockwise

// Type definitions
typedef enum
{
	X_MOTOR = 0x0U,
	Y_MOTOR = 0x1U,
	Z_MOTOR = 0x2U,
	R_MOTOR = 0x3U
} motor_t;

typedef enum
{
	CALIBRATE,
	RUN
} motor_sys_state_t;

// Function prototypes

/**
 * Place all motors in the active state.
 */
void motor_enable_all();

/**
 * Place all motors in the sleep state.
 */
void motor_disable_all();

/**
 * Locate the limit switch reference point on each Cartesian axis.
 */
void motor_calibrate();

/**
 * Trigger motor to take next step for the X motor. The nature of the step depends on the motor state and configuration.
 */
void motor_x_execute_step();

/**
 * Trigger motor to take next step for the Y motor. The nature of the step depends on the motor state and configuration.
 */
void motor_y_execute_step();

/**
 * Trigger motor to take next step for the Z motor. The nature of the step depends on the motor state and configuration.
 */
void motor_z_execute_step();

/**
 * Trigger motor to take next step for the R motor. The nature of the step depends on the motor state and configuration.
 */
void motor_r_execute_step();

#endif /* MOTOR_H_ */
