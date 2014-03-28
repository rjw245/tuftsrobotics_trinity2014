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

}