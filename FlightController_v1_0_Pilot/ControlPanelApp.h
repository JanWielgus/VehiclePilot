// ControlPanelApp.h
// Author: Jan Wielgus
// Date: 19.02.2018
//

#ifndef _CONTROLPANELAPP_h
#define _CONTROLPANELAPP_h

#include <PacketSerial.h>
#include "Komunikacja.h"
#include "typyBitowe.h"
#include "config.h"


class ControlPanelAppClass
{
 public:
	PacketSerial pSerialUSB;
	uint8_t buforT[MAX_SEND_SIZE];
	
 public:
	void init();
	void odbierz();
	void odbierzPriv(const uint8_t* bufferR, size_t PacketSize);
	void wyslij(uint8_t typRamki);
	bool sprawdzSumeKontr(const uint8_t* buffer, size_t PacketSize);
	uint8_t liczSumeKontr(const uint8_t* buffer, size_t PacketSize);
	
	uint8_t throttlePCapp;
	
	
 public:
	const uint8_t USB_SERIAL_PORT = 0; // Dla Serial
	const uint8_t USB_BAUD_RATE = 9600;

	// --- ramki ---
	// wysylane
	const uint8_t KOMUN_RAMKA_PC_1_SIZE = 5;
	const uint8_t KOMUN_RAMKA_PC_1_TYPE = 0x00;

	const uint8_t KOMUN_RAMKA_PC_PARAMETERS_SIZE = 23;
	const uint8_t KOMUN_RAMKA_PC_PARAMETERS_TYPE = 0x01;
	// odbierane
	const uint8_t KOMUN_RAMKA_ARDU_LIVE_SIZE = 11;
	const uint8_t KOMUN_RAMKA_ARDU_LIVE_TYPE = 0x00;

	const uint8_t KOMUN_RAMKA_ARDU_2_SIZE = 30;
	const uint8_t KOMUN_RAMKA_ARDU_2_TYPE = 0x01;
};

extern ControlPanelAppClass cpa;

#endif


/*
	Opis protokolu Modernity Pilot-Komputer Serial:

	1. Na konkretnych pozycjach pakietu kodowanego COBS znajduje sie kolejne dane.
	Bajty wystepuja w naturalnej kolejnosci dla AVR i PC - Little Endian - najpierw idzie nizszy bajt

	2. Pakiet kierunku pilot--->komputer moze wystepowac w kilku typach:
	
		Typ ramki - live view (bajt 0: 0x00):
		0. XOR wszystkich nastepnych bajtow (suma kontrolna) (unsigned 8b)
		1. Typ ramki - 0x00 live view (unsigned 8b)
		2. --
		3. Throttle stick (unsigned 8b)
		4. Yaw stick (signed 8b)
		5. Pitch stick (signed 8b)
		6. Roll stick (signed 8b)
		7. Drone Pitch (signed 8b)
		8. Drone Roll (signed 8b)
		9. Drone Yaw (signed 8b)
		10. Booleans (unsigned 8b):
			bit 7 - 

		Typ ramki - NAZWA (bajt 0: 0x01):
		0. XOR wszystkich nastepnych bajtow (suma kontrolna) (unsigned 8b)
		1. Typ ramki - 0x01 NAZWA (unsigned 8b)
		
	
	
	3. Pakiet kierunku komputer--->pilot moze wystepowac w kilku typach:
	
		Typ ramki - NAZWA (bajt 0: 0x00):
		0. XOR wszystkich nastepnych bajtow (suma kontrolna) (unsigned 8b)
		1. Typ ramki - 0x00 NAZWA (unsigned 8b)
		

		Typ ramki - parameters (bajt 0: 0x01):
		0. XOR wszystkich nastepnych bajtow (suma kontrolna) (unsigned 8b)
		1. Typ ramki - 0x01 dane (unsigned 8b)
		2. P level PID (float 32b)
		3. cd^
		4. cd^
		5. cd^
		6. I level PID (float 32b)
		7. cd^
		8. cd^
		9. cd^
		10. D level PID (float 32b)
		11. cd^
		12. cd^
		13. cd^
		14. I max level (unsigned 8b)
		15. P yaw PID (float 32b)
		16. cd^
		17. cd^
		18. cd^
		19. D yaw PID (float 32b)
		20. cd^
		21. cd^
		22. cd^
		
*/

