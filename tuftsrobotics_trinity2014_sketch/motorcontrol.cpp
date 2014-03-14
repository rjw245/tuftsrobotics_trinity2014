#include "motorcontrol.h"

MotorControl::MotorControl(int cType){
	motor1 = motor2 = NULL;
        controlType = cType;
}

void MotorControl::attach(int dig1, int pwm1, int dig2, int pwm2){
	motor1 = new Motor();
	motor1->attach(dig1,pwm1);
	motor2 = new Motor();
	motor2->attach(dig2,pwm2);
}

void MotorControl::attach(Motor* mot1, Motor* mot2){
	motor1 = mot1;
	motor2 = mot2;
}

void MotorControl::flip(){
	flip1();
	flip2();
}

void MotorControl::flip1(){
	if(motor1!=NULL) motor1->flip();
}

void MotorControl::flip2(){
	if(motor2!=NULL) motor2->flip();
}

void MotorControl::swapMotors(){
  Motor* temp = motor1;
  motor1 = motor2;
  motor2 = temp;
}

void MotorControl::drive(int p_left, int p_right, int inertia){
  if(controlType == PROPORTIONAL){
    int leftSpeed  = KP * (p_left - OPT)  + inertia;
    int rightSpeed = KP * (p_right - OPT) + inertia;
    Serial.println(rightSpeed);
    motor1->drive(leftSpeed);
    motor2->drive(rightSpeed);
    
  }
  else if(controlType == DERIVATIVE){
    
  }
  else if(controlType == INTEGRAL){
    
  }
}

void MotorControl::spin(int spd){
	if(motor1==NULL || motor2==NULL) return;
	motor1->drive(spd);
	motor2->drive(-spd);
}

void MotorControl::brake(){
	motor1->drive(0);
	motor2->drive(0);
}
