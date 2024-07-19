#include <LCD1602.h>
#include "stm32f4xx_hal.h"
#include <string.h>
#include <Data.h>
#include <dht11.h>
#include<stdio.h>
#include<stdint.h>

/*********** Define the LCD PINS below ****************/
#define RS_Pin GPIO_PIN_7
#define RS_GPIO_Port GPIOE
#define RW_Pin GPIO_PIN_10
#define RW_GPIO_Port GPIOE
#define EN_Pin GPIO_PIN_11
#define EN_GPIO_Port GPIOE
#define D4_Pin GPIO_PIN_12
#define D4_GPIO_Port GPIOE
#define D5_Pin GPIO_PIN_13
#define D5_GPIO_Port GPIOE
#define D6_Pin GPIO_PIN_14
#define D6_GPIO_Port GPIOE
#define D7_Pin GPIO_PIN_15
#define D7_GPIO_Port GPIOE

/****************** define the timer handler below  **************/

/****************** define variables *********************/
char prvi_red[16];
char drugi_red[16];

extern TIM_HandleTypeDef htim2;

//VOID DELAY (UINT16_T TIME){
//	__HAL_TIM_SET_COUNTER(&HTIM2, 0);
//	WHILE ((__HAL_TIM_GET_COUNTER(&HTIM2)) < TIME);
//}

// Send data/command to LCD
void send_to_lcd(char data, int rs)
{
	HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, rs);  // rs = 1 for data, rs=0 for command

	// Write the data to the respective pin
	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, ((data >> 3) & 0x01));
	HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, ((data >> 2) & 0x01));
	HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, ((data >> 1) & 0x01));
	HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, ((data >> 0) & 0x01));

	// Toggle EN PIN to send the data
	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 1);
	delay(100);
	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 0);
	delay(100);
}

// Send command to LCD
void lcd_send_cmd(char cmd)
{
	char datatosend;

	// Send upper nibble first
	datatosend = ((cmd >> 4) & 0x0F);
	send_to_lcd(datatosend, 0);  // RS must be 0 while sending command

	// Send Lower Nibble
	datatosend = ((cmd) & 0x0F);
	send_to_lcd(datatosend, 0);
}

// Send data to LCD
void lcd_send_data(char data)
{
	char datatosend;

	// Send higher nibble
	datatosend = ((data >> 4) & 0x0f);
	send_to_lcd(datatosend, 1);  // rs =1 for sending data

	// Send Lower nibble
	datatosend = ((data) & 0x0f);
	send_to_lcd(datatosend, 1);
}

// Clear LCD display
void lcd_clear(void)
{
	lcd_send_cmd(0x01);
	HAL_Delay(2);
}

// Set cursor position
void lcd_put_cur(int row, int col)
{
	switch (row)
	{
	case 0:
		col |= 0x80;
		break;
	case 1:
		col |= 0xC0;
		break;
	}

	lcd_send_cmd(col);
}

// Initialize the LCD
void lcd_init(void)
{
	// 4 bit initialization
	HAL_Delay(50);  // wait for >40ms
	lcd_send_cmd(0x02);
	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 1);
	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 0);
	HAL_Delay(10);  // wait for >4.1ms
	lcd_send_cmd(0x02);
	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 1);
	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 0);
	HAL_Delay(10);  // wait for >100us
	lcd_send_cmd(0x02);
	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 1);
	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 0);
	HAL_Delay(10);  // 4bit mode

	// Display initialization
	lcd_send_cmd(0x28);
	HAL_Delay(10);
	lcd_send_cmd(0x0E);
	HAL_Delay(10);
}

// Send string to LCD
void lcd_send_string(char *str)
{
	while (*str) lcd_send_data(*str++);
}

void lcd_print_empty_slot(void)
{
	lcd_clear();
	lcd_put_cur(0, 3);
	lcd_send_string("EMPTY SLOT");
	lcd_put_cur(1, 3);
	lcd_send_string("TRY AGAIN!");
	HAL_Delay(1000);
}

void lcd_print_current_product(Product product)
{
	sprintf(prvi_red, "%u %s %u$", product.slot_ID + 1, product.name, product.price);
	sprintf(drugi_red, "QUANTITY: %u#", product.quantity);
	lcd_clear();
	lcd_put_cur(0, 0);
	lcd_send_string(prvi_red);
	lcd_put_cur(1, 0);
	lcd_send_string(drugi_red);
}

void lcd_print_maintance(void)
{
	lcd_clear();
	lcd_put_cur(0, 2);
	lcd_send_string("MAINTENANCE");
	lcd_put_cur(1, 7);
	lcd_send_string("MODE");
}

void lcd_print_buying_product(void)
{
	lcd_clear();
	lcd_put_cur(0, 0);
	lcd_send_string("Buying product...");
	lcd_put_cur(1, 0);
	lcd_send_string("PLEASE WAIT...");
	HAL_Delay(2500);
}

void lcd_print_product_selected(Product product)
{
	sprintf(prvi_red, "BUYING PRODUCT");
	sprintf(drugi_red, "  %s", product.name);
	lcd_clear();
	lcd_put_cur(0, 0);
	lcd_send_string(prvi_red);
	lcd_put_cur(1, 0);
	lcd_send_string(drugi_red);
}
