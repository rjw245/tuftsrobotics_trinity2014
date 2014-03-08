#include "motor.h"

Motor::Motor(){
	digPin = -1;
	pwmPin = -1;
	flipped = false;
}

void Motor::attach(int dig, int pwm){
	digPin = dig;
	pwmPin = pwm;
	pinMode(digPin,OUTPUT);
	pinMode(pwmPin,OUTPUT);
	digitalWrite(digPin,LOW);
	digitalWrite(pwmPin,LOW);
}
void Motor::flip(){
	flipped = !flipped;
}

void Motor::drive(int speed){
	if(speed>255)  speed=255;
	if(speed<-255) speed=-255;
	int abs_speed = abs(speed);
	if(!flipped){
		if(speed>=0){
			digitalWrite(digPin,LOW);
			analogWrite(pwmPin,abs_speed);
		} else {
			digitalWrite(digPin,HIGH);
			analogWrite(pwmPin,255-abs_speed);
		}
	} else {
		if(speed>=0){
			digitalWrite(digPin,HIGH);
			analogWrite(pwmPin,255-abs_speed);
		} else {
			digitalWrite(digPin,LOW);
			analogWrite(pwmPin,abs_speed);
		}
	}
}

void Motor::brake(){
	drive(0);
}