#ifndef INC_LCD1602_H_
#define INC_LCD1602_H_

#include "Data.h" // Include the data.h header to use Automat and Product structures

void lcd_init (void);   // initialize lcd

void lcd_send_cmd (char cmd);  // send command to the lcd

void lcd_send_data (char data);  // send data to the lcd

void lcd_send_string (char *str);  // send string to the lcd

void lcd_put_cur(int row, int col);  // put cursor at the entered position row (0 or 1), col (0-15);

void lcd_clear (void);  // clear the lcd display

void lcd_print_product_selected (Product product);  // print selected product details

void lcd_print_maintance (void);  // print maintenance details

void lcd_print_current_product (Product product);  // print current product details

void lcd_print_empty_slot (void);  // print empty slot message

void lcd_print_buying_product (void);  // print buying product message

#endif /* INC_LCD1602_H_ */
