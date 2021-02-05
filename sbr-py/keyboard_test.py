# -*- coding: utf-8 -*-
#
# Description:  communicate with robot (Arduino) read and send command
# Author:       Jaroslaw Bulat (kwant@agh.edu.pl, kwanty@gmail.com)
# Created:      29.01.2021
# License:      GPLv3
# File:         keyboard_test.py
# TODO:
#   -

import sys
from getkey import getkey, keys
from Connectivity import Connectivity

# connectivity setup
uart_port = 'ttyUSB0'               # in case of UART connectivity
uart_speed = 115200                 # serial port speed
wifi_local_ip = '192.168.4.1'       # host (this) IP
wifi_robot_ip = '192.158.4.2'       # remote (robot) IP
wifi_local_port = '1234'            # host (this) port
wifi_robot_port = '1235'            # remote (robot) port


def main_loop(connectivity):
    """
    Main loop sending/receiving data.
    When obtain correct frame, print acc/gyro parameters,
    When read keyboard <left, right, up, down arrow> send command to robot
    :param connectivity: Connectivity object
    """

    # TODO: primitive, inefficient pooling-type bidirectional communication
    left_wheel_speed = 0
    right_wheel_speed = 0
    while True:
        # print('.')
        # print MPU readings
        msg = connectivity.read()
        if msg['type'] is None:     # ignore None msg
            pass
        elif msg['type'] == 'MPUrate':
            print('acc: {:.2f} {:.2f} {:.2f}, gyro:  {:.2f} {:.2f} {:.2f}\n'
            .format(msg['acc_x'], msg['acc_y'], msg['acc_z'], msg['gyro_x'], msg['gyro_y'], msg['gyro_z']))
        else:
            print('Unsupported message from robot, type: {}'.format(msg['type']))

        # read keyboard and setup motors
        key = getkey(blocking=False)
        if key != '':
            if key == keys.LEFT:
                left_wheel_speed = max(-255, left_wheel_speed-5)
            if key == keys.RIGHT:
                left_wheel_speed = min(255, left_wheel_speed+5)
            if key == keys.UP:
                right_wheel_speed = min(255, right_wheel_speed+5)
            if key == keys.DOWN:
                right_wheel_speed = max(-255, right_wheel_speed-5)
            print('Wheel speed: Left {}, Right {}'.format(left_wheel_speed, right_wheel_speed))

            message = {'type': 'SetMotors', 'left': left_wheel_speed, 'right': right_wheel_speed}
            connectivity.write(message)

            if key == keys.ESC:
                print('STOP and EXIT')
                connectivity.write({'type': 'SetMotors', 'left': 0, 'right': 0})
                break


if __name__ == "__main__":
    # script argument could be 'WIFI', 'BT', 'UART' (default with ttyUSB0)
    try:
        connectivity = sys.argv[1]
    except IndexError:
        connectivity = 'UART'

    if connectivity == 'WIFI':
        parameters = {'local_ip': wifi_local_ip, 'robot_ip': wifi_robot_ip, 'local_port': wifi_robot_port, 'robot_port': wifi_robot_port}
    elif connectivity == 'BT':
        parameters = {'TBD'}
    elif connectivity == 'UART':
        parameters = {'port': uart_port, 'speed': uart_speed, 'timeout': 0.01}
    else:
        assert False, 'unsupported connectivity method: {}'.format(connectivity)

    con = Connectivity(connectivity, parameters)
    con.write({'type': 'MPUrate', 'rate': 5000})
    main_loop(con)
