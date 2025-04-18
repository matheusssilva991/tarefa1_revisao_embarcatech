#include "rectangle.h"

void init_rectangle(rect_t *rect, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    if (rect == NULL) {
        return; // Invalid rectangle pointer
    }

    set_rectangle_position(rect, x, y);
    set_rectangle_size(rect, width, height);
}

void set_rectangle_position(rect_t *rect, uint16_t x, uint16_t y) {
    if (rect == NULL) {
        return; // Invalid rectangle pointer
    }
    if (x < 0 || y < 0) {
        return; // Invalid position
    }

    rect->x = x;
    rect->y = y;

}

void set_rectangle_size(rect_t *rect, uint16_t width, uint16_t height) {
    if (width < 0 || height < 0) {
        return; // Invalid size
    }

    rect->width = width;
    rect->height = height;
}
