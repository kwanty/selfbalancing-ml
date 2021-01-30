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
* \file SBRCP.cpp
* \brief Self balancing robot communication protocol
* \author Piotr Wilkon <pwilkon@student.agh.edu.pl>
* \copyright Copyright 2021 Piotr Wilkon, licensed under GNU GPLv3
**/


#include "SBRCP.h"


SBRCP::SBRCP(void (*callback)(SBRCP_data_t*))
{
	processedDataCallback = callback;
}

void SBRCP::parseRx(uint8_t *data, uint16_t len)
{
	if(len < 3) 
		return; //valid data must contain: a type byte, at least one data byte, a CRC
	if(len > (_SBRCP_MAX_PAYLOAD_SIZE + 2)) 
		return; //if frame is too long (longer than max. payload size + type byte + crc byte), drop it
	SBRCP_data_t d; //data structure
	d.type = *(data); //save data type
	uint8_t crc = crc8(CRC8_INITIAL_VAL, d.type); //initialize crc and recalculate for the first data byte
	
	for(uint16_t i = 1; i < (len - 3); i++)
	{
		d.payload[i - 1] = *(data + i); //copy all payload data
		crc = crc8(crc, d.payload[i - 1]); //and recalculate crc
		d.size++;
	}
	
	if(crc != *(data + len - 3)) //check if crc matches
		return; //if not, abort
	else
		(*processedDataCallback)(&d); //if so, call callback function
	
}

void SBRCP::parseTx(SBRCP_data_t *data, uint8_t *buf, uint8_t *len)
{
	*len = 0;
	*(buf) = data->type;
	(*len)++;
	uint8_t crc = crc8(CRC8_INITIAL_VAL, data->type);
	for(uint16_t i = 0; i < data->size; i++)
	{
		*(buf + i + 1) = data->payload[i]; //copy all payload data
		crc = crc8(crc, data->payload[i]); //calculate CRC for data
		(*len)++;
	}
	*(buf + data->size + 1) = crc;
	*(buf + data->size + 2) = '\n';
	*(buf + data->size + 3) = '\r';
	(*len) += 3;
}

uint8_t SBRCP::crc8(uint8_t lastCRC, uint8_t data)
{
	uint8_t crc = lastCRC ^ data;
	for(uint8_t i = 0; i < 8; i++)
	{
		if((crc & 0x80) != 0)
			crc = ((uint8_t)(crc << 1) ^ CRC8_POLYNOMIAL);
		else
			crc <<= 1;
	}
	return crc;
}