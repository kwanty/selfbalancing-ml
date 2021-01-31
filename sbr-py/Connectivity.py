# -*- coding: utf-8 -*-
#
# Description:  connectivity class: WIFI/BT/UART connection between host and robot
# Author:       Jaroslaw Bulat (kwant@agh.edu.pl, kwanty@gmail.com)
# Created:      30.01.2021
# License:      GPLv3
# File:         Connectivity.py

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

    def read(self):
        """
        Read and decode message, could be 'timeout' message if not received
        :return: dictionary with type and payload: {'type': ..., 'payload': ...}
                 'type': 'MPU" - payload from MPU sensors
        """
        if self.connection == 'UART':
            rx_data = self.serial.read(size=1)  # read byte by byte
            if rx_data:
                print(len(rx_data))
                print(rx_data)
                print(' ')

        return {'type': None}   # not ready

    def write(self, payload):
        """
        Write (send) data to robot
        :param payload: format: {'type', 'payload'}
                        MPU reading rate: type == 'MPUrate', payload == number of ms between reading
                        Motors speed: type =='SetMotors', payload == {'left': ..., 'right': ...} speed +-255
        """
        pass
