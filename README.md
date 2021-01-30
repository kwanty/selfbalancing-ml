# Self-balancing scooter type robot
The aim of this project is to create a self-balancing platform for testing algorithms based on Machine Learning.

It consists of two parts. The first part is firmware loaded to the Arduino (folder firmware). The second part is a control algorithm written in C/Python in folder sbr-qt or sbr-py.

## description
Self Balancing Robot Platform (hereinafter SBR) is a hardware platform and a firmware for it, which is meant to serve as a test platform, primarily for AI algorithm testing. The provided software provides an ability to control the robot using either a wired connection (as a serial port) or a wireless connection: WiFi (as an access point) or Bluetooth, which is transparent for both the robot and the computer and behaves as a standard serial port. This means that the wired and Bluetooth connections are identical from the software point of view. For communication, the special protocol (described below) is used.

The power supply should be attached to a screw connector (green) on the top of the motor shield or a standard 12V plug on the left on the green connector (both are connected). A robot can drain up to 7A from the power supply (usually no more than 0.5A). ESP/BT connectivity has to be connected to the gold-pin available on the shield. They do not require programming (Arduino uses a standard AT command). Both modules have to be disconnected during Arduino programming (upload firmware)

## Launching and using
Three ways of communication between Arduino and PC are possible:
- WiFi connection (eg. ESP32), the following pins should be connected between ESP32 and Arduino: VIN to 5V, TX2 to RX, RX2 to TX, GND to GND.
- Bluetooth connection using HC-06 module, insert the module directly into the header and make sure that the following pins are connected properly: VCC to 5V, RX to TX, TX to RX, and GND to GND. 
- Cable UART connect VCC, V5, RX/TX (crossed to TX/RX) - like Bluetooth

## Communication protocol
it corresponds to the communication protocol between Arduino and PC. 

There is a standardized protocol used, which is implemented in SBRCP.h and SBRCP.cpp files. All packets include CRC-8-CCITT checksum (0x07 polynomial) calculated over all packet bytes (excluding the CRC itself and LF-CR bytes). At the end of every packet (after the checksum) the LF-CR bytes must be present. Please mind their order! It's different from the standard CR-LF line endings. Multi-byte elements are sent each byte separately in the little-endian order.

### PC-to-robot packets

**Motor speed setting**:
content:       |0x2F| motor A| motor B| CRC| LF| CR|
byter number:  |   0|    1, 2|    3, 4|   5|  6|  7|

Motor speed is a signed 16-bit integer (int16_t). Correct values are 1 to 255 for forward rotation, -1 to -255 for backward rotation. 0 stops the motor. Values outside this range are clipped to the nearest valid value.

**MPU6050 data interval setting**:
content:      |0xA7|   interval| CRC| LF| CR|
byte number:  |   0| 1, 2, 3, 4|   5|  6|  7|

Interval is in microseconds and is an unsigned 32-bit integer (uint32_t). Can't be smaller than 5000 us. Smaller values are clipped to 5000 us.

### Robot-to-PC packets

**MPU6050 data packet**:
content:      |0x35| accelerometer X| accelerometer Y| accelerometer Z|
byte number:  |   0|      1, 2, 3, 4|      5, 6, 7, 8|   9, 10, 11, 12|

content:      |    gyroscope X|    gyroscope Y|    gyroscope Z| CRC| LF| CR|
byte number:  | 13, 14, 15, 16| 17, 18, 19, 20| 21, 22, 23, 24|  25| 26| 27|

Accelerometer and gyroscope data are 32-bit floats. Accelerometer values unit is m/s^2, gyroscope - rad/s.

**Error packet**:
content:      |0xEE|error code| CRC| LF| CR|
byte number:  |   0|         1|   2|  3|  4|

Error codes:
0x00 - other error (ERROR_OTHER)
0x01 - MPU6050 read error (ERROR_MPU_READ)
0x02 - MPU6050 initialization fail (ERROR_MPU_INIT)
0x03 - incorrect command (ERROR_ILLEGAL_CMD)

## Author and licensing
Author: Piotr Wilkon <student@agh.edu.pl>
License: GNU GPLv3, a copy of the license is included with this project

## TODO
- SBRCP.h and SBRCP.cpp protocol files are copied and provided separately to the firmware and example PC program but are identical. Something should be done about it.
- measure communication delays (but how?)
- ESP32 is in AT commands mode. It would be better to create own communication protocol that would be immune e.g. to dropped bytes