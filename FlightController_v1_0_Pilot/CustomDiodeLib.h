// CustomDiodeLib.h
// Date: 18.07.2018
// Author: Jan Wielgus
//
// Library that enable custom usage of diodes without stopping program

#ifndef _CUSTOMDIODELIB_h
#define _CUSTOMDIODELIB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define MAX_PWM 255
#define DIODE_OFF 0
#define DIODE_ON 1

class CustomDiodeLibClass
{
 public:
	CustomDiodeLibClass(int pin, bool isPwm=false);
	void init();
	void setPattern(uint8_t pattern=2, uint16_t interv=500); // Zmiana schematu swiecenia
	void runDiode(); // do uzywania w loop. Obsluguje diode
	
 private:
	int pin;
	bool isPwm;
	uint8_t pattern = DIODE_OFF;
};

//extern CustomDiodeLibClass CustomDiodeLib;

#endif

/*
Patterns:
	0 - OFF
	1 - ON
	2 - blink every 1 sec or interv if proided
	4 - 0 to 100 in 1 sec or in interv time if provided (if PWM)
	5 - 100 to 0 in 1 sec or in interv time if provided (if PWM)

*/
