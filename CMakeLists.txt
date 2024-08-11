cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(wd_dht_example C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_subdirectory(wd_dht)

add_executable(wd_dht_example wd_dht_example.c)

pico_enable_stdio_uart(wd_dht_example 1)
pico_enable_stdio_usb(wd_dht_example 0)

pico_add_extra_outputs(wd_dht_example)

target_compile_options(wd_dht_example PRIVATE -Wall)

target_link_libraries(wd_dht_example wd_dht pico_stdlib)