/*
    This file is part of the Self Balancing Robot Platform (SBR).

    SBR is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    SBR is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SBR.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
* \file soft.ino
* \brief Self Balancing Robot Platform main Arduino file
* \author Piotr Wilkon <pwilkon@student.agh.edu.pl>
* \copyright Copyright 2021 Piotr Wilkon, licensed under GNU GPLv3
**/

//to compile this project, you need to install Adafruit MPU6050 library with required dependencies
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "Motor.h"
#include "SBRCP.h"
#include "SerialFrame.h"
#include "ESP_AT.h"

#define _DATA_INTERVAL_US 5000000 //inital MPU data rate in microseconds (at least 5000). Can be changed by a command.


//#define _CONNECTION_WIFI //connection using ESP32 WiFi

#define _SSID "sbr" //WiFi AP SSID
#define _PASS "pass123456789" //AP password, at least 8 characters
#define _DEST_IP "192.168.4.2" //destination IP. Source IP is always 192.168.4.1
#define _DEST_PORT "1234" //destination port
#define _SRC_PORT "1235" //source port

//Normally, setting up the speed as a positive or negative value rotates the motor clockwise or counterclockwise respectively.
//if both motors have their speeds set to a value with the same sign, they both rotate in the same direction - BUT - because they are located on the opposite sides of the robot
//one motor "moves" forward and the other "moves" backward (the robot will spin around)
//this behavior can be inverted by uncommenting the following line
//#define _INVERT_ROTATION
//then setting both speeds to a value with the same sign will result in motors rotating in opposite directions, but the robot will move forward/backward

//pin definitions for motor controller IC
#define AIN1 13
#define AIN2 12
#define BIN1 8
#define BIN2 7
#define PWMA 6
#define PWMB 3

//serial protocol error definitions
#define ERROR_OTHER 0x00
#define ERROR_MPU_INIT 0x01
#define ERROR_MPU_READ 0x02
#define ERROR_ILLEGAL_CMD 0x03



#if (_DATA_INTERVAL_US < 5000)
#error MPU rate must be at least 5000us
#endif


Adafruit_MPU6050 mpu; //MPU6050 object
uint32_t nextDataTimerTick = 0; //software timer counter for data
uint32_t dataTimerInterval = _DATA_INTERVAL_US;


void parseRxData(SBRCP_data_t *data);
void parseRxFrame(uint8_t *, uint16_t);


Motor *motorA, *motorB;
SBRCP protocol(&parseRxData);
SerialFrame frameHandler(&parseRxFrame);
ESP_AT esp;

/**
 * \brief Reads MPU6050 data, converts it and sends to a PC
 */
void readMPUdata(void)
{
  uint8_t buf[32] = {0}; //frame buffer
  uint8_t len = 0;
  SBRCP_data_t t; //packet structure
  
  sensors_event_t a, g, temp; //special structures for mpu data
  if(mpu.getEvent(&a, &g, &temp) != true) //read data
  {
    t.type = DATA_ERROR; //if read failed
    t.payload[0] = ERROR_MPU_READ;
    t.size = 1;
    protocol.parseTx(&t, buf, &len);
#ifdef _CONNECTION_WIFI
    esp.send(buf, len); //send error packet
#else
    Serial.write(buf, len);
#endif
    return;
  }

  t.type = DATA_MPU; //data type

  uint32_t tmp = 0; //temporary variable for type conversion
  memcpy(&tmp, &(a.acceleration.x), 4); //copy data to uint32_t type variable address to make bit manipulation possible
  t.payload[0] = tmp & 0xFF; //copy data to the buffer
  t.payload[1] = (tmp & 0xFF00) >> 8;
  t.payload[2] = (tmp & 0xFF0000) >> 16;
  t.payload[3] = (tmp & 0xFF000000) >> 24;
  
  memcpy(&tmp, &(a.acceleration.y), 4);
  t.payload[4] = tmp & 0xFF;
  t.payload[5] = (tmp & 0xFF00) >> 8;
  t.payload[6] = (tmp & 0xFF0000) >> 16;
  t.payload[7] = (tmp & 0xFF000000) >> 24;

  memcpy(&tmp, &(a.acceleration.z), 4);
  t.payload[8] = tmp & 0xFF;
  t.payload[9] = (tmp & 0xFF00) >> 8;
  t.payload[10] = (tmp & 0xFF0000) >> 16;
  t.payload[11] = (tmp & 0xFF000000) >> 24;

  memcpy(&tmp, &(g.gyro.x), 4);
  t.payload[12] = tmp & 0xFF;
  t.payload[13] = (tmp & 0xFF00) >> 8;
  t.payload[14] = (tmp & 0xFF0000) >> 16;
  t.payload[15] = (tmp & 0xFF000000) >> 24;

  memcpy(&tmp, &(g.gyro.y), 4);
  t.payload[16] = tmp & 0xFF;
  t.payload[17] = (tmp & 0xFF00) >> 8;
  t.payload[18] = (tmp & 0xFF0000) >> 16;
  t.payload[19] = (tmp & 0xFF000000) >> 24;

  memcpy(&tmp, &(g.gyro.z), 4);
  t.payload[20] = tmp & 0xFF;
  t.payload[21] = (tmp & 0xFF00) >> 8;
  t.payload[22] = (tmp & 0xFF0000) >> 16;
  t.payload[23] = (tmp & 0xFF000000) >> 24;

  t.size = 24;

  protocol.parseTx(&t, buf, &len);
#ifdef _CONNECTION_WIFI
  esp.send(buf, len); //send packet
#else
  Serial.write(buf, len);
#endif
}

//callback function for parsed packets
void parseRxData(SBRCP_data_t *data)
{
    //check for command type
    if(data->type == DATA_CMD_RATE) //command for setting MPU rate
    {
      uint32_t val = data->payload[0]; //read 32-bit value
      val |= ((uint32_t)data->payload[1] << 8);
      val |= ((uint32_t)data->payload[2] << 16);
      val |= ((uint32_t)data->payload[3] << 24);

      if(val < 5000) //the rate must be at least 5000 usec
      {
        val = 5000;
      }
      dataTimerInterval = val;
    }
    else if(data->type == DATA_CMD_MOTORS) //setting motors' speeds
    {
      int16_t val1 = data->payload[0]; //read 16-bit values
      val1 |= (data->payload[1] << 8);
      int16_t val2 = data->payload[2];
      val2 |= (data->payload[3] << 8);
      motorA->set(val1);
      motorB->set(val2);
    }
}

//wrapper function to pass processed received frame to a protocol parser
void parseRxFrame(uint8_t *data, uint16_t len)
{
  protocol.parseRx(data, len);
}

void setup()
{
  
  
  motorA = new Motor(AIN1, AIN2, PWMA);

#ifdef _INVERT_ROTATION //rotation inversion, explained at the top of this file
  motorB = new Motor(BIN1, BIN2, PWMB);
#else
  motorB = new Motor(BIN2, BIN1, PWMB);
#endif

#ifdef _CONNECTION_WIFI
  Serial.begin(250000); 
  delay(3000);
  esp.init(_SSID, _PASS, _DEST_IP, _DEST_PORT, _SRC_PORT);
#else
  Serial.begin(115200); 
#endif

  if (!mpu.begin()) //try to initialize MPU6050
  {
    uint8_t buf[5] = {0};
    uint8_t len = 0;
    SBRCP_data_t t;
    t.type = DATA_ERROR;
    t.payload[0] = ERROR_MPU_INIT;
    t.size = 1;
    protocol.parseTx(&t, buf, &len);
#ifdef _CONNECTION_WIFI    
    esp.send(buf, len); //send error packet
#else
  Serial.write(buf, len);
#endif
    while (1);;
  }
  
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G); //set accelerometer range. Possible values are 2, 4, 8 and 16 G
  mpu.setGyroRange(MPU6050_RANGE_500_DEG); //set gyroscope range (250, 500, 1000 or 2000 deg)
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); //set filter bandwidth (5, 10, 21, 44, 94, 184 or 260 Hz)
}


void loop() 
{
  //simple software timer handling
  //the micros() timer overflows every ~70 minutes. We don't really need to worry about that, because nextTimerTick has the same type as micros() counter 
  //and they will overflow and wrap around 0 identically, so everything should be "in phase" all the time.
  if(micros() >= nextDataTimerTick)
  {
    nextDataTimerTick = micros() + dataTimerInterval; //set new counter value
    readMPUdata(); //then read and send MPU data
  }
#ifdef _CONNECTION_WIFI
  frameHandler.parseRawData(wifi); //process uart data
#else
  frameHandler.parseRawData(bluetooth);
#endif
}
