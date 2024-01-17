/*
 * SensorGraph.c
 *
 *  Created on: Sep 28, 2023
 *      Author: kacpe
 */
#include "SensorGraph.h"
#include "lcd.h"
#include <stdlib.h>

//static int window_start_X = 0;
//static int window_start_Y = 0;
//static int window_width = SCREEN_RES_X;
//static int window_height = SCREEN_RES_Y;
//static int border_width = 1;

static uint8_t lcd_buffer[256];


void set_background(void){
	for(int i = 0; i < 128; i += 16){
		lcd_box_fill( 0, i, 160, 1, 0x4242 );
	}
	for(int i=0; i<160; i += 20){
		lcd_box_fill( i, 0, 1, 128, 0x4242 );
	}
}

static void border_draw(uint8_t start_X, uint8_t start_Y, uint8_t width, uint8_t height, uint8_t border){
	lcd_box_fill( start_X, start_Y, width, border, 0x4242 ); //bottom border
	lcd_box_fill( start_X, start_Y, border, height, 0x4242 ); //left border
	lcd_box_fill( start_X, (height + start_Y), width, border, 0x4242 ); //top border
	lcd_box_fill( (width + start_X), start_Y, border, height, 0x4242 ); //right border
}

void set_graph_window(graph_handle* graph){
	uint8_t start_X = graph->graph_start_X;
	uint8_t start_Y = graph->graph_start_Y;
	uint8_t height = graph->graph_height;
	uint8_t width = graph->graph_width;
	uint8_t border_width = graph->border_width;

	lcd_box_fill(start_X+1, start_Y+1, width-1, height-1, WHITE);
	border_draw(start_X, start_Y, width, height, border_width);
}

//static uint8_t lcd_buffer[COLUMN_DATA] = {0};//{ [ 0 ... 255 ] = 0xff } without inverted colors;

static void set_buff_h(int pixel, int prev, uint8_t height){

	int i;
	for(i=0; i < height*2; i++){
		if( (i>=2*prev && i<=(2*pixel)+1) || (i>=2*pixel && i<=(2*prev)+1) ){
			lcd_buffer[i] = 0xff;
		}
		else
			lcd_buffer[i] = 0x00;
	}
}

static void green_line(uint8_t height){
	for(int i=0; i < height*2; i++){
		if(i%2==0){
			lcd_buffer[i] = 0xff - 0x07;
		}else{
			lcd_buffer[i] = 0xff - 0xe0;
		}
	}
}

static void draw_graph_col(int column_nr, uint8_t start_X, uint8_t start_Y, uint8_t border, uint8_t height){
	lcd_set_column( (column_nr + start_X + border), (start_Y + border), (height - border), lcd_buffer);
}

void graph(graph_handle* graph, uint16_t value, int max_val){

	uint8_t width = graph->graph_width;
	uint8_t height = graph->graph_height;
	uint8_t start_X = graph->graph_start_X;
	uint8_t start_Y = graph->graph_start_Y;
	uint8_t border = graph->border_width;

	if(graph->column_nr == 0){
		graph->prev_val = value;
	}

	value /= (uint16_t) (((float)max_val) / (height - 1.0));

	set_buff_h(value, graph->prev_val, height);
	graph->prev_val = value;

	draw_graph_col(graph->column_nr, start_X, start_Y, border, height);
	graph->column_nr += 1;

	green_line(height);
	if(graph->column_nr < (width - border))
		draw_graph_col(graph->column_nr, start_X, start_Y, border, height);
	else
		graph->column_nr = 0;
}

void graph_backwards(graph_handle* graph, uint16_t value, int max_val){

	uint8_t width = graph->graph_width;
	uint8_t height = graph->graph_height;
	uint8_t start_X = graph->graph_start_X;
	uint8_t start_Y = graph->graph_start_Y;
	uint8_t border = graph->border_width;

	if(graph->column_nr == width - border){
		graph->prev_val = value;
	}

	value /= (uint16_t) (((float)max_val) / (height - 1.0));

	set_buff_h(value, graph->prev_val, height);
	graph->prev_val = value;

	draw_graph_col(graph->column_nr, start_X, start_Y, border, height);
	graph->column_nr -= 1;

	green_line(height);
	if(graph->column_nr >= 0)
		draw_graph_col(graph->column_nr, start_X, start_Y, border, height);
	else
		graph->column_nr = width - border;
}


void graph_next(graph_handle* graph, uint16_t value, int max_val, uint8_t col){
	uint8_t width = graph->graph_width;
	uint8_t height = graph->graph_height;
	uint8_t start_X = graph->graph_start_X;
	uint8_t start_Y = graph->graph_start_Y;
	uint8_t border = graph->border_width;
	if(col == 0){
		graph->prev_val = value;
	}

	value /= (uint16_t) (((float)max_val) / (height - 1.0));

	set_buff_h(value, graph->prev_val, height);
	graph->prev_val = value;

	draw_graph_col(col, start_X, start_Y, border, height);

	green_line(height);
	if(col < (width - 2*border))
		draw_graph_col(col+1, start_X, start_Y, border, height);
}


