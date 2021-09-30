#include "motor.h"

// State variables
motor_sys_state_t system_state = SLEEP;

// Calibration step count for each axis (1/32 microstepping)
int x_cal_count;
int y_cal_count;
int z_cal_count;
int r_cal_count;

// Step position for each axis (1/32 microstepping)
int x_pos;
int y_pos;
int z_pos;
int r_pos;

// Target step position for each axis (1/32 microstepping)
int x_target_pos;
int y_target_pos;
int z_target_pos;
int r_target_pos;

// Motor enable flags
bool x_enable = false;
bool y_enable = false;
bool z_enable = false;
bool r_enable = false;

// Function prototypes
static void update_run_state();
static void reset_step_outputs();

void initialize_motor()
{
	// Initialize state
	system_state = IDLE;
	motor_enable_all();
}

void motor_enable_all()
{
	gpio_pin_write(X_MOT_SLEEP_GPIO_Port, X_MOT_SLEEP_Pin, GPIO_PIN_HIGH);
	gpio_pin_write(Y_MOT_SLEEP_GPIO_Port, Y_MOT_SLEEP_Pin, GPIO_PIN_HIGH);
	gpio_pin_write(Z_MOT_SLEEP_GPIO_Port, Z_MOT_SLEEP_Pin, GPIO_PIN_HIGH);
	gpio_pin_write(R_MOT_SLEEP_GPIO_Port, R_MOT_SLEEP_Pin, GPIO_PIN_HIGH);
}

void motor_disable_all()
{
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
	motor_enable_all(); // Enable all motors

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
	x_enable = true;
	while (gpio_input_pin_read(X_LIM_GPIO_Port, X_LIM_Pin)); // Wait for limit switch activation
	x_enable = false;

	// Find position zero for Y axis
	y_enable = true;
	while (gpio_input_pin_read(Y_LIM_GPIO_Port, Y_LIM_Pin)); // Wait for limit switch activation
	y_enable = false;

	// Find position zero for Z axis
	z_enable = true;
	while (gpio_input_pin_read(Z_LIM_GPIO_Port, Z_LIM_Pin)); // Wait for limit switch activation
	z_enable = false;

	// Initialize position state
	x_pos = X_REF_POS;
	y_pos = Y_REF_POS;
	z_pos = Z_REF_POS;
	r_pos = R_REF_POS;

	// Enable R motor
	gpio_pin_write(R_MOT_SLEEP_GPIO_Port, R_MOT_SLEEP_Pin, GPIO_PIN_HIGH);

	// Initialize target position
	motor_x_target_pos(X_MIN_POS / 32);
	motor_y_target_pos(Y_MIN_POS / 32);
	motor_z_target_pos(Z_MIN_POS / 32);
	motor_r_target_pos(R_REF_POS / 32);

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

	// Convert full steps to 1/32 microsteps
	pos *= 32;

	//Update target position if valid
	if (pos >= X_MIN_POS && pos <= X_MAX_POS)
		x_target_pos = pos;
}

void motor_y_target_pos(int pos)
{
	// Check current state is valid for function call
	if ((system_state & READY) == 0)
		return;

	// Convert full steps to 1/32 microsteps
	pos *= 32;

	//Update target position if valid
	if (pos >= Y_MIN_POS && pos <= Y_MAX_POS)
		y_target_pos = pos;
}

void motor_z_target_pos(int pos)
{
	// Check current state is valid for function call
	if ((system_state & READY) == 0)
		return;

	// Convert full steps to 1/32 microsteps
	pos *= 32;

	//Update target position if valid
	if (pos >= Z_MIN_POS && pos <= Z_MAX_POS)
		z_target_pos = pos;
}

void motor_r_target_pos(int pos)
{
	// Check current state is valid for function call
	if ((system_state & READY) == 0)
		return;

	// Convert full steps to 1/32 microsteps
	pos *= 32;

	//Update target position if valid
	if (pos >= R_MIN_POS && pos <= R_MAX_POS)
		r_target_pos = pos;
}

void motor_x_execute_step()
{
	if (system_state == CALIBRATE)
	{
		bool full_step_pos = (x_cal_count % 32 == 0); // Check if in full step position
		if ((x_enable && gpio_input_pin_read(X_LIM_GPIO_Port, X_LIM_Pin)) || !full_step_pos)
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
			// Update direction
			if (x_pos < x_target_pos)
				gpio_pin_write(X_MOT_DIR_GPIO_Port, X_MOT_DIR_Pin, X_RIGHT);
			else if (x_pos > x_target_pos)
				gpio_pin_write(X_MOT_DIR_GPIO_Port, X_MOT_DIR_Pin, X_LEFT);

			gpio_pin_state_t step = gpio_output_pin_read(Y_MOT_STEP_GPIO_Port, Y_MOT_STEP_Pin);
			gpio_pin_state_t direction = gpio_output_pin_read(X_MOT_DIR_GPIO_Port, X_MOT_DIR_Pin);

			// Update position count if falling edge
			if (step == GPIO_PIN_HIGH)
			{
				if (direction == X_LEFT)
					x_pos--;
				else
					x_pos++;
			}

			// Execute step
			gpio_pin_toggle(X_MOT_STEP_GPIO_Port, X_MOT_STEP_Pin);

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
		bool full_step_pos = (y_cal_count % 32 == 0); // Check if in full step position
		if ((y_enable && gpio_input_pin_read(Y_LIM_GPIO_Port, Y_LIM_Pin)) || !full_step_pos)
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
			// Update direction
			if (y_pos < y_target_pos)
				gpio_pin_write(Y_MOT_DIR_GPIO_Port, Y_MOT_DIR_Pin, Y_FORWARD);
			else if (y_pos > y_target_pos)
				gpio_pin_write(Y_MOT_DIR_GPIO_Port, Y_MOT_DIR_Pin, Y_BACKWARD);

			gpio_pin_state_t step = gpio_output_pin_read(Y_MOT_STEP_GPIO_Port, Y_MOT_STEP_Pin);
			gpio_pin_state_t direction = gpio_output_pin_read(Y_MOT_DIR_GPIO_Port, Y_MOT_DIR_Pin);

			// Update position count if falling edge
			if (step == GPIO_PIN_HIGH)
			{
				if (direction == Y_BACKWARD)
					y_pos--;
				else
					y_pos++;
			}

			// Execute step
			gpio_pin_toggle(Y_MOT_STEP_GPIO_Port, Y_MOT_STEP_Pin);

			// Initiate RUN state review when target position is reached
			if (y_pos == y_target_pos)
				update_run_state();
		}
	}
}

void motor_z_execute_step()
{
	if (system_state == CALIBRATE)
	{
		bool full_step_pos = (z_cal_count % 32 == 0); // Check if in full step position
		if ((z_enable && gpio_input_pin_read(Z_LIM_GPIO_Port, Z_LIM_Pin)) || !full_step_pos)
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
			// Update direction
			if (z_pos < z_target_pos)
				gpio_pin_write(Z_MOT_DIR_GPIO_Port, Z_MOT_DIR_Pin, Z_UP);
			else if (z_pos > z_target_pos)
				gpio_pin_write(Z_MOT_DIR_GPIO_Port, Z_MOT_DIR_Pin, Z_DOWN);

			gpio_pin_state_t step = gpio_output_pin_read(Z_MOT_STEP_GPIO_Port, Z_MOT_STEP_Pin);
			gpio_pin_state_t direction = gpio_output_pin_read(Z_MOT_DIR_GPIO_Port, Z_MOT_DIR_Pin);

			// Update position count if falling edge
			if (step == GPIO_PIN_HIGH)
			{
				if (direction == Z_DOWN)
					z_pos--;
				else
					z_pos++;
			}

			// Execute step
			gpio_pin_toggle(Z_MOT_STEP_GPIO_Port, Z_MOT_STEP_Pin);

			// Initiate RUN state review when target position is reached
			if (z_pos == z_target_pos)
				update_run_state();
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
			// Update direction
			if (r_pos < r_target_pos)
				gpio_pin_write(R_MOT_DIR_GPIO_Port, R_MOT_DIR_Pin, R_CLOCKWISE);
			else if (r_pos > r_target_pos)
				gpio_pin_write(R_MOT_DIR_GPIO_Port, R_MOT_DIR_Pin, R_COUNTERCLOCKWISE);

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
