#include "motor.h"

// State variables
motor_sys_state_t state = CALIBRATE;

// Step position for each axis
uint32_t x_pos = 0;
uint32_t y_pos = 0;
uint32_t z_pos = 0;
int r_pos = 0;

// Motor enable flags
bool x_enable = false;
bool y_enable = false;
bool z_enable = false;
bool r_enable = false;

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
	// Set motor to sleep mode
	motor_disable_all();
//	motor_enable_all();

	// Initialize motor directions
	gpio_pin_write(X_MOT_DIR_GPIO_Port, X_MOT_DIR_Pin, X_LEFT);
	gpio_pin_write(Y_MOT_DIR_GPIO_Port, Y_MOT_DIR_Pin, Y_BACKWARD);
	gpio_pin_write(Z_MOT_DIR_GPIO_Port, Z_MOT_DIR_Pin, Z_DOWN);
	gpio_pin_write(R_MOT_DIR_GPIO_Port, R_MOT_DIR_Pin, R_CLOCKWISE);

	// Find position zero for X axis
	x_enable = true;
	while (gpio_pin_read(X_LIM_GPIO_Port, X_LIM_Pin));
	x_enable = false;

	// Find position zero for Y axis
	y_enable = true;
	while (gpio_pin_read(Y_LIM_GPIO_Port, Y_LIM_Pin));
	y_enable = false;

	// Find position zero for Z axis
	z_enable = true;
	while (gpio_pin_read(Z_LIM_GPIO_Port, Z_LIM_Pin));
	z_enable = false;

	// Cycle motors along axis
	state = RUN;

	// Wait for Z axis to move up enough to remove suction cup from floor
	z_enable = true;
	while (z_pos < Z_MIN_POS);

	// Enable all motors
	x_enable = true;
	y_enable = true;
	r_enable = true;
}

void motor_x_execute_step()
{
	if (state == CALIBRATE)
	{
		if (x_enable && gpio_pin_read(X_LIM_GPIO_Port, X_LIM_Pin))
			gpio_pin_toggle(X_MOT_STEP_GPIO_Port, X_MOT_STEP_Pin);
	}
	else if (state == RUN)
	{
		if (x_enable)
		{
			// Update direction
			if (x_pos == 0)
				gpio_pin_write(X_MOT_DIR_GPIO_Port, X_MOT_DIR_Pin, X_RIGHT);
			else if (x_pos == X_MAX_POS)
				gpio_pin_write(X_MOT_DIR_GPIO_Port, X_MOT_DIR_Pin, X_LEFT);

			// Update position
			gpio_pin_state_t direction = gpio_pin_read(X_MOT_DIR_GPIO_Port, X_MOT_DIR_Pin);
			if (direction == X_LEFT)
				x_pos--;
			else
				x_pos++;

			// Execute step
			gpio_pin_toggle(X_MOT_STEP_GPIO_Port, X_MOT_STEP_Pin);
		}
	}
}

void motor_y_execute_step()
{
	if (state == CALIBRATE)
	{
		if (y_enable && gpio_pin_read(Y_LIM_GPIO_Port, Y_LIM_Pin))
			gpio_pin_toggle( Y_MOT_STEP_GPIO_Port, Y_MOT_STEP_Pin);
	}
	else if (state == RUN)
	{
		if (y_enable)
		{
			// Update direction
			if (y_pos == 0)
				gpio_pin_write(Y_MOT_DIR_GPIO_Port, Y_MOT_DIR_Pin, Y_FORWARD);
			else if (y_pos == Y_MAX_POS)
				gpio_pin_write(Y_MOT_DIR_GPIO_Port, Y_MOT_DIR_Pin, Y_BACKWARD);

			// Update position
			gpio_pin_state_t direction = gpio_pin_read(Y_MOT_DIR_GPIO_Port, Y_MOT_DIR_Pin);
			if (direction == Y_BACKWARD)
				y_pos--;
			else
				y_pos++;

			// Execute step
			gpio_pin_toggle(Y_MOT_STEP_GPIO_Port, Y_MOT_STEP_Pin);
		}
	}
}

void motor_z_execute_step()
{
	if (state == CALIBRATE)
	{
		if (z_enable && gpio_pin_read(Z_LIM_GPIO_Port, Z_LIM_Pin))
			gpio_pin_toggle(Z_MOT_STEP_GPIO_Port, Z_MOT_STEP_Pin);
	}
	else if (state == RUN)
	{
		if (z_enable)
		{
			// Update direction
			if (z_pos == 0 || z_pos == Z_MIN_POS)
				gpio_pin_write(Z_MOT_DIR_GPIO_Port, Z_MOT_DIR_Pin, Z_UP);
			else if (z_pos == Z_MAX_POS)
				gpio_pin_write(Z_MOT_DIR_GPIO_Port, Z_MOT_DIR_Pin, Z_DOWN);

			// Update position
			gpio_pin_state_t direction = gpio_pin_read(Z_MOT_DIR_GPIO_Port, Z_MOT_DIR_Pin);
			if (direction == Z_DOWN)
				z_pos--;
			else
				z_pos++;

			// Execute step
			gpio_pin_toggle(Z_MOT_STEP_GPIO_Port, Z_MOT_STEP_Pin);
		}
	}
}

void motor_r_execute_step()
{
	if (state == RUN)
	{
		if (r_enable)
		{
			// Update direction
			if (r_pos == R_MIN_POS)
				gpio_pin_write(R_MOT_DIR_GPIO_Port, R_MOT_DIR_Pin, R_CLOCKWISE);
			else if (r_pos == R_MAX_POS)
				gpio_pin_write(R_MOT_DIR_GPIO_Port, R_MOT_DIR_Pin, R_COUNTERCLOCKWISE);

			// Update position
			gpio_pin_state_t direction = gpio_pin_read(R_MOT_DIR_GPIO_Port, R_MOT_DIR_Pin);
			if (direction == R_COUNTERCLOCKWISE)
				r_pos--;
			else
				r_pos++;

			// Execute step
			gpio_pin_toggle(R_MOT_STEP_GPIO_Port, R_MOT_STEP_Pin);
		}
	}
}
