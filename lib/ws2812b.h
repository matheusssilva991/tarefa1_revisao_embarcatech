#ifndef WS2812B_H
#define WS2812B_H

#include <stdlib.h>
#include <stdio.h>
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "led_matrix_numbers.h"

// Tipos de dados.
struct pixel_t
{
    uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
};
typedef struct pixel_t pixel_t;
typedef pixel_t ws2812b_LED_t; // Mudança de nome de "struct pixel_t" para "ws2812bLED_t" por clareza.

extern ws2812b_LED_t led_matrix[LED_MATRIX_COUNT]; // Declaração do buffer de pixels que formam a matriz.
extern PIO led_matrix_pio;                     // Ponteiro para a máquina PIO.
extern uint sm;                        // Número da máquina state machine.

void ws2812b_init(uint pin);
void ws2812b_set_led(const uint index, const uint8_t r, const uint8_t g, const uint8_t b);
void ws2812b_clear();
void ws2812b_write();
void ws2812b_draw_number(uint8_t index);

#endif // WS2812B_H
