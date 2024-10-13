cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

# Pico SDK uses C++ and ASM
project(pico_timer C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(${CMAKE_PROJECT_NAME}
	main.c
	ssd1306.c
	xpm.c
)

# Enable USB serial output
pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)

pico_add_extra_outputs(${CMAKE_PROJECT_NAME})

target_link_libraries(${CMAKE_PROJECT_NAME} pico_stdlib hardware_i2c)
