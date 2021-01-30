/**
* \file Motor.cpp
* \brief TB6612 motor controller library
* \author Piotr Wilkon <pwilkon@student.agh.edu.pl>
* \copyright GNU GPLv3
**/

#include "Motor.h"

Motor::Motor(uint8_t in1, uint8_t in2, uint8_t pwm)
{
	this->in1 = in1; //initialize variables
	this->in2 = in2;
	this->pwm = pwm;
	pinMode(this->in1, OUTPUT); //initialize outputs
	pinMode(this->in2, OUTPUT);
	pinMode(this->pwm, OUTPUT);
	
	set(0); //put motor into stopped state
}

void Motor::set(int16_t speed)
{
	if(speed != 0)
	{
		if(speed > 0) //clockwise rotation
		{
			if(speed > 255) speed = 255; //trim the value
			digitalWrite(in1, HIGH);
			digitalWrite(in2, LOW);
			analogWrite(pwm, (uint8_t)speed);
		}
		else //counterclockwise rotation
		{
			if(speed < -255) speed = -255; //trim the value
			digitalWrite(in1, LOW);
			digitalWrite(in2, HIGH);
			analogWrite(pwm, (uint8_t)(-speed));
		}
	}
	else //stop motor
	{
		digitalWrite(in1, HIGH); //perform short brake
		digitalWrite(in2, HIGH);
		analogWrite(pwm, 0);
	}
}