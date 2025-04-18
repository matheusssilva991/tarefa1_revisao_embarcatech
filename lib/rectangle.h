#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
} rect_t;


void init_rectangle(rect_t *rect, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void set_rectangle_position(rect_t *rect, uint16_t x, uint16_t y);
void set_rectangle_size(rect_t *rect, uint16_t width, uint16_t height);


#endif // RECTANGLE_H
