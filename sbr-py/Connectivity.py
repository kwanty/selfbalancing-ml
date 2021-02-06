# -*- coding: utf-8 -*-
#
# Description:  connectivity class: WIFI/BT/UART connection between host and robot
# Author:       Jaroslaw Bulat (kwant@agh.edu.pl, kwanty@gmail.com)
# Created:      30.01.2021
# License:      GPLv3
# File:         Connectivity.py
# TODO: WIFI/BT connectivity
# TODO: test decode_frame() Error package
# TODO: better conversion byte->int than b'\n'[0]

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
        self.received_bytes = b''

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
        if self.received_bytes[-1] != b'\r'[0] or self.received_bytes[-2] != b'\n'[0]:
            return None     # there no end of the frame

        # beginning of the frame: 0x35 (MPU frame), 0xEE (correct frame with error code)
        if self.received_bytes[0] in [b'\x35'[0], b'\xEE'[0]]:
            byte_frame = self.received_bytes
            self.received_bytes = b''
            return byte_frame

        self.received_bytes = b''     # broken frame, clear it
        return None

    def crc8(self, byte_frame):
        """
        Check CRC8 parity byte_frame[-3] is CRC
        :param byte_frame: list of bytes with entire frame
        :return: crc8
        """
        hash = crc8.crc8(initial_start=0xFF)            # non standard init value
        hash.update(byte_frame)                         # CRC8 with beginning frame, without CRC and ending tags
        return hash.digest()

    def decode_frame(self, byte_frame):
        """
        Decode frame from list of bytes to json
        :param byte_frame: list of bytes with entire frame
        :return: json packed frame
        """
        empty_result = {'type': None}
        if byte_frame[0] == b'\x35'[0]:                 # MPU package
            if len(byte_frame) != 28:                   # wrong message length
                return empty_result
            if self.crc8(byte_frame[:-3])[0] != byte_frame[-3]:       # corrupted frame
                return empty_result
            acc_x = struct.unpack('<f', byte_frame[1:5])[0]
            acc_y = struct.unpack('<f', byte_frame[5:9])[0]
            acc_z = struct.unpack('<f', byte_frame[9:13])[0]
            gyro_x = struct.unpack('<f', byte_frame[13:17])[0]
            gyro_y = struct.unpack('<f', byte_frame[17:21])[0]
            gyro_z = struct.unpack('<f', byte_frame[21:25])[0]
            return {'type': 'MPUdata', 'acc_x': acc_x, 'acc_y': acc_y, 'acc_z': acc_z, 'gyro_x': gyro_x, 'gyro_y': gyro_y,'gyro_z': gyro_z}
        elif byte_frame[0] == b'\xEE'[0]:                  # package with error code
            if len(byte_frame) != 5:
                return empty_result
            if self.crc8(byte_frame[:-3])[0] != byte_frame[-3]:       # corrupted frame
                return empty_result
            error_code = 'UNKNOWN_ERROR'
            if byte_frame[1] == 0:
                error_code = 'ERROR_OTHER'
            elif error_code[1] == 1:
                error_code = 'ERROR_MPU_READ'
            elif error_code[1] == 2:
                error_code = 'ERROR_MPU_INIT'
            elif error_code[1] == 3:
                error_code = 'ERROR_ILLEGAL_CM'
            return {'type': 'ERROR', 'code': error_code}
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
                self.received_bytes += rx_data
                frame = self.extract_frame()
                if frame:
                    message = self.decode_frame(frame)
                    return message
        return message

    def write(self, payload):
        """
        Write (send) data to robot
        :param payload: format: {'type', 'payload'}
                        MPU reading rate: 'type': 'MPUrate', 'rate': number of ms between reading
                        Motors speed: type =='SetMotors', 'left': ..., 'right': ...: speed +-255
        """
        if payload['type'] == 'SetMotors':
            byte_frame = b'\x2F'
            byte_frame += struct.pack('>HH', payload['left'], payload['right'])
            byte_frame += self.crc8(byte_frame)     # add crc
            byte_frame += b'\r\n'
            print('byte frame: {}\n'.format(byte_frame))
        elif payload['type'] == 'MPUrate':
            print('payload:{}'.format(payload))
            print(type(payload['rate']))
            print(payload['rate'])
            print('\n\n')
            byte_frame = b'\xA7'
            byte_frame += struct.pack('>I', payload['rate'])
            byte_frame += self.crc8(byte_frame)     # add crc
            byte_frame += b'\r\n'

            print(byte_frame)
            print('\n\n')
            # hash = crc8.crc8(initial_start=0xFF)
            # [hash.update(b) for b in byte_frame]
            # byte_frame += hash.digest()
            # byte_frame += b'\r\n'
            print('MPUrate: {}\n'.format(byte_frame))
        else:
            assert False, 'Unsupported message PC->robot: {}'.format(payload['type'])
