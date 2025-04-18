#include "ws2812b.h"
#include "led_matrix.pio.h"

ws2812b_LED_t led_matrix[LED_MATRIX_COUNT];
PIO led_matrix_pio;
uint sm;

// Inicializa a máquina PIO para controle da matriz de LEDs.
void ws2812b_init(uint pin)
{

    // Cria programa PIO.
    uint offset = pio_add_program(pio0, &led_matrix_program);
    led_matrix_pio = pio0;

    // Toma posse de uma máquina PIO.
    sm = pio_claim_unused_sm(led_matrix_pio, false);
    if (sm < 0)
    {
        led_matrix_pio = pio1;
        sm = pio_claim_unused_sm(led_matrix_pio, true); // Se nenhuma máquina estiver livre, panic!
    }

    // Inicia programa na máquina PIO obtida.
    led_matrix_program_init(led_matrix_pio, sm, offset, pin, 800000.f);

    // Limpa buffer de pixels.
    for (uint i = 0; i < LED_MATRIX_COUNT; ++i)
    {
        led_matrix[i].R = 0;
        led_matrix[i].G = 0;
        led_matrix[i].B = 0;
    }
}

// Atribui uma cor RGB a um LED.
void ws2812b_set_led(const uint index, const uint8_t r, const uint8_t g, const uint8_t b)
{
    led_matrix[index].R = r;
    led_matrix[index].G = g;
    led_matrix[index].B = b;
}

// Limpa o buffer de pixels.
void ws2812b_clear()
{
    for (uint i = 0; i < LED_MATRIX_COUNT; ++i)
        ws2812b_set_led(i, 0, 0, 0);
}

// Escreve os dados do buffer nos LEDs.
void ws2812b_write()
{
    // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
    for (uint i = 0; i < LED_MATRIX_COUNT; ++i)
    {
        pio_sm_put_blocking(led_matrix_pio, sm, led_matrix[i].G);
        pio_sm_put_blocking(led_matrix_pio, sm, led_matrix[i].R);
        pio_sm_put_blocking(led_matrix_pio, sm, led_matrix[i].B);
    }
    sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
}

// Desenha um número na matriz de LEDs.
void ws2812b_draw_number(uint8_t number_index)
{
    uint8_t r, g, b;

    // Atribui a cor do número atual.
    r = led_matrix_number_colors[number_index][0];
    g = led_matrix_number_colors[number_index][1];
    b = led_matrix_number_colors[number_index][2];

    // Limpa a matriz de LEDs.
    ws2812b_clear();
    ws2812b_write();

    // Desenha o número na matriz de LEDs.
    printf("Desenhando número %d\n", number_index);
    for (int i = 0; i < LED_MATRIX_COUNT; i++)
    {
        if (led_matrix_numbers[number_index][i] != 0)
        {
            ws2812b_set_led(i, r, g, b);
        }
    }

    // Atualiza a matriz de LEDs.
    ws2812b_write();
}
