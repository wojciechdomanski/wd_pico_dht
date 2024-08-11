# Raspberry Pi Pico DHT sensor driver

The intention of this project was to learn **C** coding and integrate drivers for peripherals. It’s a library that handles one-wire communication and data conversion for **DHT sensors** on the **Raspberry Pi Pico**. For optimal optimization, the PIO state machine has been used for communication.

This project makes use of the **Wokwi board simulator**[^1], which can be used in the **Visual Studio Code IDE**[^2].

## Setup

To build this library, you will need **CMake** and **Pico SDK**[^3]. Additionally, the `diagram.json` and `wokwi.toml` files are included as options for board simulation.

## User manual

To use the driver, follow these steps:
1. Define the signal pin for one-wire communication.
2. Define the DHT model.
3. Initialize the DHT driver.
4. Call the `dht_read` function.
5. Wait for 2 seconds before making another function call (communication with the sensor takes some time).


## Wiring

Based on Wokwi diagram

![wiring](https://i.imgur.com/QkwtkvU.png)

## Video presentation

The Wokwi VCS extension enables real-time changes to the sensor’s reading.

[![Watch the video](https://img.youtube.com/vi/ZFBnU7sp4zc/0.jpg)](https://www.youtube.com/watch?v=ZFBnU7sp4zc)

## Thanks

I've learned a lot from Life with **David's videos**[^4] and **Valentin Milea's GitHub**[^5]

## References
[^1]: **Wokwi** https://docs.wokwi.com/?utm_source=wokwi
[^2]: **Visual Studio Code** https://code.visualstudio.com/
[^3]: **Pico SDK** https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf
[^4]: **Life with David, which helped me with PIO** https://www.youtube.com/watch?v=zeudTftbTmw&list=PLiRALtgGsxmZs_LXGkh09Zr2NUmk_mtEI&index=8
[^5]: **Valentin Milea's DHT library, which helped me learn** https://github.com/vmilea/pico_dht/tree/master
