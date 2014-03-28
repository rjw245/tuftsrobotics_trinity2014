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
#define fireSense1            -1
#define fireSense2            -1
#define fireSense3            -1
#define fireSense4            -1
#define fireSense5            -1

//MOTOR 2 IS ON THE RIGHT FOR DELUX
#define motor1dig             4
#define motor1pwm             5
#define motor2dig             7
#define motor2pwm             6

//Possible States
#define INITIALIZATION        0
#define WALLFOLLOW            1
#define INROOM                2
#define FOUNDFIRE             3
#define RETURNHOME            4

//Misc constants
#define FRONTOBSTACLEDIST     300
#define LINESENSED            700
#define FIRESENSED            50

MotorControl mcontrol(PROPORTIONAL);

Motor motor1;
Motor motor2;

int STATE = INITIALIZATION;

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
  switch(STATE){
    case INITIALIZATION:
      rotCW90();
      mcontrol.drive(analogRead(distRightBackPin),analogRead(distRightFrontPin),90);
      if(analogRead(distFrontPin)>FRONTOBSTACLEDIST){
        rotCCW90();
      }
      if(millis()>=3000){
        STATE = WALLFOLLOW;
      }
      
      break;
      
    case WALLFOLLOW:
      //Look for front obstacles:
      if(analogRead(distFrontPin)>FRONTOBSTACLEDIST){
        rotCCW90();
      }
      mcontrol.drive(analogRead(distRightBackPin),analogRead(distRightFrontPin),90);
      
      //Look for lines
      if(analogRead(lineLeftPin)>LINESENSED || analogRead(lineRightPin)>LINESENSED){
        STATE = INROOM;
      }
      
      break;
      
    case INROOM:
      //IF fire sensed
      if(analogRead(fireSense1)>FIRESENSED
      || analogRead(fireSense2)>FIRESENSED
      || analogRead(fireSense3)>FIRESENSED
      || analogRead(fireSense4)>FIRESENSED
      || analogRead(fireSense5)>FIRESENSED){
        STATE = FOUNDFIRE;
      }
      else{
        //BACK OUT OF ROOM
      }
      
      break;
      
    case FOUNDFIRE:
      //ROTATE UNTIL FIRE IS DIRECTLY AHEAD
      //DRIVE FORWARD UNTIL FIRE READS SIGNIFICANTLY HIGH
      //TRIGGER CO2
      //STATE = RETURNHOME;
      
      break;
      
    case RETURNHOME:
      
      
      break;
  }
  
  if(millis()%1000==0){
    sensorDiagnostics();
  }
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

void rotCW90(){
  motor1.drive(200);
  motor2.drive(-200);
  delay(400);
}

void rotCCW90(){
  motor1.drive(-200);
  motor2.drive(200);
  delay(400);
}
