#ifndef DHT_H
#define DHT_H

#include <hardware/pio.h>
#include <stdint.h>

static const float PIO_SM_CLK_FREQ = 1000000.0;         // MHz
static const uint DHT_LONG_CYCLE = 50;                  // μs
static const uint DHT_MEASUREMENT_TIMEOUT_US = 6000;    // μs
static const uint DHT_START_CYCLE_1 = 1000;             // μs
static const uint DHT_START_CYCLE_2 = 18000;            // μs

typedef struct 
{
    PIO pio;
    uint8_t model;
    uint8_t pio_program_offset;
    uint8_t sm;
    uint8_t dma_channel;
    uint8_t data_pin;
    uint8_t data[5];
    uint32_t start_time;
} DhtStructTypeDef;

typedef struct 
{
    float start_time;
    float long_cycle_time;
} TimingStructTypeDef;

typedef enum 
{
    DHT11,
    DHT12,
    DHT21,
    DHT22,
} DhtEnumTypeDef;

typedef enum 
{
    DHT_RESULT_OK,              /**< No error.*/
    DHT_RESULT_TIMEOUT,         /**< DHT sensor not reponding. */
    DHT_RESULT_BAD_CHECKSUM,    /**< Sensor data doesn't match checksum. */
} ResultEnumTypeDef;

/**
 * \brief Initialize DHT sensor.
 * 
 * The library claims one state machine from the given PIO instance, and one DMA
 * channel to communicate with the sensor.
 * 
 * \param dht DHT sensor.
 * \param model DHT sensor model.
 * \param pio PIO block to use (pio0 or pio1).
 * \param data_pin Sensor data pin.
 * \param pull_up Whether to enable the internal pull-up.
 */
void dht_init(DhtStructTypeDef *dht, DhtEnumTypeDef model, PIO pio, uint8_t data_pin, bool pull_up);

/**
 * \brief Deinitialize DHT sensor.
 *
 * \param dht DHT sensor.
 */
void dht_deinit(DhtStructTypeDef *dht);

/**
 * \brief Start asynchronous measurement.
 *
 * The measurement runs in the background, and may take up to 25ms depending
 * on DHT model.
 * 
 * DHT sensors typically need at least 2 seconds between measurements for
 * accurate results.
 * 
 * \param dht DHT sensor.
 */
void dht_start_measurement(DhtStructTypeDef *dht);

/**
 * \brief Wait for measurement to complete and get the result.
 *
 * \param dht DHT sensor.
 * \param[out] humidity Relative humidity. May be NULL.
 * \param[out] temperature Degrees Celsius. May be NULL.
 * \return Result status.
 */
ResultEnumTypeDef dht_finish_measurement_blocking(DhtStructTypeDef *dht, float *humidity, float *temperature, bool celsius);

/**
 * \brief Read humidity and temperature data.
 *
 * \param dht DHT sensor.
 * \param[out] humidity Relative humidity. May be NULL.
 * \param[out] temperature Degrees Celsius. May be NULL.
 * \param[out] celsius Wheater to return temperature in Celcius or Fahrenheit.
 * \return Result status.
 */
ResultEnumTypeDef dht_read(DhtStructTypeDef *dht, float *humidity, float *temperature, bool celsius);
#endif
