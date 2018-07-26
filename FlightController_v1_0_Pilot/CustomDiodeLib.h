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
	void setPattern(uint8_t ptn=2 , uint16_t time1=501, uint16_t time2=0); // Zmiana schematu swiecenia
	void runDiode(); // do uzywania w loop. Obsluguje diode
	
 private:
	int pin;
	bool isPwm;
	uint8_t pattern = DIODE_OFF;
	float value = 0; // jesli PWM to jest to obecna wartosc 0<X<200 a jesli nie to wieksze od 127 pali sie a mniejsze nie
	uint16_t param1, param2; // parametry podawane do metody setPattern()
	
	uint32_t endTime; // czas kiedy powinna sie skonczyc jakas akcja (np. rozjasnianie)
	uint16_t actionTime = 0; // czas na wykonanie obecnej akcji
	bool isBlinkModeFlag = false; // blink to zmienianie pat4 i pat5 wiec jesli jest blink to zmieniaj z jednego na drugi
};

//extern CustomDiodeLibClass CustomDiodeLib;

#endif

/*
Patterns:
	0 - OFF
	1 - ON
	2 - zmieniaj stan co param1 (domyslnie param=500ms)  |> jesli param2 != 0 <|  param1 - czas kiedy rozjasnia, param2 - czas kiedy zgasza
	3 - brightness of diode (MIN)0 < param1 < 255(MAX) (param2 - czas rozjasniania (narazie bez tego), 0 - od razu ustaw)
	4 - (jeœli PWM) 0 to 100 w param1 sec (domyslnie 0.5s)
	5 - (jeœli PWM) 100 to 0 w param1 sec (domyslnie 0.5s)

*/
