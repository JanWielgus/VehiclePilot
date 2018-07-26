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


bool gestureDiodeSteernigFlag = false;   // true - rozpoznawanie gestow steruje dioda (priorytet), false - normalne sterowanie dioda


void setup()
{
	#ifdef _INO_DEBUG
		Serial.begin(9600);
		Serial.println("INO DEBUG has sterted");
#endif

	lcd.init(); // I2C
	com.init();
	
	// init diodes
	red.init();
	green.init();
	
	cpa.init();
	//Wire.setClock(400000L); // 400kHz  DO PRZETESTOWANIA !!! jak zadziala to uzyc z tym
	
	lcd.backlight();
	lcd.setCursor(0,0);
	lcd.print("Pilot drona :)");
}

void loop()
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
	
// Obliczanie drazkow
	static float lastThrottle=0;
	static float lastRotate=0;
	static float lastTiltTB=0;
	static float lastTIltLR=0;
	// EWA filters
	lastThrottle = STEERING_FILTER_BETA*lastThrottle + (1-STEERING_FILTER_BETA)*analogRead(pinThrottle);
	lastRotate = STEERING_FILTER_BETA*lastRotate + (1-STEERING_FILTER_BETA)*analogRead(pinRotate);
	lastTiltTB = STEERING_FILTER_BETA*lastTiltTB + (1-STEERING_FILTER_BETA)*analogRead(pinTiltTB);
	lastTIltLR = STEERING_FILTER_BETA*lastTIltLR + (1-STEERING_FILTER_BETA)*analogRead(pinTiltLR);
	// maps ans constrains (final step)
	com.pilot.throttle = constrain(map(long(lastThrottle), 960, 65, 0, 1000), 0, 1000);
	com.pilot.rotate = constrain(map(long(lastRotate), 968, 50, -450, 450), -450, 450);
	com.pilot.tilt_TB = constrain(map(long(lastTiltTB), 900, 20, -450, 450), -450, 450);
	com.pilot.tilt_LR = constrain(map(long(lastTIltLR), 982, 67, -450, 450), -450, 450);
	
	#ifdef _INO_DEBUG
		Serial.print("DRAZKI: THR: ");
		Serial.print(com.pilot.throttle);
		Serial.print("\tROT: ");
		Serial.print(com.pilot.rotate);
		Serial.print("\tTB: ");
		Serial.print(com.pilot.tilt_TB);
		Serial.print("\tLR: ");
		Serial.println(com.pilot.tilt_LR);
#endif
	
	
	//com.wyslij(PILOT_RAMKA_TEST_TYPE);   // DO PRZEBUDOWY
	
	#ifdef USE_PC_APP
		// Calc steering data for pc app
		cpa.sterVar.throttle = com.pilot.throttle/4; //    0:250 dla apki na pc
		cpa.sterVar.rotate = com.pilot.rotate/3;     // -150:150
		cpa.sterVar.tiltTB = com.pilot.tilt_TB/3;    // -150:150
		cpa.sterVar.tiltLR = com.pilot.tilt_LR/3;    // -150:150
		
		cpa.wyslij(); // wyslij do apki pc
	#endif
	
	
	
	// <<<<< ====== ---  RZECZY POBOCZNE  --- ====== >>>>>

	// Naraznie testowe
	if (!gestureDiodeSteernig) // jesli diodami nie steruje rozpoznawanie gestow
	{
		com.connectionState() ? green.setPattern(DIODE_ON) : green.setPattern(DIODE_OFF);
		//if (com.connectionState()) green.setPattern(DIODE_ON);
		//else green.setPattern(DIODE_OFF);
		com.armState ? red.setPattern(DIODE_ON) : red.setPattern(DIODE_OFF);
	}
	
	red.runDiode();
	green.runDiode();
	
	//delay(48);  // ========asdfasdfajsdkj     DO PRZEMYSLENIA  !!! 
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
			if (com.pilot.throttle < 430) // drazek nie jest w pozycji do uzbrajania
			{
				s1RotateRightBegan = false;
				s1RotateRightCompleted = false;
				gestureDiodeSteernigFlag = false; // dioda nie steruje wykrywanie gestow
			}
			else if (s1RotateRightBegan && !s1RotateRightCompleted && (millis()-s1StartTime) >= 2000) // jesli rozpoczeto trzymanie i trzymano przez min 2s
			{
				s1RotateRightCompleted = true;
				green.setPattern(2, 400); // miganie 400ms
			}
			else if (!s1RotateRightBegan) // pierwsze wykrycie rotate w prawo
			{
				s1StartTime = millis();
				s1RotateRightBegan = true;
				gestureDiodeSteernigFlag = true;
				red.setPattern(4, 2000); // rozjasnianie w 2s
				green.setPattern(DIODE_OFF);
			}
		}
		else if(s0ThrottleIdle && s1RotateRightCompleted && s0TiltTBIdle && !s2TiltLRCompleted) // warunek do rozpoczecia etapu 2
		{
			// ETAP 2
			if ((millis()-s1StartTime-2000) < 700 && com.pilot.rotate > 430) // jesli miesci sie w czasie i jesli rotate jest trzymane
			{
				if (com.pilot.tilt_LR < -430)
					s2TiltLRCompleted = true;
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
			if ((millis()-s1StartTime-2000) < 700) // jesli nie trwa to juz dluzej niz 700ms
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
	else // jest uzbrojony  -  tylko rozpoznawanie rozbrojenia
	{
		
	}
		
}
