// Kod pilota drona do wersji 1.0
//
// Author: Jan Wielgus
// Date: 26.10.2017r.
//

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Communication.h"
#include "ControlPanelApp.h"
#include "CustomDiodeLib.h"
#include "config.h"

// Functions
void gestureRecognition();        // Obsluguje rozpoznawanie gestow. Trzeba wywolywac w kazdym loopie

//SoftwareSerial software_serial(tx_pin, rx_pin); // zapasowy dla mudulu BT
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);
CustomDiodeLibClass red(redDiodePin, true);
CustomDiodeLibClass green(greenDiodePin, true);

uint32_t lastLoopTime=0;
bool gestureDiodeSteernigFlag = false;   // true - rozpoznawanie gestow steruje dioda (priorytet), false - normalne sterowanie dioda

float lastThrottle=0;
float lastRotate=0;
float lastTiltTB=0;
float lastTiltLR=0;
uint16_t rawThr, rawRot, rawTB, rawLR;



void setup()
{
	#ifdef _INO_DEBUG
		Serial.begin(115200); // Jesli baud rate jest inny niz komunikacji to trzeba komentowac komunikacje !!! (komunikacja leci po tym samym URAT)
		Serial.println("INO DEBUG has sterted");
#endif

	lcd.init(); // I2C
	com.init();
	
	// init diodes
	red.init();
	green.init();
	
	#ifdef _COM_DEBUG
		cpa.init();
		//Wire.setClock(400000L); // 400kHz  DO PRZETESTOWANIA !!! jak zadziala to uzyc z tym
#endif
	Wire.setClock(400000L); // 400kHz  DO PRZETESTOWANIA !!!

	lcd.backlight();
	lcd.setCursor(0,0);
	lcd.print("Pilot drona :)");
	lcd.setCursor(0, 1);
	lcd.print("ver 1.1");
	lcd.home();
	// DALEJ NARAZIE NIERUSZAC - JAKIES NADPRZYRODZONE
	green.setPattern(2, 100);
	red.setPattern(1);
	// czekaj 500ms
	
	uint32_t st = millis();
	while (millis()-st < 500)
	{
		green.runDiode();
		red.runDiode();
	}
	
	#ifdef _INO_DEBUG
		Serial.println("End of setup()");
#endif
}

void loop()
{
	// Obliczanie drazkow
	// Swap (na dole jest najwieksza a na gorze najmniejsza dlatego trzeba odwrocic)
	rawThr = abs(analogRead(pinThrottle) - (ThrottleMin+ThrottleMax));
	rawRot = abs(analogRead(pinRotate) - (RotateMin+RotateMax));
	rawTB = abs(analogRead(pinTiltTB) - (TiltTBMin+TiltTBMax));
	rawLR = abs(analogRead(pinTiltLR) - (TiltLRMin+TiltLRMax));
	// EWA filters
	lastThrottle = STEERING_FILTER_BETA*lastThrottle + (1-STEERING_FILTER_BETA)*rawThr;
	lastRotate = STEERING_FILTER_BETA*lastRotate + (1-STEERING_FILTER_BETA)*rawRot;
	lastTiltTB = STEERING_FILTER_BETA*lastTiltTB + (1-STEERING_FILTER_BETA)*rawTB;
	lastTiltLR = STEERING_FILTER_BETA*lastTiltLR + (1-STEERING_FILTER_BETA)*rawLR;
	// maps ans constrains (final step)
		// Throttle
		if (lastThrottle > ThrottleMin+DeadZone)
			com.pilot.throttle = constrain(map(lastThrottle, ThrottleMin+DeadZone, ThrottleMax, 0, 1000), 0, 1000);
		else
			com.pilot.throttle = 0;
		// Rotate
		if (lastRotate > RotateMid+DeadZone)
			com.pilot.rotate = constrain(map(lastRotate, RotateMid+DeadZone, RotateMax, 0, 450), 0, 450);
		else if (lastRotate < RotateMid-DeadZone)
			com.pilot.rotate = constrain(map(lastRotate, RotateMin, RotateMid-DeadZone, -450, 0), -450, 0);
		else
			com.pilot.rotate = 0;
		// Tilt TB
		if (lastTiltTB > TiltTBMid+DeadZone)
			com.pilot.tilt_TB = constrain(map(lastTiltTB, TiltTBMid+DeadZone, TiltTBMax, 0, 450), 0, 450);
		else if (lastTiltTB < TiltTBMid-DeadZone)
			com.pilot.tilt_TB = constrain(map(lastTiltTB, TiltTBMin, TiltTBMid-DeadZone, -450, 0), -450, 0);
		else
			com.pilot.tilt_TB = 0;
		// Tilt LR
		if (lastTiltLR > TiltLRMid+DeadZone)
			com.pilot.tilt_LR = constrain(map(lastTiltLR, TiltLRMid+DeadZone, TiltLRMax, 0, 450), 0, 450);
		else if (lastTiltLR < TiltLRMid-DeadZone)
			com.pilot.tilt_LR = constrain(map(lastTiltLR, TiltLRMin, TiltLRMid-DeadZone, -450, 0), -450, 0);
		else
			com.pilot.tilt_LR = 0;
	
	
	
if (millis()-lastLoopTime > 48) // 20 Hz
{
	// <<<<< ====== ---  GLOWNE  --- ====== >>>>>
	
	com.odbierz();
	
	#ifdef USE_PC_APP
		static int32_t lastCpaRTime = 0; // czas oststniego odebrania danych od posrednika I2C pc app
		if (millis()-lastCpaRTime > 330) // odbieranie duzej paczki
		{
			cpa.odbierz(); // w config ustawia sie czy dziala przez UART0 czy I2C
			lastCpaRTime = millis();
		}
	#endif
	
	lcd.setCursor(0, 1);
	lcd.print(com.pilot.throttle);
	lcd.print("|");
	
	#ifdef _INO_DEBUG
	
		Serial.print("DRAZKI: THR: ");
		Serial.print(com.pilot.throttle);
		Serial.print("\tROT: ");
		Serial.print(com.pilot.rotate);
		Serial.print("\tTB: ");
		Serial.print(com.pilot.tilt_TB);
		Serial.print("\tLR: ");
		Serial.println(com.pilot.tilt_LR);
	/*
		Serial.print("DRAZKI: THR: ");
		Serial.print(lastThrottle);
		Serial.print("\tROT: ");
		Serial.print(lastRotate);
		Serial.print("\tTB: ");
		Serial.print(lastTiltTB);
		Serial.print("\tLR: ");
		Serial.println(lastTiltLR);*/
#endif
	
	
	com.wyslij(PILOT_RAMKA_VER1_TYPE);
	
	#ifdef USE_PC_APP
		// Calc steering data for pc app
		cpa.sterVar.throttle = com.pilot.throttle/4; //    0:250 dla apki na pc
		cpa.sterVar.rotate = com.pilot.rotate/3;     // -150:150
		cpa.sterVar.tiltTB = com.pilot.tilt_TB/3;    // -150:150
		cpa.sterVar.tiltLR = com.pilot.tilt_LR/3;    // -150:150
		
		cpa.wyslij(); // wyslij do apki pc
	#endif
	
	
	gestureRecognition();
	
	// <<<<< ====== ---  RZECZY POBOCZNE  --- ====== >>>>>

	// Naraznie testowe
	if (!gestureDiodeSteernigFlag) // jesli diodami nie steruje rozpoznawanie gestow
	{
		com.connectionState() ? green.setPattern(DIODE_ON) : green.setPattern(DIODE_OFF);
		com.armState>50 ? red.setPattern(DIODE_ON) : red.setPattern(DIODE_OFF);
	}
	
	red.runDiode();
	green.runDiode();
	
lastLoopTime = millis();
}
}





//  FUNCTIONS \/


void gestureRecognition()
{ // sX - stage X
// ZMIENNE
	static bool s0ThrottleIdle;    // true - throttle na dole
	static bool s0RotateIdle;      // true - rotate na srodku
	static bool s0TiltTBIdle;      // true - tiltTB na srodku
	static bool s0TiltLRIdle;      // true - tiltLR na srodku
	
	// Uzbrajanie
	static bool s1RotateRightBegan;     // rotate jest trzymane w prawo
	static bool s1RotateRightCompleted; // etap z rotate ejst zakonczony
	static uint32_t s1StartTime;        // Czas trwania s1 (jesli zbyt krotko to przerywane)
	static bool s2TiltLRCompleted;      // czy LR jest w lewo
	
	// Wlaczanie menu
	//...
	
	//////////////////////////////////////////////////////////////////////////
	
	// Sprawdzenie czy drazki sa w pozycjach neutralnych
	s0ThrottleIdle = (com.pilot.throttle < ZERO_STEERING) ? true : false;
	s0RotateIdle = (com.pilot.rotate > -ZERO_STEERING && com.pilot.rotate < ZERO_STEERING) ? true : false;
	s0TiltTBIdle = (com.pilot.tilt_TB > -ZERO_STEERING && com.pilot.tilt_TB < ZERO_STEERING) ? true : false;
	s0TiltLRIdle = (com.pilot.tilt_LR > -ZERO_STEERING && com.pilot.tilt_LR < ZERO_STEERING) ? true : false;
	
	if (com.armState < 50) // jest rozbrojony - rozpoznawanie gestow
	{
		if (s0ThrottleIdle && s0TiltTBIdle && s0TiltLRIdle && !s1RotateRightCompleted) // warunek do rozpoczecia etapu 1
		{
			// ETAP 1
			if (com.pilot.rotate < 430) // drazek nie jest w pozycji do uzbrajania
			{
				s1RotateRightBegan = false;
				s1RotateRightCompleted = false;
				gestureDiodeSteernigFlag = false; // dioda nie steruje wykrywanie gestow
			}
			else if (s1RotateRightBegan && !s1RotateRightCompleted && (millis()-s1StartTime) >= ARMING_S1_MIN_TIME) // jesli rozpoczeto trzymanie i trzymano przez min 1s
			{
				s1RotateRightCompleted = true;
				green.setPattern(2, 200); // miganie 400ms
			}
			else if (!s1RotateRightBegan) // pierwsze wykrycie rotate w prawo
			{
				s1StartTime = millis();
				s1RotateRightBegan = true;
				gestureDiodeSteernigFlag = true;
				red.setPattern(4, ARMING_S1_MIN_TIME); // rozjasnianie w 1s
				green.setPattern(DIODE_OFF);
			}
		}
		else if(s0ThrottleIdle && s1RotateRightCompleted && s0TiltTBIdle && !s2TiltLRCompleted) // warunek do rozpoczecia etapu 2
		{
			// ETAP 2
			if ((millis()-s1StartTime-ARMING_S1_MIN_TIME) < ARMING_S23_MAX_TIME && com.pilot.rotate > 430) // jesli miesci sie w czasie i jesli rotate jest trzymane
			{
				if (com.pilot.tilt_LR < -430)
				{
					s2TiltLRCompleted = true;
					green.setPattern(5, s1StartTime+2700-millis()); // zgaszanie diody zielonej przez pozostaly czas
				}
			}
			else // za dlugo albo rotate nie zostalo utrzymane
			{
				s1RotateRightBegan = false;
				s1RotateRightCompleted = false;
				s2TiltLRCompleted = false;
				gestureDiodeSteernigFlag = false;
			}
		}
		else if (s2TiltLRCompleted) // jesli LR zostal przesuniety w bok, czekanie na powrot do pozycji neutralnych
		{
			// ETAP 3
			if ((millis()-s1StartTime-ARMING_S1_MIN_TIME) < ARMING_S23_MAX_TIME) // jesli nie trwa to juz dluzej niz 700ms
			{
				if (s0ThrottleIdle && s0RotateIdle && s0TiltTBIdle && s0TiltLRIdle) // jesli drazki powrocily do neutralnych pozycji
				{
					com.armState = 100;          // UZBRAJANIE <<<
					s1RotateRightBegan = false;
					s1RotateRightCompleted = false;
					s2TiltLRCompleted = false;
					gestureDiodeSteernigFlag = false;
				}
			}
			else // za dlugo
			{
				s1RotateRightBegan = false;
				s1RotateRightCompleted = false;
				s2TiltLRCompleted = false;
				gestureDiodeSteernigFlag = false;
			}
		}
		else // Drazki nie sa w pozycjach neutralnych wiec na pewno przerwanae
		{
			s1RotateRightBegan = false;
			s1RotateRightCompleted = false;
			s2TiltLRCompleted = false;
			gestureDiodeSteernigFlag = false; // dioda nie steruje wykrywanie gestow
		}
	}
	
	// ROZBRAJANIE
	else // jest uzbrojony  -  tylko rozpoznawanie rozbrojenia
	{
		if (com.pilot.throttle > 500) com.armState = 0; // TYMCZASOWE ROZBRAJANIE
	}
		
}
