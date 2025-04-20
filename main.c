#include <stdio.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/bootrom.h"

#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/adc.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"

#include "lib/ssd1306.h"
#include "lib/font.h"
#include "lib/ws2812b.h"
#include "lib/rectangle.h"

// Variáveis de configuração
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define I2C_ADDRESS 0x3C
#define LED_MATRIX_PIN 7
#define LED_MATRIX_SIZE 5
#define GREEN_LED_PIN 11
#define BLUE_LED_PIN 12
#define RED_LED_PIN 13
#define BTN_A_PIN 5
#define BTN_B_PIN 6
#define BUZZER_A_PIN 21
#define VRX_PIN 27
#define VRY_PIN 26
#define SW_PIN 22
#define ADC_MAX_VALUE 4096
#define ADC_HALF_VALUE 2048
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define RECT_SIZE 8

// Cabeçalho das funções
void init_led(uint8_t led_pin);
void init_btn(uint8_t btn_pin);
void init_leds();
void init_btns();
void init_i2c();
void init_display(ssd1306_t *ssd);
void init_joystick();
void pwm_init_buzzer(uint pin);
void play_tone(uint pin, uint frequency);
void read_joystick_xy_values(uint16_t *x_value, uint16_t *y_value);
int get_rect_delta_x(rect_t *rect, int vrx_value, int speed);
int get_rect_delta_y(rect_t *rect, int vry_value, int speed);
int random_number(int min, int max);
int xy_to_index(int x, int y);
void gpio_irq_handler(uint gpio, uint32_t events);
bool blink_red_led_callback(struct repeating_timer *t);
void signal_life_status(int8_t life);

// Variáveis globais
static volatile int64_t last_valid_press_time_btn_a = 0; // Tempo do último pressionamento do botão A
static volatile int64_t last_valid_press_time_btn_b = 0; // Tempo do último pressionamento do botão B
static volatile int8_t spaceship_index = 2; // Índice do LED da nave
struct repeating_timer red_led_timer; // Timer para piscar o LED vermelho
bool red_led_timer_active = false; // Variável de controle do timer
static volatile bool game_started = false; // Variável de controle do jogo

int main()
{
    stdio_init_all();
    srand(time_us_32()); // Inicializa o gerador de números aleatórios

    ssd1306_t ssd; // Inicializa a estrutura do display
    uint16_t vrx_value_raw; // Inicializa o valor bruto do eixo X
    uint16_t vry_value_raw; // Inicializa o valor bruto do eixo Y
    rect_t rect; // Inicializa a estrutura do retângulo
    bool has_meteor = false; // Inicializa a variável de controle do meteorito
    int meteor_index = 0; // Inicializa o índice do meteorito
    static int8_t meteor_x; // Inicializa a posição x do meteorito
    static int8_t meteor_y; // Inicializa a posição y do meteorito
    int8_t life = 3; // Inicializa a vida do jogador

    init_rectangle(&rect, (DISPLAY_WIDTH - RECT_SIZE) / 2, (DISPLAY_HEIGHT - RECT_SIZE) / 2, RECT_SIZE, RECT_SIZE);
    init_leds();
    init_btns();
    init_i2c();
    init_display(&ssd);
    pwm_init_buzzer(BUZZER_A_PIN);
    ws2812b_init(LED_MATRIX_PIN); // Inicializa a matriz de LEDs
    adc_init();
    init_joystick();

    gpio_set_irq_enabled_with_callback(BTN_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled(BTN_B_PIN, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(SW_PIN, GPIO_IRQ_EDGE_FALL, true);

    ws2812b_clear();
    ws2812b_set_led(spaceship_index, 0, 0, 8); // Inicializa a nave
    ws2812b_write(); // Atualiza a matriz de LEDs

    while (true) {
        // Lê os valores do joystick
        read_joystick_xy_values(&vrx_value_raw, &vry_value_raw);
        vry_value_raw = ADC_MAX_VALUE - vry_value_raw; // Inverte o eixo Y

        // Calcula o delta a partir do centro
        int delta_x = get_rect_delta_x(&rect, vrx_value_raw, 80);
        int delta_y = get_rect_delta_y(&rect, vry_value_raw, 80);

        // Atualiza a posição do retângulo
        set_rectangle_position(&rect, rect.x + delta_x, rect.y + delta_y);

        // Atualiza o display
        ssd1306_fill(&ssd, false);
        ssd1306_rect(&ssd, rect.y, rect.x, rect.width, rect.height, true, true);
        ssd1306_send_data(&ssd); // Envia os dados para o display

        if (game_started) {
            // Verifica se há um meteorito
            if (!has_meteor) {
                meteor_y = 4;
                meteor_x = random_number(0, 4); // Posição x aleatória
                has_meteor = true;
            }

            // Pega o indice do meteorito
            meteor_index = xy_to_index(meteor_x, meteor_y);

            // Atualiza o LED da matriz
            ws2812b_clear();
            ws2812b_set_led(meteor_index, 8, 0, 0); // Atualiza o LED
            ws2812b_set_led(spaceship_index, 0, 0, 8); // Limpa o LED

            // Verifica se o retângulo colidiu com o meteorito
            if (spaceship_index == meteor_index) {
                ws2812b_set_led(meteor_index, 8, 8, 0); // Exibe uma explosão
                ws2812b_write();

                life--;
                printf("Meteor hit!\n");
                printf("Life: %d\n", life);

                has_meteor = false;
                play_tone(BUZZER_A_PIN, 300); // Toca um tom de buzzer
                sleep_ms(500);
                pwm_set_gpio_level(BUZZER_A_PIN, 0); // Desliga o buzzer
                continue;
            }

            signal_life_status(life);

            // Desenha o LED da matriz
            ws2812b_write();

            // Atualiza a posição do meteorito
            meteor_y--;

            // Verifica se o meteorito saiu da tela
            if (meteor_y < 0) {
                has_meteor = false;
            }

            // Game over
            if (life <= 0) {
                game_started = false;
                has_meteor = false;
                life = 3; // Reseta a vida
                spaceship_index = 2; // Reseta a nave para o meio
                printf("Game Over\n");
            }
        }


        sleep_ms(150);
    }
}

// Inicializa um led em um pino específico
void init_led(uint8_t led_pin)
{
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
}

// Inicializa um botão em um pino específico
void init_btn(uint8_t btn_pin)
{
    gpio_init(btn_pin);
    gpio_set_dir(btn_pin, GPIO_IN);
    gpio_pull_up(btn_pin);
}

// Inicializa o LED RGB (11 - Azul, 12 - Verde, 13 - Vermelho)
void init_leds()
{
    init_led(BLUE_LED_PIN);
    init_led(GREEN_LED_PIN);
    init_led(RED_LED_PIN);
}

// Inicializa os botões A e B (5 - A, 6 - B)
void init_btns()
{
    init_btn(BTN_A_PIN);
    init_btn(BTN_B_PIN);
}

// Inicializa a comunicação I2C
void init_i2c()
{
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

// Inicializa o display OLED
void init_display(ssd1306_t *ssd)
{
    ssd1306_init(ssd, WIDTH, HEIGHT, false, I2C_ADDRESS, I2C_PORT);
    ssd1306_config(ssd);
    ssd1306_send_data(ssd);

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(ssd, false);
    ssd1306_send_data(ssd);
}

// Inicializa o joystick
void init_joystick()
{
    adc_gpio_init(VRX_PIN);
    adc_gpio_init(VRY_PIN);

    init_btn(SW_PIN);
}

// Realiza a amostragem e atualiza os valores X e Y do joystick
void read_joystick_xy_values(uint16_t *x_value, uint16_t *y_value)
{
    adc_select_input(1); // Seleciona o ADC para eixo X. O pino 26 como entrada analógica
    *x_value = adc_read();
    sleep_us(20);

    adc_select_input(0); // Seleciona o ADC para eixo Y. O pino 27 como entrada analógica
    *y_value = adc_read();
    sleep_us(20);
}

// Calcula o delta X retângulo a partir dos valores do joystick
int get_rect_delta_x(rect_t *rect, int vrx_value, int speed) {
    // Calcula o delta a partir do centro
    int delta_x = (vrx_value - ADC_HALF_VALUE) / speed;

    if (rect->x + delta_x < 0) {
        delta_x = -rect->x;
    } else if (rect->x + delta_x > DISPLAY_WIDTH - rect->width) {
        delta_x = DISPLAY_WIDTH - rect->width - rect->x;
    }

    return delta_x;
}

// Calcula o delta Y retângulo a partir dos valores do joystick
int get_rect_delta_y(rect_t *rect, int vry_value, int speed) {
    // Calcula o delta a partir do centro
    int delta_y = (vry_value - ADC_HALF_VALUE) / speed;

    if (rect->y + delta_y < 0) {
        delta_y = -rect->y;
    } else if (rect->y + delta_y > DISPLAY_HEIGHT - rect->height) {
        delta_y = DISPLAY_HEIGHT - rect->height - rect->y;
    }

    return delta_y;
}

// Gera número aleatório entre min e max (inclusive)
int random_number(int min, int max) {
    return min + (rand() % (max - min + 1));
}

// Função para converter coordenadas x,y em índice do LED
int xy_to_index(int x, int y) {
    // Se a linha é par, a ordem é da esquerda para direita
    if (y % 2 == 0) {
        return y * LED_MATRIX_SIZE + x;
    }
    // Se a linha é ímpar, a ordem é da direita para esquerda
    else {
        return y * LED_MATRIX_SIZE + (LED_MATRIX_SIZE - 1 - x);
    }
}

// Função de interrupção para os botões
void gpio_irq_handler(uint gpio, uint32_t events) {
    int64_t current_time = to_ms_since_boot(get_absolute_time());

    if (gpio == BTN_A_PIN && current_time - last_valid_press_time_btn_a > 275) {
        last_valid_press_time_btn_a = to_ms_since_boot(get_absolute_time());

        if (!game_started) {
            game_started = true;
            spaceship_index = 2; // Reseta a nave para o meio
            printf("Game started\n");
        } else {
            if (spaceship_index < 4) {
                spaceship_index++;
            } else {
                spaceship_index = 4;
            }
        }

    } else if (gpio == BTN_B_PIN && current_time - last_valid_press_time_btn_b > 275) {
        last_valid_press_time_btn_b = to_ms_since_boot(get_absolute_time());

        if (!game_started) {
            game_started = true;
            spaceship_index = 2; // Reseta a nave para o meio
            printf("Game started\n");
        } else {
            if (spaceship_index > 0) {
                spaceship_index--;
            } else {
                spaceship_index = 0;
            }
        }
    } else if (gpio == SW_PIN) {
        printf("SW pressed\n");
        reset_usb_boot(0, 0);
    }
}

// Função Callback para piscar o LED
bool blink_red_led_callback(struct repeating_timer *t) {
    gpio_put(RED_LED_PIN, !gpio_get(RED_LED_PIN));
    printf("Blinking red LED\n");

    return true; // Retorna true para continuar repetindo
}

// Função para sinalizar o status da vida
// 3 vidas: Verde, 2 vidas: Azul, 1 vida: Pisca vermelho, 0 vidas: Vermelho fixo
void signal_life_status(int8_t life) {
    switch(life) {
        case 3:
            gpio_put(GREEN_LED_PIN, 1);  // Verde significa vida cheia
            gpio_put(RED_LED_PIN, 0);
            gpio_put(BLUE_LED_PIN, 0);
            break;
        case 2:
            gpio_put(BLUE_LED_PIN, 1);   // Azul significa 2 vidas
            gpio_put(GREEN_LED_PIN, 0);
            gpio_put(RED_LED_PIN, 0);
            break;
        case 1:
            gpio_put(BLUE_LED_PIN, 0);
            gpio_put(GREEN_LED_PIN, 0);

            if (!red_led_timer_active) {
                add_repeating_timer_ms(-250, blink_red_led_callback, NULL, &red_led_timer); // Pisca vermelho 3x quando só tem 1 vida
                red_led_timer_active = true;
            }

            break;
        case 0:
            if (red_led_timer_active) {
                cancel_repeating_timer(&red_led_timer);
                red_led_timer_active = false;
                gpio_put(RED_LED_PIN, 1);    // Vermelho fixo significa game over
            }

            break;
    }
}

// Inicializa o PWM no pino do buzzer
void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f); // Ajusta divisor de clock
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(pin, 0); // Desliga o PWM inicialmente
}

// Toca uma nota com a frequência
void play_tone(uint pin, uint frequency) {

    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint32_t clock_freq = clock_get_hz(clk_sys);
    uint32_t top = clock_freq / frequency - 1;

    pwm_set_wrap(slice_num, top);
    pwm_set_gpio_level(pin, top / 2); // 50% de duty cycle
}
