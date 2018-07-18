// Kod pilota drona do wersji 1.0
//
// Author: Jan Wielgus
// Date: 26.10.2017r.
//

#include "Communication.h"
#include "ControlPanelApp.h"
#include "CustomDiodeLib.h"
#include "config.h"

//SoftwareSerial software_serial(tx_pin, rx_pin); // zapasowy dla mudulu BT

CustomDiodeLibClass red(redDiodePin, true);
CustomDiodeLibClass green(greenDiodePin, true);


void setup()
{
	#ifdef _INO_DEBUG
		Serial.begin(9600);
		Serial.println("INO DEBUG has sterted");
	#endif

	com.init();
	
	// init diodes
	red.init();
	green.init();
	
	//kom.setupConfigPacket(); // zrobic to kompletnie inaczej
	
	//cpa.init(); // bedzie przerobione na obsluge z zewnetrznym urzadzeniem I2C !!!
	
	// PRZEROBIC NA tak jak jest w pomyslach (z nowym radiem)
		delay(100);
		com.wyslij(PILOT_RAMKA_CONFIG_TYPE);
		red.setPattern(DIODE_ON);
		green.setPattern(DIODE_ON);
		delay(50);
		green.setPattern(DIODE_OFF);
		com.wyslij(PILOT_RAMKA_CONFIG_TYPE);
		delay(10);
		red.setPattern(DIODE_OFF);
}

void loop()
{
	//kom.odbierz();
	
	//cpa.odbierz(); // bedzie przerobione na zewnetrzne urzadzenie I2C
	
	com.pilot.throttle = analogRead(pinThrottle);
	cpa.throttlePCapp = kom.pilot.throttle/4.1; // 0-250 dla apki na pc
	
	com.pilot.throttle = map(kom.pilot.throttle, 10, 1023, 0, 1000);
	com.pilot.throttle = constrain(kom.pilot.throttle, 0, 1000);
	
	//com.wyslij(PILOT_RAMKA_TEST_TYPE);
	
	//cpa.wyslij(cpa.KOMUN_RAMKA_ARDU_LIVE_TYPE); bedznie po I2C

	if (com.connectionState()) green.setPattern(DIODE_ON);
	else green.setPattern(DIODE_OFF);
	
	red.runDiode();
	green.runDiode();
	
	delay(48);
}
