// Communication.h
// Created: 11.01.2018r.
// Author: Jan Wielgus

#ifndef _COMMUNICATION_h
#define _COMMUNICATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <PacketSerial.h>
#include "Sensors.h"
#include "typyBitowe.h"
#include "config.h"

struct pidParams // jesli siê nie kompiluje to daæ stare - wszystkie zmienne oddzielnie lub umieœciæ t¹ strukturê gdzie indziej
{
	floatByte kP; // wzmocnienie P
	floatByte kI; // wzmocnienie I
	floatByte kD; // wzmocnienie D
	uint8_t Imax; // anti-windup I
};


class CommunicationClass
{
// ===== FUNCTIONS & OTHERS =====
	
 private:
	PacketSerial pSerial;
	uint8_t buforT[MAX_SEND_SIZE];
	
 public:
	CommunicationClass();
	void init();
	void odbierz();
	void odbierzPriv(const uint8_t* buffer, size_t size);
	void wyslij(uint8_t typRamki);
	bool connectionState(); // true - is; false - isn't
	
	void setTransmitPower(bool b1, bool b2);        // set transmitting power depending on b1 and b2
	void setTransmitPower(uint8_t power);           // (overloaded) set transmitting power depending on program constans
	void setTransmitPower();                        // (overloaded) set transmit power (decision depending on connection state and request)
	void setOTASpeed(bool b1);                      // set the Over The Air spped (in kbps)
	void autoTransmitPower();                       // if CanSat have to decide automaticly
	//void setDefaultTransceiverSettings();           // set default parameters of transceiver
	
	void writeParamsToTransceiver();                // write parameters to transceiver
	
	//before sending
	void getTransceiverParams();         // set proper bits of current transceiver parameters (NOT READ it from device, only from variables)
	
 private:
	bool sprawdzSumeKontr(const uint8_t* buffer, size_t manipPacketSize); //xor'owanie
	uint8_t liczSumeKontr(const uint8_t* buffer, size_t manipPacketSize);
	
	
// ===== VARIABLES =====
 public:
	// radio paramters
	uint8_t head;
	uint8_t addh;
	uint8_t addl;
	bitByte sped;
	bitByte chan;
	bitByte option;
	
	
	// == SENT ==
		struct //steering
		{
			uint16_t throttle;
			int16_t rotate;           // >0 - right; <0 - left
			int16_t tilt_TB;          // <0 is backward
			int16_t tilt_LR;          // <0 is left
		}pilot;
		uint16_t distanceFromPilot;   // Odleglosc od pilota [decymetr]
		uint16_t directionToPilot;    // Kierunek do pilota [stopnie]
		uint8_t flightMode;             // Tryb lotu
		uint8_t armState;               // Uzbrajanie
		uint8_t randomTxValue;          // Losowa wartosc dla drona	
		bitByte bitsTx1, bitsTx2;       // Bity do wyslania
		uint8_t signalLostScenario;     // Scenariusz utraty zasiegu
		// 6x uint8 zapasu
		
		// Zmienne konfiguracji drona
		struct //configVar
		{
			// Poziomowanie
			pidParams levelPID;
			/*
			floatByte kP_level;          // wzmocnienie P PID'u od poziomu (test: 3)
			floatByte kI_level;          // wzmocnienie I PID'u od poziomu
			floatByte kD_level;          // wzmocnienie D PID'u od poziomu  (test: 0.2)
			uint8_t I_level_limiter;     // Ograniczenie cz³onu ca³kuj¹cego
			*/
		
			// Utrzymanie kierunku
			pidParams yawPID;
			/*
			floatByte kP_yaw;            // wzmocnienie P PID'u od osi z
			floatByte kI_yaw;            // wzmocnienie I PID'u od osi z
			floatByte kD_yaw;            // wzmocnienie D PID'u od osi z
			*/
		
			pidParams altHoldPID;
		}conf;
	
	// == RECEIVED ==
		// ODPOWIEDNIE PRZETWARZANIE ODEBRANYCH ZMIENNYCH W FUNKCJI ODBIERANIA (na przyklad dzielenie przez 10)
		float cellVoltage[6];              // napiecie na poszeczegolnych celach [decy V]
		uint8_t pitch, roll;               // przechylenie i pochylenie drona [stopnie]
		uint16_t heading;                  // heading (kat wzgledem polnocy) [stopnie]
		int16_t altitude;                  // wysokosc [cm]
		int32Byte pos_longInt, pos_latInt; // Dlugosc i szerokosc geograficzna * 10^6? (zobaczyc czy to czy float)
		//float pos_longF, pos_latF;       // alternatywa
		uint8_t randomRxValue;             // Odebrana wartosc losowa
		bitByte errorList1, errorList2;    // Bledy
		bitByte bitsRx1;// bitsRx2;        // Bity odebrane
		// 6 x uint8 zapasu
		
		// Takeoff values
		struct
		{
			int32Byte posLongInt, posLatInt;
			uint32Byte pressure; // [Pa]
		} takeoff;
		// 5x uint8 zapas
		uint8_t pid_params_request;
		
	
	
 private:
	uint16_t lost_packets;       // number of lost packets
	bool if_odbierzPriv;         // true if odbierzPriv was called
	uint32_t con_lost_time;      // time when communication lost
	uint32_t timeAfterSL;        // time after communication was lost (UPDATED WHEN connectionState() called !!!)
	
	bool changeInTxParams = false; // true - byla zmiana w ustawieniach nadajnika i trzba ponownie wyslac parametry
};

extern CommunicationClass com;

#endif

