# Interfacing Pololu AltIMU-v6 with STM32
## Overview
This repository contains example code for interfacing the Pololu AltIMU-v10 with the STM32-L073RZ MCU for our test purposes. The AltIMU-v10 is a versatile sensor that combines an accelerometer, gyroscope, and magnetometer, making it ideal for various motion and orientation applications.
More information + datasheets to all sensors here: https://www.pololu.com/product/2863.

## Connections

| AltIMU-v10 Pin | STM32 GPIO Pin  |
|----------------|-----------------|
| VIN            | PA14 (3.3V)            |
| GND            | GND             |
| SDA            | PA9             |
| SCL            | PA10            |
| SA0            | GND             |


This repo has been created in reference to [this issue](https://github.com/waterloo-rocketry/cansw_processor_canards/issues/1).
