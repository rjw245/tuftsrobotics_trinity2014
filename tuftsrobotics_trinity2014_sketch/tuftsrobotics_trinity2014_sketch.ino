#include "motor.h"
#include "motorpair.h"

//DEFINE ALL PINS HERE
#define lineLeftPin   A14
#define lineRightPin  A13
#define distRight1Pin A12
#define distRight2Pin A11
#define distFrontPin  A10
#define distLeft2Pin  A9
#define distLeft1Pin  A8

#define motor1dig 8
#define motor1pwm 9

MotorPair mpair;

void setup() {
  // put your setup code here, to run once:
  mpair.attach(8,9,10,11);
  Serial.begin(9600);

}

void loop() {
    mpair.drive(155,0);
  // put your main code here, to run repeatedly:

}
