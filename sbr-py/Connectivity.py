# -*- coding: utf-8 -*-
#
# Description:  connectivity class: WIFI/BT/UART connection between host and robot
# Author:       Jaroslaw Bulat (kwant@agh.edu.pl, kwanty@gmail.com)
# Created:      30.01.2021
# License:      GPLv3
# File:         Connectivity.py

import crc8
import struct
import serial

class Connectivity:
    def __init__(self, connection_type, parameters):
        """
        WIFI/BT/UART connection between host and robot
        :param connection_type: type of connection 'WIFI'/'BT/'UART'
        :param parameters: dictionary with parameters for each connection type
                            WIFI: {'local_ip': ..., 'local_port': ..., 'robot_ip': ..., 'robot_port': ...}
                            BT: TBD
                            UART: {'port': ..., 'speed' ..., 'timeout' ...}
        TODO: WIFI/BT timeout ???
        """
        self.serial = None
        self.wifi = None
        self.bt = None
        self.received_bytes = []

        self.connection = connection_type.upper()
        if self.connection == 'WIFI':
            assert False, 'WIFI connectivity not yet supported'
        elif self.connection == 'BT':
            assert False, 'BT connectivity not yet supported'
        elif self.connection == 'UART':
            # TODO manage SerialException and add auto port detection (eg. typical ttyUSB0, ttyUSB1, ttyACM0, etc...)
            self.serial = serial.Serial(
                port='/dev/'+parameters['port'],
                baudrate=parameters['speed'],
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS,
                timeout=parameters['timeout']
            )
            # https://pyserial.readthedocs.io/en/latest/pyserial_api.html
        else:
            assert False, 'connectivity method: {} not supported'.format(connection_type)

    def extract_frame(self):
        """
        Extract frame from the stream. Search for beginning tag and end tags. Do not interpret
        :return: None if incomplete/broken frame, byte frame if complete.
        """
        if len(self.received_bytes) < 2:
            return None     # frame to short
        if self.received_bytes[-1] != b'\r' or self.received_bytes[-2] != b'\n':
            return None     # there no end of the frame

        # beginning of the frame: 0x35 (MPU frame), 0xEE (correct frame with error code)
        if self.received_bytes[0] in [b'\x35', b'\xEE']:
            # print('package!')
            byte_frame = self.received_bytes.copy()
            self.received_bytes.clear()
            return byte_frame

        self.received_bytes.clear()     # broken frame, clear it
        # print('Broken frame!')
        return None

    def decode_frame(self, byte_frame):
        """
        Decode frame from list of bytes to json
        :param byte_frame: list of bytes with entire frame
        :return: json packed frame
        """
        empty_result = {'type': None}
        if byte_frame[0] == b'\x35':                    # MPU package
            if len(byte_frame) != 28:                   # wrong message length
                # print('!=28')
                return empty_result
            # print(byte_frame)
            hash = crc8.crc8(initial_start=0xFF)        # non standard init value        
            [hash.update(b) for b in byte_frame[0:-3]]  # CRC8 with beginning frame, without CRC and ending tags
            if hash.digest() != byte_frame[-3]:         # corrupted frame
                return empty_result
            # print('CRC8 OK!')
            acc_x = struct.unpack_from('<f', b''.join(byte_frame[1:5]))[0]
            acc_y = struct.unpack('<f', b''.join(byte_frame[5:9]))[0]
            acc_z = struct.unpack('<f', b''.join(byte_frame[9:13]))[0]
            gyro_x = struct.unpack('<f', b''.join(byte_frame[13:17]))[0]
            gyro_y = struct.unpack('<f', b''.join(byte_frame[17:21]))[0]
            gyro_z = struct.unpack('<f', b''.join(byte_frame[21:25]))[0]
            return {'type': 'MPU', 'acc_x': acc_x, 'acc_y': acc_y, 'acc_z': acc_z, 'gyro_x': gyro_x, 'gyro_y': gyro_y,'gyro_z': gyro_z}
        return empty_result

    def read(self):
        """
        Read and decode message, could be 'timeout' message if not received
        :return: dictionary with type and payload: {'type': ..., 'payload': ...}
                 'type': 'MPU" - payload from MPU sensors
        """
        message = {'type': None}   # not ready

        if self.connection == 'UART':
            rx_data = self.serial.read(size=1)  # read byte by byte
            if rx_data:
                self.received_bytes.append(rx_data)
                frame = self.extract_frame()
                if frame:
                    message = self.decode_frame(frame)
                    return message
        return message

    def write(self, payload):
        """
        Write (send) data to robot
        :param payload: format: {'type', 'payload'}
                        MPU reading rate: type == 'MPUrate', payload == number of ms between reading
                        Motors speed: type =='SetMotors', payload == {'left': ..., 'right': ...} speed +-255
        """
        pass
