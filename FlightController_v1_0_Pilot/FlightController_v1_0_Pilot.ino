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

//SoftwareSerial software_serial(tx_pin, rx_pin); // zapasowy dla mudulu BT
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);
CustomDiodeLibClass red(redDiodePin, true);
CustomDiodeLibClass green(greenDiodePin, true);


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
	
	//kom.setupConfigPacket(); // zrobic to kompletnie inaczej
	
	cpa.init(); // bedzie przerobione na obsluge z zewnetrznym urzadzeniem I2C !!!
	//Wire.setClock(400000L); // 400kHz
	
	lcd.backlight();
	lcd.setCursor(0,0);
	lcd.print("Pilot drona :)");
	
	// PRZEROBIC NA tak jak jest w pomyslach (z nowym radiem)
		delay(100);
		com.wyslij(PILOT_RAMKA_CONFIG_TYPE);
		red.setPattern(DIODE_ON);
		green.setPattern(DIODE_ON);
		delay(50);
		green.setPattern(DIODE_OFF);
		com.wyslij(PILOT_RAMKA_CONFIG_TYPE);
		delay(10);
		red.setPattern(2);
}

void loop()
{
	// <<<<< ====== ---  GLOWNE  --- ====== >>>>>
	
	//kom.odbierz(); // nie dziala przy starych radiach
	
	#ifdef USE_PC_APP
		static int32_t lastCpaRTime = 0; // czas oststniego odebrania danych od posrednika I2C pc app
		if (millis()-lastCpaRTime > 300) // odbieranie duzej paczki
		{
			cpa.odbierz(); // w config ustawia sie czy dziala przez UART0 czy I2C
			lastCpaRTime = millis();
		}
	#endif
	
	com.pilot.throttle = analogRead(pinThrottle);
	com.pilot.rotate = analogRead(pinRotate);
	com.pilot.tilt_TB = analogRead(pinTiltTB);
	com.pilot.tilt_LR = analogRead(pinTiltLR);
	
	cpa.sterVar.throttle = com.pilot.throttle/4; // 0-255 dla apki na pc
	cpa.sterVar.rotate = com.pilot.rotate/4;
	cpa.sterVar.tiltTB = com.pilot.tilt_TB/4;
	cpa.sterVar.tiltLR = com.pilot.tilt_LR/4;
	
	// To jest ze starej wersji i tego ma nie byæ
		com.pilot.throttle = map(kom.pilot.throttle, 10, 1023, 0, 1000);
		com.pilot.throttle = constrain(kom.pilot.throttle, 0, 1000);
	// tego ^^^
	
	//com.wyslij(PILOT_RAMKA_TEST_TYPE);   // DO PRZEBUDOWY
	
	#ifdef USE_PC_APP
		cpa.wyslij(); // wyslij do apki pc
	#endif
	
	
	
	// <<<<< ====== ---  RZCZY POBOCZNE  --- ====== >>>>>

	// Naraznie testowe
	if (com.connectionState()) green.setPattern(DIODE_ON);
	else green.setPattern(DIODE_OFF);
	
	red.runDiode();
	green.runDiode();
	
	delay(48);
}
