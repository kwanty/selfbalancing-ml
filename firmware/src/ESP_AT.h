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
* \file ESP_AT.h
* \brief ESP32 AT commands library
* \author Piotr Wilkon <pwilkon@student.agh.edu.pl>
* \copyright Copyright 2021 Piotr Wilkon, licensed under GNU GPLv3
**/
#ifndef ESP_AT_H_
#define ESP_AT_H_
#include <stdint.h>
#include <Arduino.h>

class ESP_AT
{
	
public:
	ESP_AT();
	/**
	* \brief Initializes ESP in WiFi AP mode
	* \param ssid Network SSID
	* \param pass Network password
	* \param dstIP Destination IP (as string)
	* \param dstPort Destination port (as string)
	* \param srcPort Source port (as string)
	**/
	void init(String ssid, String pass, String dstIP, String dstPort, String srcPort);
	/**
	* \brief Sends data using WiFi
	* \param *data Pointer to data
	* \param len Data length
	**/
	void send(uint8_t *data, uint16_t len);
};



#endif