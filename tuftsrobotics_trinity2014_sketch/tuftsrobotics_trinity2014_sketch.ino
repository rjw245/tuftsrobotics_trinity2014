#include "motor.h"
#include "motorcontrol.h"
#include "fireSensorArray.h"

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

//Motor pins
#define leftMotordig           4
#define leftMotorpwm           5
#define rightMotordig          7
#define rightMotorpwm          6

//Possible States
#define INITIALIZATION        0
#define WALLFOLLOW            1
#define INROOM                2
#define FOUNDFIRE             3
#define RETURNHOME            4
#define ALIGNLINE             5

//Wall-follow constants
#define FRONTOBSTACLEDIST     300

//Line sense & alignment constants
#define LINESENSED            500
#define LINE_INFRONT          0
#define LINE_BEHIND           1
#define ON_LINE               2

//Fire sensing constants
#define FIRESENSED            50

//Motor controller object
MotorControl mcontrol(PROPORTIONAL);

//Motor objects
Motor leftMotor;
Motor rightMotor;

//Start state is INITIALIZATION
int STATE = INITIALIZATION;

void setup() {
  //Set up motors with proper pins
  leftMotor.attach(leftMotordig,leftMotorpwm);
  rightMotor.attach(rightMotordig,rightMotorpwm);
  
  //Get left motor in proper orientation
  leftMotor.flip();
  
  //Give these motors to the motor controller for wall following
  mcontrol.attach(&leftMotor,&rightMotor);
  
  
  Serial.begin(9600);
  pinMode(lineLeftPin,INPUT);
  pinMode(lineRightPin,INPUT);
  pinMode(distRightBackPin,INPUT);
  pinMode(distRightFrontPin,INPUT);
  pinMode(distFrontPin,INPUT);
  pinMode(distLeftBackPin,INPUT);
  pinMode(distLeftFrontPin,INPUT);
}

//These declarations are for line adjustment
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
      
      //Look for lines. If found, change state to aligning with line
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
      
      //Currently on the line on right side?
      if(rLineSide == ON_LINE){
        
        //If moving forward, moving past line -> LINE_BEHIND
        //If moving backward, moving past line -> LINE_INFRONT
                                   
        //This is done in case the robot is about to
        //go back off the line, so we know what side
        //of the line it must be on when it does
        
        if(rightMotor.isMovingForward()){
          rLineSide = LINE_BEHIND;
        }
        else{
          rLineSide = LINE_INFRONT;
        }
                                   
      }
      if (lLineSide == ON_LINE){
        if(leftMotor.isMovingForward()){         //Ditto
          lLineSide = LINE_BEHIND;
        }
        else{
          lLineSide = LINE_INFRONT;
        }
      }
      
      if (lineRightSensed){        //If we are still on the line, store that fact
        rLineSide = ON_LINE;
      }
      if (lineLeftSensed){         //...and for the left side too
        lLineSide = ON_LINE;
      }
      
      if (rLineSide == ON_LINE && lLineSide == ON_LINE){   //Both sides on line!
        Serial.println("ALIGNED WITH LINE!!");
        delay(100);                                        //Pause
        STATE = INROOM;                                    //and then change to next state
      }
      
      if(rLineSide == LINE_INFRONT){ //If right side behind line...
        rightMotor.drive(mspeed);        //...drive right side forward
        if(lLineSide == ON_LINE){    //..and if left side on line...
          leftMotor.drive(-mspeed);     //...drive left side opposite to stay in place
        }
      }
      if(rLineSide == LINE_BEHIND){  //If right side in front of line...
        rightMotor.drive(-mspeed);       //...drive right side backward
        if(lLineSide == ON_LINE){    //..and if left side on line...
          leftMotor.drive(mspeed);      //...drive left side opposite to stay in place
        }
      }
      if(lLineSide == LINE_INFRONT){ //If left side behind line...
        leftMotor.drive(mspeed);        //...drive left side forward
        if(rLineSide == ON_LINE){    //and if right side on line...
          rightMotor.drive(-mspeed);     //...drive right side opposite to stay in place
        }
      }
      if(lLineSide == LINE_BEHIND){  //If left side in front of line...
        leftMotor.drive(-mspeed);       //...drive left backward
        if(rLineSide == ON_LINE){    //and if right side on line...
          rightMotor.drive(mspeed);      //...drive right side opposite to stay in place
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
  
  Serial.println("--------------------------------------------------");
}



//Function for rotation (dead recknoning!)

//Rotate clockwise 90 degrees
void rotCW90(){
  leftMotor.drive(200);
  rightMotor.drive(-200);
  delay(400);
}

//Rotate counterclockwise 90 degrees
void rotCCW90(){
  leftMotor.drive(-200);
  rightMotor.drive(200);
  delay(400);
}
