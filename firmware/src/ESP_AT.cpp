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
* \file ESP_AT.cpp
* \brief ESP32 AT commands library
* \author Piotr Wilkon <pwilkon@student.agh.edu.pl>
* \copyright Copyright 2021 Piotr Wilkon, licensed under GNU GPLv3
**/
#include "ESP_AT.h"

void ESP_AT::init(String ssid, String pass, String dstIP, String dstPort, String srcPort)
{
	Serial.println("AT+CIPCLOSE"); //close existing connections
	delay(30);
	Serial.println("AT+CWMODE=2"); //softAP mode
	delay(30);
	Serial.println("AT+CIPMUX=0"); //single connection mode
	delay(30);
	Serial.println("AT+CWDHCP=1,1"); //enable dhcp
	delay(30);
	Serial.println("AT+CIPDINFO=0"); //disable reading remote ip and port
	delay(30);
	Serial.print("AT+CWSAP=\""); //AP configuration
	Serial.print(ssid);
	Serial.print("\",\"");
	Serial.print(pass);
	Serial.println("\",5,3");
	delay(300);
	Serial.print("AT+CIPSTART=\"UDP\",\"");
	Serial.print(dstIP);
	Serial.print("\",");
	Serial.print(dstPort);
	Serial.print(",");
	Serial.println(srcPort);
	delay(300);
}

void ESP_AT::send(uint8_t *data, uint16_t len)
{
	if(len == 0) 
		return;

	Serial.print("AT+CIPSEND="); //send data command
	Serial.println(len, DEC); //append data length
	delay(1); //unfortunately it's neccesary, otherwise ESP32 won't accept data and everything will go out of sync
	Serial.write(data, len);
	Serial.println(""); //this adds additional CR-LF, which is NOT sent, but keeps the data communication "in sync"
							//if there was an UART communcation problem and not enough bytes were received by ESP32 and it was still waiting for data
}


ESP_AT::ESP_AT()
{
	
}