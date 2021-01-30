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
* \file SBRCP.h
* \brief Self balancing robot communication protocol
* \author Piotr Wilkon <pwilkon@student.agh.edu.pl>
* \copyright Copyright 2021 Piotr Wilkon, licensed under GNU GPLv3
**/

#ifndef SBRCP_H_
#define SBRCP_H_
#include <stdint.h>
#define _SBRCP_MAX_PAYLOAD_SIZE (30) //maximum payload size in one packet (in bytes)

#define CRC8_INITIAL_VAL 0xFF
#define CRC8_POLYNOMIAL 0x07

//serial protocol data types
#define DATA_ERROR 0xEE
#define DATA_MPU 0x35
#define DATA_CMD_RATE 0xA7
#define DATA_CMD_MOTORS 0x2F

typedef struct
{
	uint8_t type; //data type (first byte of the packet)
	uint8_t payload[_SBRCP_MAX_PAYLOAD_SIZE]; //packet payload
	uint8_t size; //payload length
} SBRCP_data_t;

class SBRCP
{
private:
	void (*processedDataCallback)(SBRCP_data_t*); //callback function to be called when the packet is processed
	uint8_t crc8(uint8_t lastCRC, uint8_t data); //CRC8 calculation
public:
	/**
	* \brief SBRCP library initializer
	* \param[in] *callback Pointer to the function that should be called when a packet is received
	**/
	SBRCP(void (*callback)(SBRCP_data_t*));
	/**
	* \brief Parses incoming frame and calls the callback function
	* \param[in] *data Incoming frame
	* \param[in] len Incoming frame length
	**/
	void parseRx(uint8_t *data, uint16_t len);
	/**
	* \brief Parses outcoming data structure into frame
	* \param[in] *data Data structure to be processed
	* \param[out] *buf Frame buffer
	* \param[in] *len Frame buffer data length
	**/
	void parseTx(SBRCP_data_t *data, uint8_t *buf, uint8_t *len);
};
#endif