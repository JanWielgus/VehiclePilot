/*
 * config.h
 *
 *  Created: 29.10.2017 21:04:31
 *  Author: Janek
 *
 *	Plk konfiguracji wszystkich parametrów pilota
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

// DEBUG (only one could be uncommented)
	//#define _INO_DEBUG
	//#define _COM_DEBUG
	

// CONTROL_PANEL_APP
	//#define USE_PC_APP // Czy wlaczac obsluge apki pc. Jesli nie ma pidowania albo nie potrzeba czegos wyswietlac to nie ma sensu
	#define CPA_I2C    // Brakuje protow UART dlatego komunikacja bedzie musiala przebiegac przez zewnetrzne urzadzenie I2C


// <<<<<<<<<=====================     KOMUNIKACJA     =====================>>>>>>>>>

	//-------Pamiêtaj o zmianie wielkoœci ramek po zmianie protoko³u!!!-------
	
	//      ===  TYLKO TABLICE O WYMIARACH TAKICH JAK RAMKA, NIE WIÊKSZE !!!  ===
	
	#define serialPort 1                      // Serial: 0, Serial1: 1, Serial2: 2, Serial3: 3
	#define comSerial Serial1                 // Serial with transceiver
	#define BAUD_RATE 115200                  // With transciever
	#define MAX_SEND_SIZE 45                  // Size of the largest packet (could be higher than needed)

	// Szablon: nadawca_RAMKA_nazwa_SIZE/TYPE
	
	
	// --  SEND  --
		// Wersja 1 - rozszerzona (pelna)
		#define PILOT_RAMKA_VER1_SIZE 26
		#define PILOT_RAMKA_VER1_TYPE 0x00
		
		// Wersja 2 - podstawowa (narazie bez)
		//#define PILOT_RAMKA_VER2_SIZE x
		//#define PILOT_RAMKA_VER1_TYPE 0x01
		
		// Wersja 3 - parametry PID
		#define PILOT_RAMKA_VER3_SIZE 41
		#define PILOT_RAMKA_VER3_TYPE 0x02
	
	// --  RECEIVE  --
		// Wersja 1 - rozszerzona (pelna)
		#define DRON_RAMKA_VER1_SIZE 32
		#define DRON_RAMKA_VER1_TYPE 0x00
	
		// Wersja 2 - podstawowa (narazie bez)
		//#define DRON_RAMKA_VER2_SIZE x
		//#define DRON_RAMKA_VER2_TYPE 0x01
	
		// Wersja 3 - po uzbrojeniu (takze PID request)
		#define DRON_RAMKA_VER3_SIZE 20
		#define DRON_RAMKA_VER3_TYPE 0x02
	
	
	
	// times [ms] after vehicle set ... power; eg. after x[ms] high power is set
	#define MP_SET_TIME 1100  // Medium power (21dBm)
	
	#define  RESTORE_CONNECTION_PARAMS_TIME 3000  // Time after signal lost when OTA speed is set to 1.2kbp and smallest packet is used

	#define MAX_LOST_PACKETS 2          // If more -> signal lost mode
	
	// transmission powers
	#define DBM20 4  // 20dBm
	#define DBM14 3  // 14dBm
	#define DBM8 2  // 8dBm
	#define DBM2 1  // 2dBm
	
	#define M0pin 10
	#define M1pin 9
	#define AUXpin 11
	
// <<<<<<<<<=====================     PINY     =====================>>>>>>>>>
	
	// Software serial pins for BT module
	#define tx_pin 2
	#define rx_pin 3
	
	#define buzzer 12 // buzzer pin
	
	#define pinThrottle A0
	#define pinRotate A1
	#define pinTiltTB A2
	#define pinTiltLR A3
	
	#define redDiodePin 5
	#define greenDiodePin 6
	
	
// EWA filters betas
	#define STEERING_FILTER_BETA 0.6
	
	
// ADDRESSES
	#define LCD_ADDRESS 0x3F
	
	
// CONSTS
	#define ZERO_STEERING 10    // +- tyle musi byc aby uznac ze drazek jest w zerze
	
	// Uzbrajanie
	#define ARMING_S1_MIN_TIME 1000
	#define ARMING_S23_MAX_TIME 1500
	
	// Drazki
		// Throttle
			#define ThrottleMin 65
			//#define ThrottleMid 500
			#define ThrottleMax 960
		// Rotate
			#define RotateMin 48
			#define RotateMid 514
			#define RotateMax 970
		// TB
			#define TiltTBMin 16
			#define TiltTBMid 490
			#define TiltTBMax 900
		// LR
			#define TiltLRMin 65
			#define TiltLRMid 526
			#define TiltLRMax 984
		#define DeadZone 20



#endif /* CONFIG_H_ */