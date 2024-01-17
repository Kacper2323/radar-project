/*
 * lcd.c
 *
 *  Created on: Sep 16, 2023
 *      Author: kacpe
 */

#include "lcd.h"
#include "spi.h"
#include "font.h"

#define ST7735S_SLPOUT			0x11
#define ST7735S_DISPOFF			0x28
#define ST7735S_DISPON			0x29
#define ST7735S_CASET			0x2a
#define ST7735S_RASET			0x2b
#define ST7735S_RAMWR			0x2c
#define ST7735S_MADCTL			0x36
#define ST7735S_COLMOD			0x3a
#define ST7735S_FRMCTR1			0xb1
#define ST7735S_FRMCTR2			0xb2
#define ST7735S_FRMCTR3			0xb3
#define ST7735S_INVCTR			0xb4
#define ST7735S_PWCTR1			0xc0
#define ST7735S_PWCTR2			0xc1
#define ST7735S_PWCTR3			0xc2
#define ST7735S_PWCTR4			0xc3
#define ST7735S_PWCTR5			0xc4
#define ST7735S_VMCTR1			0xc5
#define ST7735S_GAMCTRP1		0xe0
#define ST7735S_GAMCTRN1		0xe1
#define ST7735S_INVON			0x21
#define ST7735S_INVOFF			0x20

#define CMD(x) ((x) | 0x100)

static void lcd_data(uint8_t val){
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, 1);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 0);
	HAL_SPI_Transmit(&hspi1, &val, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 1);
}

static void lcd_data16(uint16_t val){
	lcd_data(val >> 8);
	lcd_data(val);
}

static void lcd_cmd(uint8_t val){
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, 0);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 0);
	HAL_SPI_Transmit(&hspi1, &val, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 1);
}

static void lcd_send(uint16_t val){
	if((val & 0x100) == 0){
		lcd_data((uint8_t) val);
	}
	else{
		lcd_cmd((uint8_t) val);
	}
}

static const uint16_t init_table[] = {
  CMD(ST7735S_FRMCTR1), 0x01, 0x2c, 0x2d,
  CMD(ST7735S_FRMCTR2), 0x01, 0x2c, 0x2d,
  CMD(ST7735S_FRMCTR3), 0x01, 0x2c, 0x2d, 0x01, 0x2c, 0x2d,
  CMD(ST7735S_INVCTR), 0x07,
  CMD(ST7735S_PWCTR1), 0xa2, 0x02, 0x84,
  CMD(ST7735S_PWCTR2), 0xc5,
  CMD(ST7735S_PWCTR3), 0x0a, 0x00,
  CMD(ST7735S_PWCTR4), 0x8a, 0x2a,
  CMD(ST7735S_PWCTR5), 0x8a, 0xee,
  CMD(ST7735S_VMCTR1), 0x0e,
  CMD(ST7735S_GAMCTRP1), 0x0f, 0x1a, 0x0f, 0x18, 0x2f, 0x28, 0x20, 0x22,
                         0x1f, 0x1b, 0x23, 0x37, 0x00, 0x07, 0x02, 0x10,
  CMD(ST7735S_GAMCTRN1), 0x0f, 0x1b, 0x0f, 0x17, 0x33, 0x2c, 0x29, 0x2e,
                         0x30, 0x30, 0x39, 0x3f, 0x00, 0x07, 0x03, 0x10,
  CMD(0xf0), 0x01,
  CMD(0xf6), 0x00,
  CMD(ST7735S_COLMOD), 0x05,
  CMD(ST7735S_MADCTL), 0x20,
  CMD(ST7735S_INVON),
};

void lcd_init(void){
	int i;
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, 0);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, 1);
	HAL_Delay(100);

	for(i=0; i < (sizeof(init_table) / sizeof(uint16_t)); i++){
		lcd_send(init_table[i]);
	}
	HAL_Delay(200);

	lcd_cmd(ST7735S_SLPOUT);
	HAL_Delay(120);
	lcd_cmd(ST7735S_DISPON);
}

#define X_OFFSET 1
#define Y_OFFSET 2

static void lcd_set_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height){
	lcd_cmd(ST7735S_CASET);
	lcd_data16(X_OFFSET + x);
	lcd_data16(X_OFFSET + x + width -1);

	lcd_cmd(ST7735S_RASET);
	lcd_data16(Y_OFFSET + y);
	lcd_data16(Y_OFFSET + y + height -1);
}

void lcd_box_fill(int x, int y, int width, int height, uint16_t color){
	int i;

	lcd_set_window(x, y, width, height);

	lcd_cmd(ST7735S_RAMWR);
	for(i=0; i < width * height; i++){
		lcd_data16(color);
	}
}

void lcd_set_column(int x, int y, int height, uint8_t *data){
	lcd_set_window(x, y, 1, height);

	lcd_cmd(ST7735S_RAMWR);
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, 1);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 0);
	HAL_SPI_Transmit(&hspi1, (uint8_t*)data, height*2, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 1);
}


static uint8_t font_buffer[FONT_WIDTH * FONT_HEIGHT * 2];

static void load_font_buffer(char symbol){
	symbol -= 32;	//font to ascii offset

	uint16_t temp;
    int counter = 0;

    for(int j=0; j<FONT_HEIGHT; j++){

        for(int i=0; i<FONT_WIDTH; i++){

            temp = ( fontData[(int)symbol][i] & (0x1 << (FONT_HEIGHT-j)) );
            font_buffer[counter] = (uint8_t) (!!temp * 0xff);
            font_buffer[counter+1] = (uint8_t) (!!temp * 0xff);
            counter += 2;
        }

    }
}

void lcd_print_char(int x, int y, char c){

	load_font_buffer(c);

	lcd_set_window(x, y, FONT_WIDTH, FONT_HEIGHT);

	lcd_cmd(ST7735S_RAMWR);
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, 1);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 0);
	HAL_SPI_Transmit(&hspi1, (uint8_t*)font_buffer, FONT_WIDTH*FONT_HEIGHT*2, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 1);
}
