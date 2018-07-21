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
	uint8_t head;
	uint8_t addh;
	uint8_t addl;
	bitByte sped;
	bitByte chan;
	bitByte option;
	
	uint32_t timeAfterSL; // time after communication was lost (UPDATED AFTER connectionState() !!!)
	
	
	// == SENT ==
	//bitByte settingsConfirmation;        // booleany steruj¹ce
	bitByte bitsT1;                        // Bity do wyslania 1
	bitByte bitsT2;                        // Bity do wyslania 2
	struct steering
	{
		uint16_t throttle;
		int16_t tilt_TB; // <0 is backward
		int16_t tilt_LR; // <0 is left
		int16_t rotate; // >0 - right; <0 - left
	}pilot;
	// ...
	
	// Zmienne konfiguracji drona
	struct configVar
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
	//bitByte switchesR;                   // przelaczniki
	bitByte bitsR1;                        // Odebrane bity 1
	bitByte bitsR2;                        // Odebrane bity 2
	// ...
	
	
 private:
	uint16_t lost_packets;       // number of lost packets
	bool if_odbierzPriv;         // true if odbierzPriv was called
	uint32_t con_lost_time;      // time when communication lost
	
	bool changeInTxParams = false; // true - byla zmiana w ustawieniach nadajnika i trzba ponownie wyslac parametry
};

extern CommunicationClass com;

#endif

