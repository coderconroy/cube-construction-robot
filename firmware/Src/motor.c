#include "motor.h"
#include "math.h"
#include <stdlib.h>

// Acceleration parameter defines
#define X_ACC_MIN_PERIOD 49
#define Y_ACC_MIN_PERIOD 99
#define Z_ACC_MIN_PERIOD 29

#define X_ACC_PERIOD_FULL_STEPS 50
#define Y_ACC_PERIOD_FULL_STEPS 50
#define Z_ACC_PERIOD_FULL_STEPS 50

#define X_ACC_UPDATE_PERIOD 32
#define Y_ACC_UPDATE_PERIOD 16
#define Z_ACC_UPDATE_PERIOD 32

#define X_ACC_PERIODS_SIZE (X_ACC_PERIOD_FULL_STEPS * X_STEP_MODE / X_ACC_UPDATE_PERIOD + 1)
#define Y_ACC_PERIODS_SIZE (Y_ACC_PERIOD_FULL_STEPS * Y_STEP_MODE / Y_ACC_UPDATE_PERIOD + 1)
#define Z_ACC_PERIODS_SIZE (Z_ACC_PERIOD_FULL_STEPS * Z_STEP_MODE / Z_ACC_UPDATE_PERIOD + 1)

// State variables
motor_sys_state_t system_state;

// Calibration step count for each axis (step size defined by the axis step mode)
int x_cal_count;
int y_cal_count;
int z_cal_count;
int r_cal_count;

// Step position for each axis (step size defined by the axis step mode)
int x_pos;
int y_pos;
int z_pos;
int r_pos;

// Starting step position for each axis for current move (step size defined by the axis step mode)
int x_start_pos;
int y_start_pos;
int z_start_pos;
int r_start_pos;

// Target step position for each axis (step size defined by the axis step mode)
int x_target_pos;
int y_target_pos;
int z_target_pos;
int r_target_pos;

// Motor directions
gpio_pin_state_t x_direction;
gpio_pin_state_t y_direction;
gpio_pin_state_t z_direction;
gpio_pin_state_t r_direction;

// Acceleration curve timer period lookup tables
uint16_t y_acc_periods[Y_ACC_PERIODS_SIZE];
uint16_t x_acc_periods[X_ACC_PERIODS_SIZE];
uint16_t z_acc_periods[Z_ACC_PERIODS_SIZE];

// Motor calibration enable flags
bool x_cal_enable = false;
bool y_cal_enable = false;
bool z_cal_enable = false;
bool r_cal_enable = false;

// Function prototypes
static void update_run_state();
static void reset_step_outputs();

void initialize_motor()
{
	// Initialize motor system state
	system_state = IDLE;

	// Initialize step modes
	uint8_t x_step_mode = (uint8_t) log2(X_STEP_MODE);
	gpio_pin_write(X_MOT_M0_GPIO_Port, X_MOT_M0_Pin, x_step_mode & 0x1);
	gpio_pin_write(X_MOT_M1_GPIO_Port, X_MOT_M1_Pin, (x_step_mode & 0x2) >> 1);
	gpio_pin_write(X_MOT_M2_GPIO_Port, X_MOT_M2_Pin, (x_step_mode & 0x4) >> 2);

	uint8_t y_step_mode = (uint8_t) log2(Y_STEP_MODE);
	gpio_pin_write(Y_MOT_M0_GPIO_Port, Y_MOT_M0_Pin, y_step_mode & 0x1); // Non-functional
	gpio_pin_write(Y_MOT_M1_GPIO_Port, Y_MOT_M1_Pin,  (y_step_mode & 0x2) >> 1); // Non-functional
	gpio_pin_write(Y_MOT_M2_GPIO_Port, Y_MOT_M2_Pin, (y_step_mode & 0x4) >> 2);

	uint8_t z_step_mode = (uint8_t) log2(Z_STEP_MODE);
	gpio_pin_write(Z_MOT_M0_GPIO_Port, Z_MOT_M0_Pin, z_step_mode & 0x1);
	gpio_pin_write(Z_MOT_M1_GPIO_Port, Z_MOT_M1_Pin, (z_step_mode & 0x2) >> 1);
	gpio_pin_write(Z_MOT_M2_GPIO_Port, Z_MOT_M2_Pin, (z_step_mode & 0x4) >> 2);

	uint8_t r_step_mode = (uint8_t) log2(R_STEP_MODE);
	gpio_pin_write(R_MOT_M0_GPIO_Port, R_MOT_M0_Pin, r_step_mode & 0x1);
	gpio_pin_write(R_MOT_M1_GPIO_Port, R_MOT_M1_Pin, (r_step_mode & 0x2) >> 1);
	gpio_pin_write(R_MOT_M2_GPIO_Port, R_MOT_M2_Pin, (r_step_mode & 0x4) >> 2);

	// Remove motors from sleep mode
	motor_wake_all();

	// Remove motors from reset state
	gpio_pin_set(X_MOT_RESET_GPIO_Port, X_MOT_RESET_Pin);
	gpio_pin_set(Y_MOT_RESET_GPIO_Port, Y_MOT_RESET_Pin);
	gpio_pin_set(Z_MOT_RESET_GPIO_Port, Z_MOT_RESET_Pin);
	gpio_pin_set(R_MOT_RESET_GPIO_Port, R_MOT_RESET_Pin);

	// Initialize motor acceleration curves
	float min_velocity_frac = 0.1;

	for (int i = 0; i < Y_ACC_PERIODS_SIZE; i++)
	{
		float velocity = ((float) i / (Y_ACC_PERIOD_FULL_STEPS * Y_STEP_MODE / Y_ACC_UPDATE_PERIOD) + min_velocity_frac) / Y_ACC_MIN_PERIOD / (1 + min_velocity_frac);
		y_acc_periods[i] = 1 / velocity;
	}

	for (int i = 0; i < X_ACC_PERIODS_SIZE; i++)
	{
		float velocity = ((float) i / (X_ACC_PERIOD_FULL_STEPS * X_STEP_MODE / X_ACC_UPDATE_PERIOD) + min_velocity_frac) / X_ACC_MIN_PERIOD / (1 + min_velocity_frac);
		x_acc_periods[i] = 1 / velocity;
	}

	for (int i = 0; i < Z_ACC_PERIODS_SIZE; i++)
	{
		float velocity = ((float) i / (Z_ACC_PERIOD_FULL_STEPS * Z_STEP_MODE / Z_ACC_UPDATE_PERIOD) + min_velocity_frac) / Z_ACC_MIN_PERIOD / (1 + min_velocity_frac);
		z_acc_periods[i] = 1 / velocity;
	}
}

void motor_wake_all()
{
	gpio_pin_write(X_MOT_SLEEP_GPIO_Port, X_MOT_SLEEP_Pin, GPIO_PIN_HIGH);
	gpio_pin_write(Y_MOT_SLEEP_GPIO_Port, Y_MOT_SLEEP_Pin, GPIO_PIN_HIGH);
	gpio_pin_write(Z_MOT_SLEEP_GPIO_Port, Z_MOT_SLEEP_Pin, GPIO_PIN_HIGH);
	gpio_pin_write(R_MOT_SLEEP_GPIO_Port, R_MOT_SLEEP_Pin, GPIO_PIN_HIGH);
}

void motor_sleep_all()
{
	system_state = SLEEP;
	gpio_pin_write(X_MOT_SLEEP_GPIO_Port, X_MOT_SLEEP_Pin, GPIO_PIN_LOW);
	gpio_pin_write(Y_MOT_SLEEP_GPIO_Port, Y_MOT_SLEEP_Pin, GPIO_PIN_LOW);
	gpio_pin_write(Z_MOT_SLEEP_GPIO_Port, Z_MOT_SLEEP_Pin, GPIO_PIN_LOW);
	gpio_pin_write(R_MOT_SLEEP_GPIO_Port, R_MOT_SLEEP_Pin, GPIO_PIN_LOW);
}

void motor_calibrate()
{
	// Check current state is valid for function call
	if ((system_state & (IDLE | SLEEP)) == 0)
		return;

	// Initialize state
	system_state = CALIBRATE;
	motor_wake_all(); // Enable all motors

	// Initialize calibration velocities by setting the timer auto-reload register values
	TIM6->ARR = (uint16_t) 199;
	TIM7->ARR = (uint16_t) 399;
	TIM21->ARR = (uint16_t) 99;
	TIM22->ARR = (uint16_t) 199;

	// Disable R motor to release rotational tension on the vacuum tube
	gpio_pin_write(R_MOT_SLEEP_GPIO_Port, R_MOT_SLEEP_Pin, GPIO_PIN_LOW);

	// Initialize motor directions
	gpio_pin_write(X_MOT_DIR_GPIO_Port, X_MOT_DIR_Pin, X_LEFT);
	gpio_pin_write(Y_MOT_DIR_GPIO_Port, Y_MOT_DIR_Pin, Y_BACKWARD);
	gpio_pin_write(Z_MOT_DIR_GPIO_Port, Z_MOT_DIR_Pin, Z_DOWN);
	gpio_pin_write(R_MOT_DIR_GPIO_Port, R_MOT_DIR_Pin, R_CLOCKWISE);

	// Initialize motor step outputs
	reset_step_outputs();

	// Initialize calibration step counts (used to ensure motor stops in full step position)
	x_cal_count = 0;
	y_cal_count = 0;
	z_cal_count = 0;
	r_cal_count = 0;

	// Find position zero for X axis
	x_cal_enable = true;
	while (gpio_input_pin_read(X_LIM_GPIO_Port, X_LIM_Pin)); // Wait for limit switch activation
	x_cal_enable = false;

	// Find position zero for Y axis
	y_cal_enable = true;
	while (gpio_input_pin_read(Y_LIM_GPIO_Port, Y_LIM_Pin)); // Wait for limit switch activation
	y_cal_enable = false;

	// Find position zero for Z axis
	z_cal_enable = true;
	while (gpio_input_pin_read(Z_LIM_GPIO_Port, Z_LIM_Pin)); // Wait for limit switch activation
	z_cal_enable = false;

	// Initialize position state
	x_pos = X_REF_POS;
	y_pos = Y_REF_POS;
	z_pos = Z_REF_POS;
	r_pos = R_REF_POS;

	// Enable R motor
	gpio_pin_write(R_MOT_SLEEP_GPIO_Port, R_MOT_SLEEP_Pin, GPIO_PIN_HIGH);

	system_state = READY;

	// Initialize initial target position
	motor_x_target_pos(X_MIN_POS / X_STEP_MODE);
	motor_y_target_pos(Y_MIN_POS / Y_STEP_MODE);
	motor_z_target_pos(Z_MIN_POS / Z_STEP_MODE);
	motor_r_target_pos(R_REF_POS / R_STEP_MODE);

	// Run motors to initial target position
	motor_run();

	// Wait for motor runs to complete (system state will change to READY)
	while (system_state != READY);
}

const motor_sys_state_t motor_system_state()
{
	return system_state;
}

void motor_x_target_pos(int pos)
{
	// Check current state is valid for function call
	if ((system_state & READY) == 0)
		return;

	// Convert full steps to step size used on axis
	pos *= X_STEP_MODE;

	//Update target position if valid
	if (pos >= X_MIN_POS && pos <= X_MAX_POS)
		x_target_pos = pos;
}

void motor_y_target_pos(int pos)
{
	// Check current state is valid for function call
	if ((system_state & READY) == 0)
		return;

	// Convert full steps to step size used on axis
	pos *= Y_STEP_MODE;

	//Update target position if valid
	if (pos >= Y_MIN_POS && pos <= Y_MAX_POS)
		y_target_pos = pos;
}

void motor_z_target_pos(int pos)
{
	// Check current state is valid for function call
	if ((system_state & READY) == 0)
		return;

	// Convert full steps to step size used on axis
	pos *= Z_STEP_MODE;

	//Update target position if valid
	if (pos >= Z_MIN_POS && pos <= Z_MAX_POS)
		z_target_pos = pos;
}

void motor_r_target_pos(int pos)
{
	// Check current state is valid for function call
	if ((system_state & READY) == 0)
		return;

	// Convert full steps to step size used on axis
	pos *= R_STEP_MODE;

	//Update target position if valid
	if (pos >= R_MIN_POS && pos <= R_MAX_POS)
		r_target_pos = pos;
}

void motor_x_execute_step()
{
	if (system_state == CALIBRATE)
	{
		bool full_step_pos = (x_cal_count % X_STEP_MODE == 0); // Check if in full step position
		if ((x_cal_enable && gpio_input_pin_read(X_LIM_GPIO_Port, X_LIM_Pin)) || !full_step_pos)
		{
			// Update calibration step count on falling edge
			gpio_pin_state_t step_state = gpio_output_pin_read(X_MOT_STEP_GPIO_Port, X_MOT_STEP_Pin);
			if (step_state == GPIO_PIN_HIGH)
				x_cal_count++;

			// Execute step
			gpio_pin_toggle(X_MOT_STEP_GPIO_Port, X_MOT_STEP_Pin);
		}
	}
	else if (system_state == RUN)
	{
		// Update position
		if (x_pos != x_target_pos)
		{
			gpio_pin_state_t step = gpio_output_pin_read(X_MOT_STEP_GPIO_Port, X_MOT_STEP_Pin);

			// Update position count if falling edge
			if (step == GPIO_PIN_HIGH)
			{
				if (x_direction == X_LEFT)
					x_pos--;
				else
					x_pos++;
			}

			// Execute step
			gpio_pin_toggle(X_MOT_STEP_GPIO_Port, X_MOT_STEP_Pin);

			// Update speed
			if (x_pos % 32 == 0)
			{
				int x_steps_remain = abs(x_target_pos - x_pos);
				int x_steps_complete = abs(x_start_pos - x_pos);

				if (x_steps_complete < x_steps_remain)
				{
					if (x_steps_complete <= X_ACC_PERIOD_FULL_STEPS * X_STEP_MODE)
						TIM6->ARR = x_acc_periods[x_steps_complete / X_ACC_UPDATE_PERIOD];
				}
				// Deceleration phase
				else
				{
					if (x_steps_remain <= X_ACC_PERIOD_FULL_STEPS * X_STEP_MODE)
						TIM6->ARR = x_acc_periods[x_steps_remain / X_ACC_UPDATE_PERIOD];
				}
			}

			// Initiate RUN state review when target position is reached
			if (x_pos == x_target_pos)
				update_run_state();
		}
	}
}

void motor_y_execute_step()
{
	if (system_state == CALIBRATE)
	{
		bool full_step_pos = (y_cal_count % Y_STEP_MODE == 0); // Check if in full step position
		if ((y_cal_enable && gpio_input_pin_read(Y_LIM_GPIO_Port, Y_LIM_Pin)) || !full_step_pos)
		{
			// Update calibration step count on falling edge
			gpio_pin_state_t step_state = gpio_output_pin_read(Y_MOT_STEP_GPIO_Port, Y_MOT_STEP_Pin);
			if (step_state == GPIO_PIN_HIGH)
				y_cal_count++;

			// Execute step
			gpio_pin_toggle(Y_MOT_STEP_GPIO_Port, Y_MOT_STEP_Pin);
		}
	}
	else if (system_state == RUN)
	{
		// Update position
		if (y_pos != y_target_pos)
		{
			gpio_pin_state_t step = gpio_output_pin_read(Y_MOT_STEP_GPIO_Port, Y_MOT_STEP_Pin);

			// Update position count if falling edge
			if (step == GPIO_PIN_HIGH)
			{
				if (y_direction == Y_BACKWARD)
					y_pos--;
				else
					y_pos++;
			}

			// Execute step
			gpio_pin_toggle(Y_MOT_STEP_GPIO_Port, Y_MOT_STEP_Pin);

			// Update speed
			if (y_pos % 16 == 0)
			{
				int y_steps_remain = abs(y_target_pos - y_pos);
				int y_steps_complete = abs(y_start_pos - y_pos);

				if (y_steps_complete < y_steps_remain)
				{
					if (y_steps_complete <= Y_ACC_PERIOD_FULL_STEPS * Y_STEP_MODE)
						TIM7->ARR = y_acc_periods[y_steps_complete / Y_ACC_UPDATE_PERIOD];
				}
				// Deceleration phase
				else
				{
					if (y_steps_remain <= Y_ACC_PERIOD_FULL_STEPS * Y_STEP_MODE)
						TIM7->ARR = y_acc_periods[y_steps_remain / Y_ACC_UPDATE_PERIOD];
				}

				// Initiate RUN state review when target position is reached
				if (y_pos == y_target_pos)
					update_run_state();
			}
		}
	}
}

void motor_z_execute_step()
{
	if (system_state == CALIBRATE)
	{
		bool full_step_pos = (z_cal_count % Z_STEP_MODE == 0); // Check if in full step position
		if ((z_cal_enable && gpio_input_pin_read(Z_LIM_GPIO_Port, Z_LIM_Pin)) || !full_step_pos)
		{
			// Update calibration step count on falling edge
			gpio_pin_state_t step_state = gpio_output_pin_read(Z_MOT_STEP_GPIO_Port, Z_MOT_STEP_Pin);
			if (step_state == GPIO_PIN_HIGH)
				z_cal_count++;

			// Execute step
			gpio_pin_toggle(Z_MOT_STEP_GPIO_Port, Z_MOT_STEP_Pin);
		}
	}
	else if (system_state == RUN)
	{
		// Update position
		if (z_pos != z_target_pos)
		{
			gpio_pin_state_t step = gpio_output_pin_read(Z_MOT_STEP_GPIO_Port, Z_MOT_STEP_Pin);

			// Update position count if falling edge
			if (step == GPIO_PIN_HIGH)
			{
				if (z_direction == Z_DOWN)
					z_pos--;
				else
					z_pos++;
			}

			// Execute step
			gpio_pin_toggle(Z_MOT_STEP_GPIO_Port, Z_MOT_STEP_Pin);

			// Update speed
			if (z_pos % 32 == 0)
			{
				int z_steps_remain = abs(z_target_pos - z_pos);
				int z_steps_complete = abs(z_start_pos - z_pos);

				if (z_steps_complete < z_steps_remain)
				{
					if (z_steps_complete <= Z_ACC_PERIOD_FULL_STEPS * Z_STEP_MODE)
						TIM21->ARR = z_acc_periods[z_steps_complete / Z_ACC_UPDATE_PERIOD];
				}
				// Deceleration phase
				else
				{
					if (z_steps_remain <= Z_ACC_PERIOD_FULL_STEPS * Z_STEP_MODE)
						TIM21->ARR = z_acc_periods[z_steps_remain / Z_ACC_UPDATE_PERIOD];
				}

				// Initiate RUN state review when target position is reached
				if (z_pos == z_target_pos)
					update_run_state();
			}
		}
	}
}

void motor_r_execute_step()
{
	if (system_state == RUN)
	{
		// Update position
		if (r_pos != r_target_pos)
		{
			gpio_pin_state_t step = gpio_output_pin_read(R_MOT_STEP_GPIO_Port, R_MOT_STEP_Pin);
			gpio_pin_state_t direction = gpio_output_pin_read(R_MOT_DIR_GPIO_Port, R_MOT_DIR_Pin);

			// Update position count if falling edge
			if (step == GPIO_PIN_HIGH)
			{
				if (direction == R_COUNTERCLOCKWISE)
					r_pos--;
				else
					r_pos++;
			}

			// Execute step
			gpio_pin_toggle(R_MOT_STEP_GPIO_Port, R_MOT_STEP_Pin);

			// Initiate RUN state review when target position is reached
			if (r_pos == r_target_pos)
				update_run_state();
		}
	}
}

void motor_run()
{
	// Check current state is valid for function call
	if ((system_state & (READY | CALIBRATE)) == 0)
		return;

	// Check if any motors need to move to their targets
	if ((x_pos == x_target_pos) && (y_pos == y_target_pos) && (z_pos == z_target_pos) && (r_pos == r_target_pos))
		return;

	// Initialize motor directions
	if (x_pos < x_target_pos)
		gpio_pin_write(X_MOT_DIR_GPIO_Port, X_MOT_DIR_Pin, X_RIGHT);
	else if (x_pos > x_target_pos)
		gpio_pin_write(X_MOT_DIR_GPIO_Port, X_MOT_DIR_Pin, X_LEFT);

	if (y_pos < y_target_pos)
		gpio_pin_write(Y_MOT_DIR_GPIO_Port, Y_MOT_DIR_Pin, Y_FORWARD);
	else if (y_pos > y_target_pos)
		gpio_pin_write(Y_MOT_DIR_GPIO_Port, Y_MOT_DIR_Pin, Y_BACKWARD);

	if (z_pos < z_target_pos)
		gpio_pin_write(Z_MOT_DIR_GPIO_Port, Z_MOT_DIR_Pin, Z_UP);
	else if (z_pos > z_target_pos)
		gpio_pin_write(Z_MOT_DIR_GPIO_Port, Z_MOT_DIR_Pin, Z_DOWN);

	if (r_pos < r_target_pos)
		gpio_pin_write(R_MOT_DIR_GPIO_Port, R_MOT_DIR_Pin, R_CLOCKWISE);
	else if (r_pos > r_target_pos)
		gpio_pin_write(R_MOT_DIR_GPIO_Port, R_MOT_DIR_Pin, R_COUNTERCLOCKWISE);

	x_direction = gpio_output_pin_read(X_MOT_DIR_GPIO_Port, X_MOT_DIR_Pin);
	y_direction = gpio_output_pin_read(Y_MOT_DIR_GPIO_Port, Y_MOT_DIR_Pin);
	z_direction = gpio_output_pin_read(Z_MOT_DIR_GPIO_Port, Z_MOT_DIR_Pin);
	r_direction = gpio_output_pin_read(R_MOT_DIR_GPIO_Port, R_MOT_DIR_Pin);

	// Initialize motor starting positions
	x_start_pos = x_pos;
	y_start_pos = y_pos;
	z_start_pos = z_pos;
	r_start_pos = r_pos;

	// Initialize state
	reset_step_outputs();
	system_state = RUN;
}

/**
 * Check if all motors have reached their target position and set system state to READY if true.
 * Only intended for use in the RUN motor control state.
 */
static void update_run_state()
{
	// Check current state is valid for function call
	if ((system_state & RUN) == 0)
		return;

	// Check if all motors have reached their target position
	if ((x_pos == x_target_pos) && (y_pos == y_target_pos) && (z_pos == z_target_pos) && (r_pos == r_target_pos))
	{
		reset_step_outputs();
		system_state = READY;
	}
}

/**
 * Set the step outputs of all the motors to logic level low
 */
static void reset_step_outputs()
{
	gpio_pin_reset(X_MOT_STEP_GPIO_Port, X_MOT_STEP_Pin);
	gpio_pin_reset(Y_MOT_STEP_GPIO_Port, Y_MOT_STEP_Pin);
	gpio_pin_reset(Z_MOT_STEP_GPIO_Port, Z_MOT_STEP_Pin);
	gpio_pin_reset(R_MOT_STEP_GPIO_Port, R_MOT_STEP_Pin);
}
