#include "motor.h"

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

void motor_execute_step(motor_t motor)
{

}
