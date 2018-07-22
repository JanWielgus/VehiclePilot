/*
 * config.h
 *
 *  Created: 29.10.2017 21:04:31
 *  Author: Janek
 *
 *	Plk konfiguracji wszystkich parametr�w pilota
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

// DEBUG (only one could be uncommented)
	//#define _INO_DEBUG
	//#define _COM_DEBUG
	

// CONTROL_PANEL_APP
	#define USE_PC_APP // Czy wlaczac obsluge apki pc. Jesli nie ma pidowania albo nie potrzeba czegos wyswietlac to nie ma sensu
	#define CPA_I2C    // Brakuje protow UART dlatego komunikacja bedzie musiala przebiegac przez zewnetrzne urzadzenie I2C


// <<<<<<<<<=====================     KOMUNIKACJA     =====================>>>>>>>>>

	//-------Pami�taj o zmianie wielko�ci ramek po zmianie protoko�u!!!-------
	
	//      ===  TYLKO TABLICE O WYMIARACH TAKICH JAK RAMKA, NIE WI�KSZE !!!  ===
	
	#define serialPort 1                      // Serial: 0, Serial1: 1, Serial2: 2, Serial3: 3
	#define comSerial Serial1                 // Serial with transceiver
	#define BAUD_RATE 9600                    // With transciever
	#define MAX_SEND_SIZE 56                  // Size of the largest packet (could be higher than needed)

	// Szablon: nadawca_RAMKA_nazwa_SIZE/TYPE
	
	/* DO ZAPROJEKTOWANIA OD NOWA
	
	// -- wysy�ane --
	#define PILOT_RAMKA_STEROWANIE_SIZE 3
	#define PILOT_RAMKA_STEROWANIE_TYPE 0x00

	#define PILOT_RAMKA_DANE_SIZE 4
	#define PILOT_RAMKA_DANE_TYPE 0x01

	#define PILOT_RAMKA_TEST_SIZE 8
	#define PILOT_RAMKA_TEST_TYPE 0x02
	
	#define PILOT_RAMKA_CONFIG_SIZE 23
	#define PILOT_RAMKA_CONFIG_TYPE 0x03
	// -- odebrane --
	#define DRON_RAMKA_POZYCJA_SIZE 6
	#define DRON_RAMKA_POZYCJA_TYPE 0x05

	#define DRON_RAMKA_TEST_SIZE 6
	#define DRON_RAMKA_TEST_TYPE 0x06  */
	
	// times [ms] after vehicle set ... power; eg. after x[ms] high power is set
	#define MP_SET_TIME 1100  // Medium power (21dBm)
	
	#define  RESTORE_CONNECTION_PARAMS_TIME 3000  // Time after signal lost when OTA speed is set to 1.2kbp and smallest packet is used

	#define MAX_LOST_PACKETS 2          // If more -> signal lost mode
	
	// transmission powers
	#define DBM20 4  // 20dBm
	#define DBM14 3  // 14dBm
	#define DBM8 2  // 8dBm
	#define DBM2 1  // 2dBm
	
	
// <<<<<<<<<=====================     PINY     =====================>>>>>>>>>
	
	// Software serial pins for BT module
	#define tx_pin 2
	#define rx_pin 3
	
	#define buzzer 12 // buzzer pin
	
	#define pinThrottle A0;
	#define pinRotate A1;
	#define pinTiltTB A2;
	#define pinTiltLR A3;
	
	#define redDiodePin 5
	#define greenDiodePin 6
	
	
// EWA filters betas
	#define STEERING_FILTER_BETA 0.6
	
	
// ADDRESSES
	#define LCD_ADDRESS 0x3F



#endif /* CONFIG_H_ */