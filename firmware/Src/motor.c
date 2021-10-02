#include "motor.h"
#include "math.h"

// State variables
motor_sys_state_t system_state = SLEEP;

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

// Target step position for each axis (step size defined by the axis step mode)
int x_target_pos;
int y_target_pos;
int z_target_pos;
int r_target_pos;

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
