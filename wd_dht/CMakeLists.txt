add_library(wd_dht INTERFACE)

pico_generate_pio_header(wd_dht ${CMAKE_CURRENT_LIST_DIR}/dht_read_data.pio)

target_include_directories(wd_dht
    INTERFACE
    ./include
)

target_sources(wd_dht
    INTERFACE
    wd_dht.c
)

target_link_libraries(wd_dht
    INTERFACE
    hardware_clocks
    hardware_dma
    hardware_pio
)