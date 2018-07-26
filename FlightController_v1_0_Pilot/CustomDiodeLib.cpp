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


void CustomDiodeLibClass::setPattern(uint8_t ptn=2 , uint16_t p1=501, uint16_t p2=0)
{
	static uint8_t last_ptn = pattern;
	param1 = p1;
	param2 = p2;
	
	if (ptn != last_ptn || ptn == 3) // jesli jest zmiana patternu albo to jest 3
	{
		if (ptn == 2)
		{
			pattern = 4; // najpierw rozjasnianie
			isBlinkModeFlag = true; // ma zmieniac z pat4 na pat5 i odwrotnie
		}
		else
		{
			isBlinkModeFlag = false;
			pattern = ptn;
		}
		last_ptn = ptn;
		
		switch(pattern)
		{
			case DIODE_OFF:
				value = 0;
				break;
			case DIODE_ON:
				value = MAX_PWM;
				break;
			case 3: // brightness
				if (p2 == 0) value = constrain(p1, 0, MAX_PWM);
				else value = 0;
				break;
			case 4: // 0 - 100
				value = 0;
				break;
			case 5: // 100 - 0
				value = MAX_PWM;
				break;
			default:
				pattern = 1;
				value = MAX_PWM;
				break;
		}
		
		actionTime = p1;
		endTime = millis()+actionTime; // pierwsze obliczenie czasu konca
	}
}


void CustomDiodeLibClass::runDiode()
{
	// Obliczane dt
		static uint32_t lastTime=0;
		static uint16_t dtMs; // delta time [ms]
		uint32_t tNow = millis();
		dtMs = tNow - lastTime;
		lastTime = tNow;
		
	
	if (pattern == 4) // 0 - 100
	{
		if (tNow < endTime) // dzialamy
		{
			float toAdd = (float)MAX_PWM / ((float)actionTime/dtMs);
			value += toAdd<=256?toAdd:1; // Jesli poleci duza wartosc to ignoruj i dodaj 1
		}
			
		else // zmiana jesli jest blink mode
		{
			value = MAX_PWM; // powinno byc zapalone
			if (isBlinkModeFlag)
			{
				pattern = 5;
				actionTime = param2!=0 ? param2 : param1; // wybranie
				endTime = tNow + actionTime; // obliczanie czasu konca akcji
			}
		}
	}
	
	else if (pattern == 5) // 100 - 0
	{
		if (tNow < endTime)
		{
			float toSubtr = (float)MAX_PWM / ((float)actionTime/dtMs);
			value -= toSubtr<=256?toSubtr:1; // Jesli poleci duza wartosc to ignoruj i odejmij 1
		}
		
		else
		{
			value = 0; // powinno byc zero
			if (isBlinkModeFlag)
			{
				pattern = 4;
				actionTime = param1; // zawsze rosnie z param1
				endTime = tNow + actionTime; // obliczenie czasu konca
			}
		}
	}


	// Write value on pin
	value = constrain(value, 0, MAX_PWM);
	isPwm ? analogWrite(pin, (uint8_t)value) : digitalWrite(pin, value>127?HIGH:LOW);
}


