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
* \file SerialFrame.cpp
* \brief Simple data frames handling
* \author Piotr Wilkon <pwilkon@student.agh.edu.pl>
* \copyright Copyright 2021 Piotr Wilkon, licensed under GNU GPLv3
**/

#include "SerialFrame.h"


SerialFrame::SerialFrame(void (*callback)(uint8_t*, uint16_t))
{
	parsedFrameCallback = callback;
	len = 0;
}

void SerialFrame::parseRawData(SerialFrame_type type)
{
	uint8_t n = Serial.available(); //check for received data
	if(n == 0)
	{
		return;
	}
	for(uint8_t i = 0; i < n; i++)
	{
		data[len++] = Serial.read(); //save received data
		if(len == _RAW_DATA_BUFFER_SIZE) 
		{
			len = 0; //buffer overflow
		}
		if(type == bluetooth)
		{
			if((len > 3) && (data[len - 2] == '\n') && (data[len - 1] == '\r'))
			{
				for(uint8_t j = 0; j < len; j++) //look for frame type byte
				{
					if(data[j] == DATA_CMD_RATE || data[j] == DATA_CMD_MOTORS) //correct frames must begin with any of these bytes
					{
						(*parsedFrameCallback)(&data[j], len - j); //if found, call the callback function
					}
				}			
				//there can be more consecutive frames
				len = 0; //reset buffer index
				continue; //continue receiving data
			}			
			return;
		}
		
		if((len > 3) && (data[len - 2] == '\r') && (data[len - 1] == '\n')) //look for CR-LF, that is, the end of data sent by ESP32 to Arduino
		{

			if(data[0] != '\r' || data[1] != '\n' ||  data[2] != '+' || data[3] != 'I' || data[4] != 'P' || data[5] != 'D' || data[6] != ',')  //Received data (WiFi) must begin with "<CR><LF>+IPD,len:"
			{
				len = 0;
				return;
			}
			//data length is not really important, because there can be more than one packet, so just skip it
			//and look for the colon
			uint16_t i = 0;
			for(i = 8; i < len; i++) //"," is at position 7, there must be at least one digit for data length, so start from 8th position
			{
				if(data[i] == ':') //colon is found
				{
					i++; //go to the next position (probably the first byte of a packet)
					break;
				}
				if(i + 1 == len) //colon not found
				{
					len = 0;
					return;
				}
			}		
			for(; i < len; i++) //look for frames
			{
				if(data[i] == DATA_CMD_RATE || data[i] == DATA_CMD_MOTORS) //correct frames must begin with any of these bytes
				{
					for(uint16_t j = i + 1; j < len - 2; j++) //and must also end with LF-CR
					{
						if((data[j - 1] == '\n') && (data[j] == '\r')) //if true, j is the last byte of a frame
						{	
							(*parsedFrameCallback)(&data[i], j - i + 1); //if found, call the callback function
							i = j + 1; //i index to the first byte of next frame
						}
					}
				}
				//there can be more consecutive frames, so just look for the next starting byte
			}			
			len = 0; //reset the index if no more frames were found
		}
	}
	
}