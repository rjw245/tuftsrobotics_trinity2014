#include "fireSensorArray.h"

FireSensorArray::FireSensorArray(){
	for(int i=0; i<NUMFIRESENSORS; i++){
		fireSensePins[i] = -1;
	}
}

void FireSensorArray::attach(int pins[NUMFIRESENSORS]){
	for(int i=0; i<NUMFIRESENSORS; i++){
		fireSensePins[i] = pins[i];
		pinMode(fireSensePins[i],INPUT);
	}
}

boolean FireSensorArray::isThereFire(){
	for(int i=0; i<NUMFIRESENSORS; i++){
		if(analogRead(fireSensePins[i])>FIRETHRESHOLD)
			return true;
		}
	}
	return false;
}

int FireSensorArray::fireAngle(){
  int curMax = fireSensePins[NUMFIRESENSORS/2]; //Middle one
  int angle = map((fireSensePins[NUMFIRESENSORS-1] - fireSensePins[0]),-curMax, curMax,-45,45);
  return angle;
}
