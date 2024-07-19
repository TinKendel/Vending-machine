/*
 * dht11.h
 *
 *  Created on: Jun 14, 2024
 *      Author: luka.leko
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "main.h"

void delay (uint16_t time);
void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void DHT11_Start (void);
void DHT11_SendToPython(void);
void read_Temperature_and_Humidity(void);

uint8_t DHT11_Check_Response (void);
uint8_t DHT11_Read (void);

extern float Temperature;
extern float Humidity;

#endif /* INC_DHT11_H_ */
