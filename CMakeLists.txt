cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)

project(alarmeInterativo C CXX ASM)

pico_sdk_init()

add_executable(alarmeInterativo alarmeInterativo.c inc/ssd1306_i2c.c inc/matriz_leds.c)

pico_generate_pio_header(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/inc/pio_matrix.pio)

pico_enable_stdio_uart(alarmeInterativo 1)
pico_enable_stdio_usb(alarmeInterativo 1)

target_link_libraries(alarmeInterativo
        pico_stdlib 
        hardware_adc 
        hardware_pwm
        hardware_clocks
        hardware_i2c
        pico_multicore
        hardware_pio)

target_include_directories(alarmeInterativo PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(alarmeInterativo)

