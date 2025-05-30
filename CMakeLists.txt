cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)

# Set the project name and version
# The project name is used to create the executable name
project(tarefa1_revisao_embarcatech C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(${PROJECT_NAME} main.c lib/ssd1306.c lib/ws2812b.c
lib/led_matrix_numbers.c lib/rectangle.c)

pico_set_program_name(${PROJECT_NAME}  "tarefa1_revisao_embarcatech")
pico_set_program_version(${PROJECT_NAME}  "0.1")

# Generate PIO header
pico_generate_pio_header(${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}/led_matrix.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(${PROJECT_NAME}  1)
pico_enable_stdio_usb(${PROJECT_NAME}  1)

# Add the standard library to the build
target_link_libraries(${PROJECT_NAME}
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(${PROJECT_NAME}  PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(${PROJECT_NAME}
        hardware_i2c
        hardware_pio
        hardware_adc
        hardware_clocks
        hardware_timer
        hardware_pwm
        )

pico_add_extra_outputs(${PROJECT_NAME})

