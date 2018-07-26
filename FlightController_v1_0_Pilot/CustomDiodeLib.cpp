// Date: 18.07.2018
// Author: Jan Wielgus
//
// 

#include "CustomDiodeLib.h"
//CustomDiodeLibClass CustomDiodeLib;


CustomDiodeLibClass::CustomDiodeLibClass(int pin, bool isPwm=false)
{
	this->pin = pin;
	this->isPwm = isPwm;
}


void CustomDiodeLibClass::init()
{
	pinMode(pin, OUTPUT);
}


void CustomDiodeLibClass::setPattern(uint8_t pattern=2 , uint16_t interv=500)
{
	//...
	
	runDiode();
}


void CustomDiodeLibClass::runDiode()
{
	switch(pattern)
	{
		case DIODE_OFF:
			digitalWrite(pin, LOW);
			break;
			
		case DIODE_ON:
			digitalWrite(pin, HIGH);
			break;
			
		default:
			digitalWrite(pin, HIGH);
			break;
	}
}


