/*
 * PWM_LED_Controller.c
 *
 *  Created on: June 10, 2024
 *      Author: ana.zebic
 */
#include "PWM_LED_Controller.h"
#include "config.h"

uint8_t SLAVE_ID = 0x40;

uint8_t address_W = 0x40 << 1;   		//  write to SLAVE_ID

uint8_t ALL_LED_ON_L_Reg = 0xFA;
uint8_t ALL_LED_ON_H_Reg = 0xFB;

uint8_t ALL_LED_OFF_L_Reg = 0xFC;
uint8_t ALL_LED_OFF_H_Reg = 0xFD;


void init_PWM_LED_Controller_Channels(I2C_HandleTypeDef *hi2c)
{
	  HAL_Delay(100);

	  enable_All_Outputs();

	  uint8_t message_Buffer[2];

	  message_Buffer[0] = 0x00; 				// MODE 1
	  message_Buffer[1] = 0x01;
	  HAL_I2C_Master_Transmit(hi2c, address_W, (uint8_t*) &message_Buffer, 2, 1000);

	  message_Buffer[0] = 0x01;					// MODE 2
	  message_Buffer[1] = 0x04;
	  HAL_I2C_Master_Transmit(hi2c, address_W, (uint8_t*) &message_Buffer, 2, 1000);

	  HAL_Delay(100);

	for(int i = 0; i < NUMBER_OF_PRODUCTS; ++i)
	{
		set_Duty_Cycle(hi2c, 100, 2*i, 0);
		set_Duty_Cycle(hi2c, 0, 2*i + 1, 0);
	}
}

void disable_All_Outputs()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
}

void enable_All_Outputs()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
}

void enable_Sleep_Mode(I2C_HandleTypeDef *hi2c)
{
	uint8_t message_Buffer[2];

	message_Buffer[0] = 0x00; 				//  MODE 1
	message_Buffer[1] = 0x10;				//  SLEEP bit enabled

	HAL_I2C_Master_Transmit(hi2c, address_W, (uint8_t*) &message_Buffer, 2, 1000);
}

void disable_Sleep_Mode(I2C_HandleTypeDef *hi2c)
{
	uint8_t message_Buffer[2];

	message_Buffer[0] = 0x00; 				//  MODE 1

	HAL_I2C_Master_Transmit(hi2c, address_W, (uint8_t*) &message_Buffer, 1, 1000);
	HAL_I2C_Master_Receive(hi2c, address_W, (uint8_t*) &message_Buffer + 1, 1, 1000);

	if(message_Buffer[1] >= 0x80)			// RESTART bit set
	{
		message_Buffer[1] &= 0xEF;			//  SLEEP bit disabled
		HAL_I2C_Master_Transmit(hi2c, address_W, (uint8_t*) &message_Buffer, 2, 1000);

		HAL_Delay(1);

		message_Buffer[1] |= 0x80;			// set logic 1 to bit 7 of MODE 1 register
		HAL_I2C_Master_Transmit(hi2c, address_W, (uint8_t*) &message_Buffer, 2, 1000);
	}
}

void set_Duty_Cycle(I2C_HandleTypeDef *hi2c, uint8_t dutyCycle, uint8_t led, uint8_t delay)
{
	uint8_t led_ON_L_Reg = 0x06 + led * 4;
	uint8_t led_ON_H_Reg = 0x07 + led * 4;

	uint8_t led_OFF_L_Reg = 0x08 + led * 4;
	uint8_t led_OFF_H_Reg = 0x09 + led * 4;

	uint16_t led_ON_Value;
	uint8_t led_ON_L_Value;
	uint8_t led_ON_H_Value;

	uint16_t led_OFF_Value;
	uint8_t led_OFF_L_Value;
	uint8_t led_OFF_H_Value;

	if(delay + dutyCycle <= 100)
	{
		led_ON_Value = 4095 * (delay / 100.00);

		led_ON_L_Value = led_ON_Value % 256;
		led_ON_H_Value = led_ON_Value >> 8;

		led_OFF_Value = ((uint16_t)(led_ON_Value + (dutyCycle / 100.00) * 4095)) % 4096;

		led_OFF_L_Value = led_OFF_Value % 256;
		led_OFF_H_Value = led_OFF_Value >> 8;

	}
	else
	{
		led_ON_Value = 4095 * (delay / 100.00);

		led_ON_L_Value = led_ON_Value % 256;
		led_ON_H_Value = led_ON_Value >> 8;

		led_OFF_Value = 4095 - ((((delay + dutyCycle) % 100) / 100.00) * 4095.00);

		led_OFF_L_Value = led_OFF_Value % 256;
		led_OFF_H_Value = led_OFF_Value >> 8;
	}

	uint8_t message_Buffer_ON_L[2];
	uint8_t message_Buffer_ON_H[2];

	uint8_t message_Buffer_OFF_L[2];
	uint8_t message_Buffer_OFF_H[2];

	message_Buffer_ON_L[1] = led_ON_L_Value;
	message_Buffer_ON_L[0] = led_ON_L_Reg;

	message_Buffer_ON_H[1] = led_ON_H_Value;
	message_Buffer_ON_H[0] = led_ON_H_Reg;

	message_Buffer_OFF_L[1] = led_OFF_L_Value;
	message_Buffer_OFF_L[0] = led_OFF_L_Reg;

	message_Buffer_OFF_H[1] = led_OFF_H_Value;
	message_Buffer_OFF_H[0] = led_OFF_H_Reg;

	HAL_I2C_Master_Transmit(hi2c, address_W, (uint8_t*) &message_Buffer_ON_L, 2, 1000);
	HAL_I2C_Master_Transmit(hi2c, address_W, (uint8_t*) &message_Buffer_ON_H, 2, 1000);

	HAL_I2C_Master_Transmit(hi2c, address_W, (uint8_t*) &message_Buffer_OFF_L, 2, 1000);
	HAL_I2C_Master_Transmit(hi2c, address_W, (uint8_t*) &message_Buffer_OFF_H, 2, 1000);
}

void change_Frequency(I2C_HandleTypeDef *hi2c, uint16_t frequency)
{
	uint8_t prescaler_Value = round(25000000.00 / (frequency * 4096.00)) - 1;
	uint8_t prescaler_Reg = 0xFE;

	uint8_t message_Buffer[2];
	message_Buffer[0] = prescaler_Reg;
	message_Buffer[1] = prescaler_Value;

	enable_Sleep_Mode(hi2c);	// prescaler value can only be modified in SLEEP MODE

	HAL_I2C_Master_Transmit(hi2c, address_W, (uint8_t*) &message_Buffer, 2, 1000);

	disable_Sleep_Mode(hi2c);
}

void start_AC(I2C_HandleTypeDef *hi2c)
{
	set_Duty_Cycle(hi2c, 100, (2 * (NUMBER_OF_PRODUCTS)) + 1, 0);
}

void stop_AC(I2C_HandleTypeDef *hi2c)
{
	set_Duty_Cycle(hi2c, 0, (2 * (NUMBER_OF_PRODUCTS)) + 1, 0);
}
