# Self-Balancing Robot Firmware

## Overview
This is the low-level firmware for the Arduino UNO based controller. It is responsible for reading data from the MPU6050 sensor, dispatching this data to a connected PC, and receiving motor control instructions to drive the attached TB6612 2WD motor shield.

## Documentation

### 1. Arduino to TB6612 2WD Communication
The Arduino controls the TB6612 H-bridge motor driver shield through direct digital and PWM pins. The protocol is implemented on a hardware-level (simple logic high/low and PWM signals).

**Used Pins:**
- **Motor A (Left/Right):**
  - `AIN1` (Pin 13) - Direction control
  - `AIN2` (Pin 12) - Direction control
  - `PWMA` (Pin 6) - Speed control (PWM)
- **Motor B (Right/Left):**
  - `BIN1` (Pin 8) - Direction control
  - `BIN2` (Pin 7) - Direction control
  - `PWMB` (Pin 3) - Speed control (PWM)

**Motor Control Logic:**
- To rotate clockwise: `IN1` = HIGH, `IN2` = LOW. The speed is controlled via PWM mapped to `analogWrite` (0-255).
- To rotate counterclockwise: `IN1` = LOW, `IN2` = HIGH. The speed is analogWrite (0-255).
- To stop/brake: `IN1` = HIGH, `IN2` = HIGH. PWM is set to 0.

### 2. PC Communication via UART
When a wired connection or Bluetooth is used (default), the Arduino communicates with the PC using UART over the default hardware serial interface.

- **Baud rate**: `115200` bps
- **Protocol Structure**: The connection uses a custom binary protocol with framing. Each packet contains:
  - **Type byte** (1 byte): Defines the instruction type. E.g., `0x35` for MPU data from Arduino, `0xA7` for MPU Rate settings from PC, and `0x2F` for motor commands from PC.
  - **Payload** (variable-length): Raw memory representation of data, generally parsed byte-to-byte using `memcpy` or equivalent.
  - **CRC8** (1 byte): Error-checking code (Initial start value `0xFF`, polynomial `0x07`).
  - **Ending tags**: `\n\r` (2 bytes, 0x0A 0x0D).

## Installing & Uploading

### Environment Setup
1. **Platform:** The project is built using PlatformIO. You have two options for your environment:
   - **Method A (GUI):** Install Visual Studio Code (VSC) and add the **PlatformIO IDE** extension.
   - **Method B (CLI / Conda):** Install PlatformIO Core via `pip`, directly into your project's Conda environment. This is highly recommended for terminal-centric setups or agentic IDEs.
     ```bash
     conda activate selfbalancing
     pip install -U platformio
     ```
2. Library dependencies, including `Adafruit MPU6050`, are explicitly defined in `platformio.ini` and will be downloaded automatically by the PlatformIO system upon build.

### Compilation and Upload Process
Perform the following steps whenever you need to upload standard firmware to the Arduino UNO.

1. **Open Project:** Open the `/firmware/` directory in Visual Studio Code. Wait a few seconds for PlatformIO to finish its background initialization.
2. **Verify Configuration:** By default, the UART (Cable/Bluetooth) connection mode is selected.
    *To switch to WiFi mode, you would uncomment `#define _CONNECTION_WIFI` in `src/main.cpp`.*
3. **Set Upload Port:** Open `platformio.ini` and verify the `upload_port`. It is important to set this to the correct USB port your Arduino is connected to (such as `/dev/ttyUSB0` or `/dev/ttyACM0`). You can remove the `upload_port` line entirely to let PlatformIO auto-detect the port automatically.
    - *Linux permission tip:* Ensure your user has permissions to the serial port, executing `sudo usermod -a -G dialout $USER` and restarting if you encounter permission denied errors.
4. **Hardware preparation:** If you're uploading via the main Arduino USB port, **disconnect any Bluetooth/WiFi or external RS232 converters currently bridged to Arduino's RX/TX (pins 0/1)** to avoid flashing interruptions.
5. **Upload:** 
   - **Using UI:** Click the right-pointing arrow (`Upload`) icon located in the lower-left status bar of VSC PlatformIO.
   - **Using Terminal:** Use the terminal and run `pio run -t upload` inside the `firmware/` directory.

## Start
After flashing and successfully resetting, the firmware will stand by, ready to read instructions. Motors do not spin up automatically upon start.