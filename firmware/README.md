# Firmware
Firmware for Arduino based controller. This is a low-level part of the entire stack. The purpose of this layer is to read sensors and send data to a computer and read the instruction about motors and set it up.

Firmware code It requires the Adafruit MPU6050 and its required dependencies to be installed beforehand. Prior to compilation process, connection type must be selected by uncommenting either "#define _CONNECTION_WIFI" (for WiFi connection)  or "#define _CONNECTION_BLUETOOTH" (for Bluetooth or wired connection) in "soft.ino" file. For WiFi mode, the access point and connection settings must be configured in "soft.ino" file. However, the Bluetooth or wired connection mode is recommended.

# Installing
All procedures were tested on Linux platform (Ubuntu 20.04). All procedures are designed for Arduino UNO or compatible.

## Requirements and Environment setup
- Install Visual Studio Code, next install PlatformIO (from VSC application)
- in VSC, open PIO (PlatformIO) Home, in Libraries search for Adafruit_MPU6050, download and install
- open project from directory /firmware/
- in platformio.ini adjust if necessary "upload_port = /dev/ttyACM0"
- if you are not able to connect with arduino check permission to /dev/ttyACM0, if necessary:
-- sudo usermod -a -G tty yourname (access to /dev/tty*)
-- sudo usermod -a -G dialout yourname (access to /dev/tty*)

## Compilation and Upload
Before compilation one have to configure connectivity. In the src/main.cpp:
- Bluetooth: disable (comment out) #define CONNECTION_WIFI
- WiFi: enable #define CONNECTION_WIFI, adjust SSID/pass/ip below
- UART (cable): disable (comment out) #define CONNECTION_WIFI

Compile project in PIO (icon available on the VCS status bar). Disconnect connectivity (BT/WIFI/UART attached to moto shield). You have to upload firmware to Arduino by main (USB square) port, any other connectivity has to be disconnected.

# Start
After reset, the firmware is ready. It does not blink led, do not start automatically motors.