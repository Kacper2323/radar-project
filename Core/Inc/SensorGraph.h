/*
 * SensorGraph.h
 *
 *  Created on: Sep 28, 2023
 *      Author: kacpe
 */

#ifndef INC_SENSORGRAPH_H_
#define INC_SENSORGRAPH_H_

#include <stdint.h>

typedef struct {
	uint8_t graph_start_X;
	uint8_t graph_start_Y;
	uint8_t graph_width;
	uint8_t graph_height;
	uint8_t border_width;
	uint8_t column_nr;
	uint16_t prev_val;
}graph_handle;

void set_graph_window(graph_handle* graph);
void graph(graph_handle* graph, uint16_t value, int max_val);
void graph_backwards(graph_handle* graph, uint16_t value, int max_val);
void set_background(void);
void graph_next(graph_handle* graph, uint16_t value, int max_val, uint8_t col);

#endif /* INC_SENSORGRAPH_H_ */
