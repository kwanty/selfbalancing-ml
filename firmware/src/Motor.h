/**
* \file Motor.h
* \brief TB6612 motor controller library
* \author Piotr Wilkon <pwilkon@student.agh.edu.pl>
* \copyright GNU GPLv3
**/

#ifndef MOTOR_H_
#define MOTOR_H_
#include <stdint.h>
#include "Arduino.h"

class Motor
{
private:
	uint8_t in1, in2, pwm; //pin numbers
	
public:
	/**
	* \brief Library initializer
	* \param[in] in1 TB6612 xIN1 pin
	* \param[in] in2 TB6612 xIN2 pin
	* \param[in] pwm TB6612 PWMx pin
	**/
	Motor(uint8_t in1, uint8_t in2, uint8_t pwm);
	
	/**
	* \brief Sets motor speed
	* \param[in] speed Speed in range 1 to 255 for one direction or -255 to -1 for another. 0 to stop the motor.
	*/
	void set(int16_t speed);
};
#endif