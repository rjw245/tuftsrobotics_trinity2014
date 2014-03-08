#ifndef MOTOR_H
#define MOTOR_H
#include "Arduino.h"

class Motor{
    public:
        Motor();
        void attach(int dig, int pwm);
		void flip();
		void drive(int speed);
		void brake();
        
    private:
        int digPin;
        int pwmPin;
		boolean flipped;
};
#endif MOTOR_H
