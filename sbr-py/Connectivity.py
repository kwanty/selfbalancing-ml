# -*- coding: utf-8 -*-
#
# Description:  connectivity class: WIFI/BT/UART connection between host and robot
# Author:       Jaroslaw Bulat (kwant@agh.edu.pl, kwanty@gmail.com)
# Created:      30.01.2021
# License:      GPLv3
# File:         Connectivity.py

import serial
import crc8

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
            print('package!')
            byte_frame = self.received_bytes.copy()
            self.received_bytes.clear()
            return byte_frame

        self.received_bytes.clear()     # broken frame, clear it
        return None

    def decode_frame(self, byte_frame):
        """
        Decode frame from list of bytes to json
        :param byte_frame: list of bytes with entire frame
        :return: json packed frame
        """

        empty_result = {'type': None}
        if byte_frame[0] == b'\x35':    # MPU package
            if len(byte_frame) != 28:
                return empty_result
            # hash = crc8.crc8()
            # hash.update(byte_frame[1:-3])
            # if hash.digest() != byte_frame[-3]:
            #     return empty_result

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
