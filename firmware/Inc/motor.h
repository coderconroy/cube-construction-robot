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

//// Motor base timer defines
//#define X_MOT_TIM TIM6
//#define Y_MOT_TIM TIM7
//#define Z_MOT_TIM TIM21
//#define R_MOT_TIM TIM21

// Motor direction defines
#define X_LEFT GPIO_PIN_LOW
#define X_RIGHT GPIO_PIN_HIGH
#define Y_FORWARD GPIO_PIN_LOW
#define Y_BACKWARD GPIO_PIN_HIGH
#define Z_UP GPIO_PIN_LOW
#define Z_DOWN GPIO_PIN_HIGH
#define R_CLOCKWISE GPIO_PIN_LOW
#define R_COUNTERCLOCKWISE GPIO_PIN_HIGH

// Motor range defines (in steps for each axis (1/32 microstepping)
// The maximum and minimum positions are defined relative to the reference position
#define X_REF_POS 0
#define X_MIN_POS X_REF_POS
#define X_MAX_POS 32500
#define Y_REF_POS 0
#define Y_MIN_POS Y_REF_POS
#define Y_MAX_POS 39500
#define Z_REF_POS -4500
#define Z_MIN_POS 0
#define Z_MAX_POS 76500
#define R_REF_POS 0
#define R_MIN_POS -2500 // Anti-clockwise
#define R_MAX_POS 2500 // Clockwise

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
	SLEEP = 0x1,
	IDLE = 0x2,
	CALIBRATE = 0x4,
	READY = 0x8,
	RUN = 0x10
} motor_sys_state_t;

// Function prototypes

/**
 * Initialize the stepper motor control system.
 */
void initialize_motor();

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
 * Read the motor control state
 *
 * @return Motor control system state
 */
const motor_sys_state_t motor_system_state();

/**
 * Set the target position for the system on the X axis.
 * If the target position is outside of [X_MIN_POS, X_MAX_POS], the value will not be updated
 *
 * @param [in] pos Step position on X axis (full_steps).
 */
void motor_x_target_pos(int pos);

/**
 * Set the target position for the system on the Y axis.
 * If the target position is outside of [Y_MIN_POS, Y_MAX_POS], the value will not be updated
 *
 * @param [in] pos Step position on Y axis (full_steps).
 */
void motor_y_target_pos(int pos);

/**
 * Set the target position for the system on the Z axis.
 * If the target position is outside of [Z_MIN_POS, Z_MAX_POS], the value will not be updated
 *
 * @param [in] pos Step position on Z axis (full steps).
 */
void motor_z_target_pos(int pos);

/**
 * Set the target position for the system on the R axis.
 * If the target position is outside of [R_MIN_POS, R_MAX_POS], the value will not be updated
 *
 * @param [in] pos Step position on R axis (full steps).
 */
void motor_r_target_pos(int pos);

/**
 * Trigger motor to take next step for the X motor. The nature of the step depends on the motor state and configuration.
 * This must be called from the timer interrupt for the base timer for the motor
 */
void motor_x_execute_step();

/**
 * Trigger motor to take next step for the Y motor. The nature of the step depends on the motor state and configuration.
 * This must be called from the timer interrupt for the base timer for the motor
 */
void motor_y_execute_step();

/**
 * Trigger motor to take next step for the Z motor. The nature of the step depends on the motor state and configuration.
 * This must be called from the timer interrupt for the base timer for the motor
 */
void motor_z_execute_step();

/**
 * Trigger motor to take next step for the R motor. The nature of the step depends on the motor state and configuration.
 * This must be called from the timer interrupt for the base timer for the motor
 */
void motor_r_execute_step();

/**
 * Initiate motor run to target positions. The motor control state will be set to READY when all motors have reached their target position.
 */
void motor_run();

#endif /* MOTOR_H_ */
