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
* \file SerialFrame.h
* \brief Simple data frames handling
* \author Piotr Wilkon <pwilkon@student.agh.edu.pl>
* \copyright Copyright 2021 Piotr Wilkon, licensed under GNU GPLv3
**/


#ifndef SERIALFRAME_H_
#define SERIALFRAME_H_
#include <stdint.h>
#include "SBRCP.h"
#include <Arduino.h>

#define _RAW_DATA_BUFFER_SIZE 60 //raw data buffer size in bytes

typedef enum
{
	wifi,
	bluetooth,
} SerialFrame_type;

class SerialFrame
{
private:
	uint8_t data[_RAW_DATA_BUFFER_SIZE]; //raw received data buffer
	uint16_t len; //buffer length
	
	void (*parsedFrameCallback)(uint8_t*, uint16_t); //callback function for received frames
	
public:
	/**
	* \brief Library initializer
	* \param[in] *callback Pointer to a callback function that handles received frames
	**/
	SerialFrame(void (*callback)(uint8_t*, uint16_t));
	/**
	* \brief Parses raw incoming data from Serial object
	* \attention Must be executed in the main loop
	**/
	void parseRawData(SerialFrame_type type);
};
#endif