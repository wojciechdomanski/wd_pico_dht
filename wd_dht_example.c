#include <stdio.h>
#include <pico/stdlib.h>
#include <wd_dht.h>
 
#define DHTPIN 15          // Signal pin
#define DHTTYPE DHT22      // Sensor type

int main()
{
  // Initialize IO for terminal
  stdio_init_all();
  printf("Hello, Raspberry Pi Pico!\n");

  // Initialize DHT
  DhtStructTypeDef dht;
  dht_init(&dht, DHTTYPE, pio0, DHTPIN, true);

  // Loop
  do {
    float humidity, temperature;
    bool celsius = true;
    ResultEnumTypeDef result = dht_read(&dht, &humidity, &temperature, celsius);

    if (result == DHT_RESULT_OK) 
    {
      printf("\n=============== DHT22 Readings ==============\n");
      printf("Temperature:\t%.1f°C\n", temperature);
      printf("Humidity:\t%.1f%%\n", humidity);
    } 
    else if (result == DHT_RESULT_TIMEOUT) { printf("DHT sensor not responding. Please check your wiring.\n"); } 
    else 
    {
      assert(result == DHT_RESULT_BAD_CHECKSUM);
      printf("Bad checksum\n");
    }
    sleep_ms(2000);
  } while(true);
}