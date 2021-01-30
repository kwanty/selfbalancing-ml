# PC connectivity
This algorithm connects with Arduino (Wifi/BT/UART-cable). This is only an example of use. Allows for setting motors and IMU packet rate, as well as for handling received IMU data.

## Compilation
Connection mode needs to be selected in "main.cpp" file be uncommenting either "#define _MODE_WIFI" (for WiFi connection) or "#define _MODE_BLUETOOTH" (for Bluetooth or wired connection). For Bluetooth/wired connection proper serial port must be selected. For WiFi connection, the computer must be connected to the ESP32 network using the OS interface. Moreover, the connection settings in this file must match settings in the robot firmware. 

The application uses the Qt framework and should be compiled and run it this environment. Compiling from CMD:
- cd sbr-qt/
- qmake -p sbr-test.pro
- make

## Run
From CMD:
- cd sbr-qt/
- ./sbr-test