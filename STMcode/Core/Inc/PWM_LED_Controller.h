/*
 * PWM_LED_Controller.h
 *
 *  Created on: June 10, 2024
 *      Author: ana.zebic
 */

#ifndef INC_PWM_LED_CONTROLLER_H_
#define INC_PWM_LED_CONTROLLER_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include <math.h>

extern uint8_t SLAVE_ID;
extern uint8_t address_W;

void disable_All_Outputs();
void enable_All_Outputs();
void init_PWM_LED_Controller_Channels(I2C_HandleTypeDef *hi2c);
void disable_Sleep_Mode(I2C_HandleTypeDef *hi2c);
void enable_Sleep_Mode(I2C_HandleTypeDef *hi2c);
void set_Duty_Cycle(I2C_HandleTypeDef *hi2c, uint8_t dutyCycle, uint8_t led, uint8_t delay);
void change_Frequency(I2C_HandleTypeDef *hi2c, uint16_t frequency);
void start_AC(I2C_HandleTypeDef *hi2c);
void stop_AC(I2C_HandleTypeDef *hi2c);

#endif /* INC_PWM_LED_CONTROLLER_H_ */
