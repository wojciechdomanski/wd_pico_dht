# Raspberry Pi Pico DHT sensor driver

The intention of this project was to learn C coding and integrating drivers for peripherials. It's a library handling one-wire communication and data conversion for DHT sensors, for Raspberry Pi Pico. For best omptimization the PIO state machines have been used for communication.

This project makes use of Wokwi board simulator, which can be used in Visual Studio Code IDE.

# Setup

CMake and Pico SDK are necessary to build this library. As an option, diagram.json and wokwi.toml files are included for board simulation.

# User manual

In order to use the driver you need to:
1. Define signal pin for one-wire communication
2. Define DHT model
3. Initialize DHT driver
4. Call *dht_read* function