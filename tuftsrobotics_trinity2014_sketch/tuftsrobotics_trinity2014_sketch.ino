#include "motor.h"

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

Motor motor1;

void setup() {
  // put your setup code here, to run once:
  motor1.attach(8,9);

}

void loop() {
  motor1.drive(200);
  motor1.flip();
  delay(1000);
  // put your main code here, to run repeatedly:

}
