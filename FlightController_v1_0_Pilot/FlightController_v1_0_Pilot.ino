// Kod pilota drona do wersji 1.0
//
// Author: Jan Wielgus
// Date: 26.10.2017r.
//

#include "Komunikacja.h"
#include "ControlPanelApp.h"

#define buzzer 12 // buzzer pin

SoftwareSerial software_serial(tx_pin, rx_pin); // HC-12 TX Pin, HC-12 RX Pin


long czas_ostatniego_pong = 0;
boolean ostatni_pong = false; //stan ostatniego odebranego pongu
boolean stan_sygnalu = false;



void setup()
{
	//Serial.begin(9600);
	kom.init(&software_serial);
	kom.setupConfigPacket();
	cpa.init();
	
	pinMode(buzzer, OUTPUT);
	
	delay(100);
	kom.wyslij(PILOT_RAMKA_CONFIG_TYPE);
	digitalWrite(buzzer, HIGH);
	delay(50);
	digitalWrite(buzzer, LOW);
	kom.wyslij(PILOT_RAMKA_CONFIG_TYPE);
	delay(10);
	digitalWrite(buzzer, HIGH);
	delay(50);
	digitalWrite(buzzer, LOW);
}

void loop()
{
	//kom.odbierz();
	//Serial.println(kom.zmiennaTestowa.value);
	cpa.odbierz();
	
	if (kom.pong.b0 != ostatni_pong)
	{
		ostatni_pong = kom.pong.b0;
		czas_ostatniego_pong = millis();
	}
	
	if ((millis() - czas_ostatniego_pong) > 1000) stan_sygnalu = false;
	else stan_sygnalu = true;
	
	kom.ping.b0 = !kom.ping.b0;
	
	kom.pilot.throttle = analogRead(A0);
	cpa.throttlePCapp = kom.pilot.throttle/4.1; // 0-250 dla apki na pc
	
	kom.pilot.throttle = map(kom.pilot.throttle, 10, 1023, 0, 1000);
	kom.pilot.throttle = constrain(kom.pilot.throttle, 0, 1000);
	
	kom.wyslij(PILOT_RAMKA_TEST_TYPE);
	cpa.wyslij(cpa.KOMUN_RAMKA_ARDU_LIVE_TYPE);

	if (stan_sygnalu == true) digitalWrite(LED_BUILTIN, HIGH);
	else digitalWrite(LED_BUILTIN, LOW);
	
	delay(48);
}

