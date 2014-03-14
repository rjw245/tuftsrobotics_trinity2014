#include "motor.h"
#include "motorcontrol.h"

//Set 0 if this bot is "Irrelephant"
#define IS_DELUX              1

//DEFINE ALL PINS HERE
#define lineLeftPin           -1
#define lineRightPin          -1
#define distRightBackPin      A4
#define distRightFrontPin     A5
#define distFrontPin          -1
#define distLeftBackPin       A2
#define distLeftFrontPin      A3


#define motor1dig             4
#define motor1pwm             5
#define motor2dig             7
#define motor2pwm             6

MotorControl mcontrol(PROPORTIONAL);

Motor motor1;
Motor motor2;

void setup() {
  motor1.attach(motor1dig,motor1pwm);
  motor2.attach(motor2dig,motor2pwm);
  motor1.flip();
  
  mcontrol.attach(&motor1,&motor2);
  
  
  Serial.begin(9600);
  pinMode(lineLeftPin,INPUT);
  pinMode(lineRightPin,INPUT);
  pinMode(distRightBackPin,INPUT);
  pinMode(distRightFrontPin,INPUT);
  pinMode(distFrontPin,INPUT);
  pinMode(distLeftBackPin,INPUT);
  pinMode(distLeftFrontPin,INPUT);
  
  pinMode(motor1dig,OUTPUT);
  pinMode(motor1pwm,OUTPUT);
  pinMode(motor2dig,OUTPUT);
  pinMode(motor2pwm,OUTPUT);
  

}

void loop() {
  mcontrol.drive(analogRead(distRightBackPin),analogRead(distRightFrontPin),90);
  if(millis()%1000==0){
    //sensorDiagnostics();
  }
  // put your main code here, to run repeatedly:

}

void sensorDiagnostics(){
  Serial.println("------------Printing robot information------------");
  
  Serial.print("ROBOT MONICKER:                 ");
  if(IS_DELUX){
    Serial.println("Delux");
  } else {
    Serial.println("Irrelephant");
  }
  
  Serial.print("LINE SENSOR - Left:             ");
  Serial.println(analogRead(lineLeftPin));
  
  Serial.print("LINE SENSOR - Right:            ");
  Serial.println(analogRead(lineRightPin));
  
  Serial.print("DISTANCE SENSOR - Left-Back:    ");
  Serial.println(analogRead(distLeftBackPin));
  
  Serial.print("DISTANCE SENSOR - Left-Front:   ");
  Serial.println(analogRead(distLeftFrontPin));
  
  Serial.print("DISTANCE SENSOR - Right-Back:   ");
  Serial.println(analogRead(distRightBackPin));
  
  Serial.print("DISTANCE SENSOR - Right-Front:  ");
  Serial.println(analogRead(distRightFrontPin));
  
  Serial.print("DISTANCE SENSOR - Front:        ");
  Serial.println(analogRead(distFrontPin));
  
  Serial.println();
}
