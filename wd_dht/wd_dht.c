#include <pico/stdlib.h>
#include <hardware/dma.h>
#include <math.h>
#include <string.h>
#include <wd_dht.h>
#include "dht_read_data.pio.h"

TimingStructTypeDef timing_struct;

static uint calc_start_timing(DhtEnumTypeDef model)
{
    return (model == DHT21 || model == DHT22) ? DHT_START_CYCLE_1 : DHT_START_CYCLE_2;
}

static uint get_pio_sm_clocks(uint us) 
{
    return roundf(us * PIO_SM_CLK_FREQ / 1000000.0f);
}

static bool pio_sm_is_enabled(PIO pio, uint sm)
{
    return (pio->ctrl & (1 << sm)) != 0;
}

static float celsius_to_fahrenheit(float temperature) 
{
    return temperature * (9.0f / 5) + 32;
}

static void configure_dma_channel(uint channel, PIO pio, uint sm, uint8_t *write_addr)
{
    dma_channel_config c = dma_channel_get_default_config(channel);
    channel_config_set_dreq(&c, pio_get_dreq(pio, sm, false));
    channel_config_set_irq_quiet(&c, true);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    dma_channel_configure(channel, &c, write_addr, &pio->rxf[sm], 5, true);
}

static float decode_temperature(DhtEnumTypeDef model, uint8_t byte0, uint8_t byte1)
{
    float temperature;
    switch (model)
    {
        case DHT11:
            // Below-zero temperature not supported by DHT11
            temperature = (byte1 & 0x80) ? 0.0f : byte0 + 0.1f * (byte1 & 0x7F);
            break;
        case DHT12:
            temperature = byte0 + 0.1f * (byte1 & 0x7F);
            if (byte1 & 0x80) { temperature = -temperature; }
            break;
        case DHT21:
        case DHT22:
            temperature = 0.1f * (((byte0 & 0x7F) << 8) + byte1);
            if (byte0 & 0x80) { temperature = -temperature; }
            break;
        default:
            assert(false); // Throw error in case of invalid model
    }
    return temperature;
}

static float decode_humidity(DhtEnumTypeDef model, uint8_t byte0, uint8_t byte1)
{
    float humidity;
    switch (model)
    {
        case DHT11:
        case DHT12:
            humidity = byte0 + 0.1f * byte1;
            break;
        case DHT21:
        case DHT22:
            humidity = 0.1f * ((byte0 << 8) + byte1);
            break;
        default:
            assert(false); // Throw error in case of invalid model
    }
    return humidity;
}

void dht_init(DhtStructTypeDef *dht, DhtEnumTypeDef model, PIO pio, uint8_t data_pin, bool pull_up)
{
    // Check if propper programmed IO has been provided
    assert(pio == pio0 || pio == pio1);

    // Define DHT structure parameter values
    memset(dht, 0, sizeof(DhtStructTypeDef));
    dht->model = model;
    dht->pio = pio;
    dht->pio_program_offset = pio_add_program(pio, &dht_read_data_program);
    dht->sm = pio_claim_unused_sm(pio, true);
    dht->dma_channel = dma_claim_unused_channel(true);
    dht->data_pin = data_pin;

    // Define timing structure parameter values
    timing_struct.start_time = get_pio_sm_clocks(calc_start_timing(model));
    timing_struct.long_cycle_time = get_pio_sm_clocks(DHT_LONG_CYCLE / 2);

    pio_gpio_init(pio, data_pin);
    gpio_set_pulls(data_pin, pull_up, false);
}

void dht_deinit(DhtStructTypeDef *dht)
{
    assert(dht->pio != NULL); // Not initialized

    dma_channel_abort(dht->dma_channel);
    dma_channel_unclaim(dht->dma_channel);

    pio_sm_set_enabled(dht->pio, dht->sm, false);
    // Make sure pin is left in high impedance mode; original pin function and pulls are not restored
    pio_sm_set_consecutive_pindirs(dht->pio, dht->sm, dht->data_pin, 1, false);
    pio_sm_unclaim(dht->pio, dht->sm);
    pio_remove_program(dht->pio, &dht_read_data_program, dht->pio_program_offset);

    dht->pio = NULL;
}

void dht_start_measurement(DhtStructTypeDef *dht)
{
    assert(dht->pio != NULL); // Mot initialized
    assert(!pio_sm_is_enabled(dht->pio, dht->sm)); // Another measurement in progress

    memset(dht->data, 0, sizeof(dht->data));
    configure_dma_channel(dht->dma_channel, dht->pio, dht->sm, dht->data);
    read_data_program_init(dht->pio, dht->sm, dht->pio_program_offset, dht->data_pin, PIO_SM_CLK_FREQ, timing_struct);
    dht->start_time = time_us_32();
}

ResultEnumTypeDef dht_finish_measurement_blocking(DhtStructTypeDef *dht, float *humidity, float *temperature, bool celsius)
{
    assert(dht->pio != NULL); // Check if pio has been initialized
    assert(pio_sm_is_enabled(dht->pio, dht->sm)); // No measurement in progress

    uint32_t timeout = calc_start_timing(dht->model) + DHT_MEASUREMENT_TIMEOUT_US;
    while (dma_channel_is_busy(dht->dma_channel) && time_us_32() - dht->start_time < timeout) { tight_loop_contents(); }
    pio_sm_set_enabled(dht->pio, dht->sm, false);
    // Make sure pin is left in high impedance mode
    pio_sm_exec(dht->pio, dht->sm, pio_encode_set(pio_pindirs, 0));

    if (dma_channel_is_busy(dht->dma_channel))
    {
        dma_channel_abort(dht->dma_channel);
        return DHT_RESULT_TIMEOUT;
    }
    uint8_t checksum = dht->data[0] + dht->data[1] + dht->data[2] + dht->data[3];
    if (dht->data[4] != checksum) { return DHT_RESULT_BAD_CHECKSUM; }
    if (humidity != NULL) { *humidity = decode_humidity(dht->model, dht->data[0], dht->data[1]); }
    if (temperature != NULL)
    {
        *temperature = decode_temperature(dht->model, dht->data[2], dht->data[3]);
        *temperature = (celsius) ? *temperature : celsius_to_fahrenheit(*temperature);
    }
    return DHT_RESULT_OK;
}

ResultEnumTypeDef dht_read(DhtStructTypeDef *dht, float *humidity, float *temperature, bool celsius)
{
    dht_start_measurement(dht);
    return dht_finish_measurement_blocking(dht, humidity, temperature, celsius);
}
