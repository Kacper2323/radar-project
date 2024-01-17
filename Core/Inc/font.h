/*
 * font11x16.h
 *
 * Created: 4/8/2015 11:31:21 AM
 *  Author: Baron Williams
 */

#ifndef _FONT11X16_h
#define _FONT11X16_h

#include <stdint.h>

#define FONT_CHAR_FIRST 32
#define FONT_CHAR_LAST	126
#define FONT_CHARS 95
#define FONT_WIDTH 11
#define FONT_HEIGHT 16
#define FONT_SPACING 1

extern const uint16_t fontData[FONT_CHARS][FONT_WIDTH];

#endif
