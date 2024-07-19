/*
 * communicate_with_GUI.c
 *
 *  Created on: Jun 27, 2024
 *      Author: ana.zebic
 */
#include "communicate_with_GUI.h"
#include "main.h"
#include <string.h>

extern Automat My_Automat;
extern UART_HandleTypeDef huart3;

extern bool power_flag_up;
extern bool door_flag_up;
extern bool refilled_flag_up;
extern bool ideal_temperature_flag_up;
extern int8_t ideal_temperature;
extern bool received_data_read_flag;
extern int8_t received_data[2];
bool communication_established_flag_up = false;


void serialize_Automat(uint8_t* serialized_Automat, uint8_t data_Size)
{
	serialized_Automat[0] = My_Automat.products_Number;

	for(int i = 0; i < My_Automat.products_Number; ++i)
	{
		uint8_t product_Size = sizeof(My_Automat.products[i]);
		uint8_t product_offset =  1 + product_Size * i;
		serialized_Automat[product_offset] = My_Automat.products[i].slot_ID;
		for(int j = 0; j < 10; j++)
		{
			if(j < strlen(My_Automat.products[i].name))
				serialized_Automat[product_offset + 1 + j] = My_Automat.products[i].name[j];
			else
				serialized_Automat[product_offset + 1 + j] = '*';
		}
		serialized_Automat[product_offset + product_Size - 3] = My_Automat.products[i].price;
		serialized_Automat[product_offset + product_Size - 2] = My_Automat.products[i].quantity;
		serialized_Automat[product_offset + product_Size - 1] = My_Automat.products[i].capacity;
	}

	serialized_Automat[data_Size - 14] = My_Automat.capacity;
	serialized_Automat[data_Size - 13] = My_Automat.door_opened;
	serialized_Automat[data_Size - 12] = My_Automat.turned_on;
	serialized_Automat[data_Size - 11] = My_Automat.stand_by;
	serialized_Automat[data_Size - 10] = My_Automat.current_Temperature;
	serialized_Automat[data_Size - 9] = My_Automat.current_Humidity;
	serialized_Automat[data_Size - 8] = My_Automat.wanted_Temperature;
	serialized_Automat[data_Size - 7] = My_Automat.light_Intensity;
	serialized_Automat[data_Size - 6] = My_Automat.selected_Product;
	serialized_Automat[data_Size - 5] = My_Automat.current_Profit % (2*2*2*2*2*2*2*2);
	serialized_Automat[data_Size - 4] = My_Automat.current_Profit >> 8;
	serialized_Automat[data_Size - 3] = My_Automat.overheating;
	serialized_Automat[data_Size - 2] = My_Automat.freezing;
	serialized_Automat[data_Size - 1] = My_Automat.temperature_offset_tolerance;
}

void send_Serialized_Automat(uint8_t* serialized_Automat, uint8_t serialized_Automat_Size)
{
	HAL_UART_Transmit(&huart3, (uint8_t*)serialized_Automat, serialized_Automat_Size, 10*serialized_Automat_Size);
}

void receive_Data(int8_t* data_Buffer, uint8_t data_Size)
{
	if(data_Buffer[0] == 0)
	{
		power_flag_up = true;
	}
	else if(data_Buffer[0] == 1)
	{
		door_flag_up = true;
	}

	if(!My_Automat.turned_on)
	{
		received_data_read_flag = true;
		return;
	}

	if(data_Buffer[0] == 2)
	{
		refilled_flag_up = true;
	}
	else if(data_Buffer[0] == 3)
	{
		ideal_temperature_flag_up = true;
		ideal_temperature = data_Buffer[1];
	}
	else if(data_Buffer[0] == 4 && data_Buffer[1] == 4)
	{
		communication_established_flag_up = true;
        uint8_t serialized_Automat[99];
        serialize_Automat(serialized_Automat, sizeof(serialized_Automat));
        send_Serialized_Automat(serialized_Automat, sizeof(serialized_Automat));
	}
	received_data_read_flag = true;
}
