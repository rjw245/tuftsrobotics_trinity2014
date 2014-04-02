#ifndef FIRESENSORARRAY_H
#define FIRESENSORARRAY_H
#include "Arduino.h"

#define NUMFIRESENSORS  3
#define FIRETHRESHOLD   50

class FireSensorArray{
	public:
		FireSensorArray();
		void attach(int pins[NUMFIRESENSORS]);
		boolean isThereFire();
		int fireAngle();
	private:
		int fireSensePins[NUMFIRESENSORS];
};

#endif
