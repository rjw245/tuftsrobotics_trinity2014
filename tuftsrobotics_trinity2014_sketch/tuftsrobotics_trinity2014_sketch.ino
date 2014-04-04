#include <Servo.h>

#include "motor.h"
#include "motorcontrol.h"
#include "fireSensorArray.h"

//Set 0 if this bot is "Irrelephant"
#define IS_DELUX              1

//DEFINE ALL PINS HERE
#if IS_DELUX
  #define lineLeftPin           A0
  #define lineRightPin          A1
  #define distRightBackPin      A12
  #define distRightFrontPin     A11
  #define distFrontPin          A10
  #define distLeftBackPin       A14
  #define distLeftFrontPin      A13
  #define fireSensePin1         A3
  #define fireSensePin2         A2
  #define fireSensePin3         A4
  #define fireSensePin4         -1
  #define fireSensePin5         -1
  #define startButton           8

  //Motor pins
  #define leftMotordig           4
  #define leftMotorpwm           5
  #define rightMotordig          7
  #define rightMotorpwm          6
  
#else

  #define lineLeftPin           A0
  #define lineRightPin          A1
  #define distRightBackPin      A12
  #define distRightFrontPin     A11
  #define distFrontPin          A10
  #define distLeftBackPin       A14
  #define distLeftFrontPin      A13
  #define fireSensePin1         A3
  #define fireSensePin2         A2
  #define fireSensePin3         A4
  #define fireSensePin4         -1
  #define fireSensePin5         -1
  #define startButton           8

  //Motor pins
  #define leftMotordig           4
  #define leftMotorpwm           5
  #define rightMotordig          7
  #define rightMotorpwm          6
#endif

//Possible States
#define INITIALIZATION        0
#define WALLFOLLOW            1
#define INROOM                2
#define FOUNDFIRE             3
#define RETURNHOME            4
#define ALIGNLINE             5
#define PUTOUTFIRE            6
#define ALIGNFIRE             7

//Wall-follow constants
#define FRONTOBSTACLEDIST     400

//Line sense & alignment constants
#define LINESENSED            500
#define LINE_INFRONT          0
#define LINE_BEHIND           1
#define ON_LINE               2
#define LINE_ADJ_SPEED        180
#define LINESENSING_INVERTED  0 //1 = look for white, 0 = look for black

//Fire sensing constants
#define FIRESENSED            50
#define FIRECLOSE             600

//Motor controller object
//(motorcontrol.h)
MotorControl mcontrol(DERIVATIVE);

//Fire sensor object
//(fireSensorArray.h)
FireSensorArray fireSense;


//Motor objects
Motor leftMotor;
Motor rightMotor;

//Servo for canister
Servo pullServo;

//Start state is INITIALIZATION
int STATE = INITIALIZATION;

void setup() {
  //Set up motors with proper pins
  leftMotor.attach(leftMotordig,leftMotorpwm);
  rightMotor.attach(rightMotordig,rightMotorpwm);
  
  pullServo.attach(10);
  pullServo.write(10);
  
  //Get left motor in proper orientation
  leftMotor.flip();
  
  //Give these motors to the motor controller for wall following
  mcontrol.attach(&leftMotor,&rightMotor);
  
  //Initialize array of fire sense pins
  //and pass it to the fire sensor array object
  int fireSensePins[3] = {fireSensePin1,fireSensePin2,fireSensePin3};
  fireSense.attach(fireSensePins);
  
  
  Serial.begin(9600);
  pinMode(lineLeftPin,INPUT);
  pinMode(lineRightPin,INPUT);
  pinMode(distRightBackPin,INPUT);
  pinMode(distRightFrontPin,INPUT);
  pinMode(distFrontPin,INPUT);
  pinMode(distLeftBackPin,INPUT);
  pinMode(distLeftFrontPin,INPUT);
  pinMode(startButton,INPUT);
  
  //Wait for start
  while(digitalRead(startButton)==LOW);
  Serial.println("Started");
}

//These declarations are for line adjustment
int lineLeft,lineRight;
boolean lineLeftSensed, lineRightSensed;
int rLineSide = LINE_INFRONT;   //Constants define
int lLineSide = LINE_INFRONT;   //at top of code
//END DECLARATIONS FOR LINE ADJUSTMENT

//These declarations are for fire sensing
int fire_motinertia = 140;
int fire_motcorrection = 60;
//END DECLARATIONS FOR FIRE SENSING

void loop() {
  switch(STATE){
    case INITIALIZATION:
      //Rotate 90 deg CW at start
      rotCW90();
      
      //Is there something in front of me? If so, rotate 90 CCW
      if(analogRead(distFrontPin)>FRONTOBSTACLEDIST){
        //leftMotor.drive(-200);
        //rightMotor.drive(200);
      }
      //Wall follow
      mcontrol.drive(analogRead(distRightBackPin),analogRead(distRightFrontPin),90);
      
      //After 3 seconds, initialization is over, so go to next state
      if(millis()>=3000){
        STATE = WALLFOLLOW;
      }
      
      break;
      
    case WALLFOLLOW:
      //Serial.println("WALL FOLLOWING");
      //Look for front obstacles:
      //Is there something in front of me? If so, rotate 90 CCW
      if(analogRead(distFrontPin)>FRONTOBSTACLEDIST){
        //leftMotor.drive(-200);
        //rightMotor.drive(200);
      }
      mcontrol.drive(analogRead(distRightBackPin),analogRead(distRightFrontPin),140);
      
      
      //Look for lines. If found, change state to aligning with line
      #if LINESENSING_INVERTED
        if(analogRead(lineLeftPin)>LINESENSED || analogRead(lineRightPin)>LINESENSED){
          //STATE = ALIGNLINE;
        }
      #else
        if(analogRead(lineLeftPin)<LINESENSED || analogRead(lineRightPin)<LINESENSED){
          //STATE = ALIGNLINE;
        }
      #endif
      
      break;
     
    
    case ALIGNLINE:
      lineLeft  = analogRead(lineLeftPin);
      lineRight = analogRead(lineRightPin);
      
      #if LINESENSING_INVERTED
        lineLeftSensed  = (lineLeft  > LINESENSED);
        lineRightSensed = (lineRight > LINESENSED);
      #else
        lineLeftSensed  = (lineLeft  < LINESENSED);
        lineRightSensed = (lineRight < LINESENSED);
      #endif
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
        //STATE = INROOM;                                    //and then change to next state
      }
      
      
      //Drive motors appropriately
      if(rLineSide == LINE_INFRONT){        //If right side behind line...
        rightMotor.drive(LINE_ADJ_SPEED);   //...drive right side forward
        if(lLineSide == ON_LINE){           //..and if left side on line...
          leftMotor.drive(-LINE_ADJ_SPEED); //...drive left side opposite to stay in place
        }
      }
      if(rLineSide == LINE_BEHIND){         //If right side in front of line...
        rightMotor.drive(-LINE_ADJ_SPEED);  //...drive right side backward
        if(lLineSide == ON_LINE){           //..and if left side on line...
          leftMotor.drive(LINE_ADJ_SPEED);  //...drive left side opposite to stay in place
        }
      }
      if(lLineSide == LINE_INFRONT){        //If left side behind line...
        leftMotor.drive(LINE_ADJ_SPEED);    //...drive left side forward
        if(rLineSide == ON_LINE){           //and if right side on line...
          rightMotor.drive(-LINE_ADJ_SPEED);//...drive right side opposite to stay in place
        }
      }
      if(lLineSide == LINE_BEHIND){         //If left side in front of line...
        leftMotor.drive(-LINE_ADJ_SPEED);   //...drive left backward
        if(rLineSide == ON_LINE){           //and if right side on line...
          rightMotor.drive(LINE_ADJ_SPEED); //...drive right side opposite to stay in place
        }
      }
        
      
      break;
      
    case INROOM:
      //IF fire sensed

      if(fireSense.isThereFire()){
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
      if(fireSense.fireStrength()>=FIRECLOSE){
        fire_motinertia = 0;
      }
      if(abs(fireSense.fireAngle())>=7){
        leftMotor.drive(fire_motinertia + fire_motcorrection);
        rightMotor.drive(fire_motinertia - fire_motcorrection);
      }
      else{
        STATE = ALIGNFIRE;
      }
      
      break;
      
    case ALIGNFIRE:
      
      
    case PUTOUTFIRE:
      pullServo.write(160);
      delay(1200);
      pullServo.write(10);
      delay(1000);
      STATE = RETURNHOME;
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
  
  Serial.print("LINE SENSOR - Left:            ");
  Serial.println(analogRead(lineLeftPin));
  
  Serial.print("LINE SENSOR - Right:           ");
  Serial.println(analogRead(lineRightPin));
  
  Serial.println();
  
  Serial.print("FIRE SENSOR - Left:              ");
  Serial.println(analogRead(fireSensePin1));
  
  Serial.print("FIRE SENSOR - Center:            ");
  Serial.println(analogRead(fireSensePin2));
  
  Serial.print("FIRE SENSOR - Right:             ");
  Serial.println(analogRead(fireSensePin3));
  
  Serial.println();
  
  Serial.print("FIRE SENSOR - Angle:             ");
  Serial.println(fireSense.fireAngle());
  
  Serial.print("FIRE SENSOR - Strength:          ");
  Serial.println(fireSense.fireStrength());
  
  Serial.println();
  
  Serial.print("DISTANCE SENSOR - Left-Back:   ");
  Serial.println(analogRead(distLeftBackPin));
  
  Serial.print("DISTANCE SENSOR - Left-Front:  ");
  Serial.println(analogRead(distLeftFrontPin));
  
  Serial.print("DISTANCE SENSOR - Right-Back:  ");
  Serial.println(analogRead(distRightBackPin));
  
  Serial.print("DISTANCE SENSOR - Right-Front: ");
  Serial.println(analogRead(distRightFrontPin));
  
  Serial.print("DISTANCE SENSOR - Front:       ");
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
