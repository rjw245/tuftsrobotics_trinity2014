#include "motor.h"
#include "motorcontrol.h"

//Set 0 if this bot is "Irrelephant"
#define IS_DELUX              1

//DEFINE ALL PINS HERE
#define lineLeftPin           A0
#define lineRightPin          A1
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
#define LINE                  5
//Misc constants
#define FRONTOBSTACLEDIST     300
#define LINESENSED            500
#define FIRESENSED            50

//Motor controller object
MotorControl mcontrol(PROPORTIONAL);

//Motor objects
Motor motor1;
Motor motor2;

//Start state is INITIALIZATION
int STATE = INITIALIZATION;

void setup() {
  //Set up motors with proper pins
  motor1.attach(motor1dig,motor1pwm);
  motor2.attach(motor2dig,motor2pwm);
  
  //Get motor 1 in proper orientation
  motor1.flip();
  
  //Give these motors to the motor controller for wall following
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
  motor1.drive(200);
  motor2.drive(200);
  

}

//These declarations are for line adjustment
boolean rmotorForward = true;
boolean lmotorForward = true;
int lineLeft,lineRight;
boolean lineLeftSensed, lineRightSensed;
int rLineSide = 0, lLineSide = 0; //0 = behind line,
                                  //1 = in front of line,
                                  //2 = on line!
int mspeed = 200;
//END DECLARATIONS FOR LINE ADJUSTMENT

void loop() {
  switch(STATE){
    case INITIALIZATION:
      //Rotate 90 deg CW at start
      rotCW90();
      
      //Wall follow
      mcontrol.drive(analogRead(distRightBackPin),analogRead(distRightFrontPin),90);
      
      //Is there something in front of me? If so, rotate 90 CCW
      if(analogRead(distFrontPin)>FRONTOBSTACLEDIST){
        rotCCW90();
      }
      
      //After 3 seconds, initialization is over, so go to next state
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
      
      //Look for lines. If found, change state
      if(analogRead(lineLeftPin)<LINESENSED || analogRead(lineRightPin)<LINESENSED){
        STATE = ALIGNLINE;
      }
      
      break;
     
    
    case ALIGNLINE:
      lineLeft  = analogRead(lineLeftPin);
      lineRight = analogRead(lineRightPin);
      lineLeftSensed  = (lineLeft < LINESENSED);
      lineRightSensed = (lineRight < LINESENSED);
      /*
      Serial.print("Left line side: ");
      Serial.println(lLineSide);
      Serial.print("Right line side: ");
      Serial.println(rLineSide);
      */
      
      if(rLineSide == 2){
        rLineSide = rmotorForward;
      }
      if (lLineSide == 2){
        lLineSide = lmotorForward;
      }
      
      if (lineRightSensed){
        rLineSide = 2;
      }
      if (lineLeftSensed){
        lLineSide = 2;
      }
      
      if (rLineSide == 2 && lLineSide == 2){
        Serial.println("ALIGNED WITH LINE!!");
        delay(100);
        STATE = INROOM;
      }
      
      if(rLineSide==0){          //If right side behind line...
        motor2.drive(mspeed);    //...drive right side forward
        rmotorForward = true;
        if(lLineSide==2){        //..and if left side on line...
          motor1.drive(-mspeed); //...drive left side opposite to stay in place
          lmotorForward = false;
        }
      }
      if(rLineSide==1){          //If right side in front of line...
        motor2.drive(-mspeed);   //...drive right side backward
        rmotorForward = false;
        if(lLineSide==2){        //..and if left side on line...
          motor1.drive(mspeed);  //...drive left side opposite to stay in place
          lmotorForward = true;
        }
      }
      if(lLineSide==0){          //If left side behind line...
        motor1.drive(mspeed);    //...drive left side forward
        lmotorForward = true;
        if(rLineSide==2){        //and if right side on line...
          motor2.drive(-mspeed); //...drive right side opposite to stay in place
          rmotorForward = true;
        }
      }
      if(lLineSide==1){         //If left side in front of line...
        motor1.drive(-mspeed);  //...drive left backward
        lmotorForward = false;
        if(rLineSide==2){       //and if right side on line...
          motor2.drive(mspeed); //...drive right side opposite to stay in place
          rmotorForward = false;
        }
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



//Function for rotation (dead recknoning!)

//Rotate clockwise 90 degrees
void rotCW90(){
  motor1.drive(200);
  motor2.drive(-200);
  delay(400);
}

//Rotate counterclockwise 90 degrees
void rotCCW90(){
  motor1.drive(-200);
  motor2.drive(200);
  delay(400);
}
