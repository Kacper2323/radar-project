/*
 * lcd.h
 *
 *  Created on: Sep 16, 2023
 *      Author: kacpe
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include <stdint.h>

#define BLACK     0xffff
#define WHITE     0x0000
#define SCREEN_RES_X 128;
#define SCREEN_RES_Y 160;

#define COLUMN_DATA 128
#define COLUMN_PIXELS 64

void lcd_init(void);
void lcd_box_fill(int x, int y, int width, int height, uint16_t color);
void lcd_set_column(int x, int y, int height, uint8_t *data);
void lcd_print_char(int x, int y, char c);

#endif /* INC_LCD_H_ */
