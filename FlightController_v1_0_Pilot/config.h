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


//<<<<<<<<<=====================     KOMUNIKACJA     =====================>>>>>>>>>

	//-------Pami�taj o zmianie wielko�ci ramek po zmianie protoko�u!!!-------
	
	//      ===  TYLKO TABLICE O WYMIARACH TAKICH JAK RAMKA, NIE WI�KSZE !!!  ===
	
	#define BAUD_RATE 9600
	#define MAX_SEND_SIZE 50  // Wielkos� najwi�kszej ramki

	// Szablon: nadawca_RAMKA_nazwa_SIZE/TYPE
	
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
	#define DRON_RAMKA_TEST_TYPE 0x06
	
	
	#define tx_pin 2
	#define rx_pin 3

	#define MAX_ILOSC_ZGUBIONYCH_RAMEK 2    // Po prezkroczeniu wykrywany jest brak po��czenia z pilotem




#endif /* CONFIG_H_ */