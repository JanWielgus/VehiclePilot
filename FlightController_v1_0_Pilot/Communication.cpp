// Created: 11.01.2018r.
// Author: Jan Wielgus
// 

#include "Communication.h"

CommunicationClass com;

void _protezaOdbierz(const uint8_t* buffer, size_t _size)
{
	com.odbierzPriv(buffer, _size);
}



CommunicationClass::CommunicationClass()
{
	//defaults:
	lost_packets = 10; // domyslnie nie ma polacznenia
}



void CommunicationClass::init()
{
	pSerial.setPacketHandler(_protezaOdbierz);
	pSerial.begin(BAUD_RATE, serialPort);
	
	// transciever deafult settings
	head        = 0xC2;
	addh        = 0x00;
	addl        = 0x00;
	sped.bajt   = 0x18;
	chan.bajt   = 0x50;
	option.bajt = 0x44;
}



void CommunicationClass::odbierz()
{
	if_odbierzPriv = false;
	
	pSerial.update();
	
	// check if connection lost
	if (if_odbierzPriv)
		lost_packets=0;
	else
		lost_packets++;

	if (lost_packets == MAX_LOST_PACKETS) // czas utraty polaczenia
		con_lost_time = millis();
}



void CommunicationClass::odbierzPriv(const uint8_t* bufferR, size_t PacketSize)
{
	// przyklad: if (bufferR[1] == DRON_RAMKA_TEST_TYPE && PacketSize == DRON_RAMKA_TEST_SIZE && sprawdzSumeKontr(bufferR, PacketSize))
	//odbieranie do zmiennych
	
	
	if_odbierzPriv = true;
}



void CommunicationClass::wyslij(uint8_t typRamki)
{
	buforT[1] = typRamki;
	
	/* PRZYKLAD:
	if (typRamki == PILOT_RAMKA_TEST_TYPE)
		buforT[7] = ping.bajt;
		buforT[0] = liczSumeKontr(buforT, PILOT_RAMKA_TEST_SIZE);
		pSerial.send(buforT, PILOT_RAMKA_TEST_SIZE);
	else if (typRamki == PILOT_RAMKA_STEROWANIE_TYPE)*/
	
	// zmienne do bufora
	//w kazdym if'ie bufor[0] i send
	
}



bool CommunicationClass::connectionState()
{
	if (lost_packets < MAX_LOST_PACKETS)
		return true;
	timeAfterSL = millis()-con_lost_time;
	return false; // otherwise
}



bool CommunicationClass::sprawdzSumeKontr(const uint8_t* buffer, size_t PacketSize)
{
	uint8_t suma_kontrolna = buffer[1];
	for(int i=2; i<PacketSize; i++)
	suma_kontrolna ^= buffer[i];	//xor'owanie kolejnych bajt?w

	if(suma_kontrolna==buffer[0]) return true;
	else return false;
}



uint8_t CommunicationClass::liczSumeKontr(const uint8_t* buffer, size_t PacketSize)
{
	uint8_t suma_kontrolna = buffer[1];
	for(int i=2; i<PacketSize; i++)
	suma_kontrolna ^= buffer[i];	//xor'owanie kolejnych bajt?w

	return suma_kontrolna;
}



void CommunicationClass::setTransmitPower(bool b1, bool b2)
{
	if (b1 == 1 && b2 == 1)
	{
		// 18dBm
		option.b6 = 1;
		option.b7 = 1;
	}
	
	else if (b1 == 1 && b2 == 0)
	{
		// 21dBm
		option.b6 = 1;
		option.b7 = 0;
	}
	
	else if (b1 == 0 && b2 == 1)
	{
		// 24dBm
		option.b6 = 0;
		option.b7 = 1;
	}
	
	else
	{
		// 27dBm
		option.b6 = 0;
		option.b7 = 0;
	}
	changeInTxParams = true; // send new params to module
}



// overloaded
void CommunicationClass::setTransmitPower(uint8_t power)
{
	switch (power)
	{
		case DBM2:
			setTransmitPower(1, 1);
			break;
		case DBM8:
			setTransmitPower(1, 0);
			break;
		case DBM14:
			setTransmitPower(0, 1);
			break;
		case DBM20:
			setTransmitPower(0, 0);
			break;
		default:
			setTransmitPower(0, 0);
	}
}



// overloaded
void CommunicationClass::setTransmitPower()
{
	if (connectionState())
	{
		if (switchesR.b0 == 0) // jesli moc ustawia pilot
		{
			// requested transmission power
			setTransmitPower(switchesR.b6, switchesR.b5);
		}
		else
			autoTransmitPower();
	}
	else
	{
		// MAX when signal lost
		setTransmitPower(DBM20); // MAX
	}
}



void CommunicationClass::setOTASpeed(bool b1)
{
	if (b1)
	{
		// 2.4kbps
		sped.b5 = 0;
		sped.b6 = 0;
		sped.b7 = 1;
	}
	
	else
	{
		// 1.2kbps
		sped.b5 = 0;
		sped.b6 = 0;
		sped.b7 = 0;
	}
	changeInTxParams = true;
}


void CommunicationClass::autoTransmitPower()
{
	// Do napisania !!!
	
	setTransmitPower(DBM20); // temp
}



void CommunicationClass::writeParamsToTransceiver()
{
	if (changeInTxParams)
	{
		// write if was change in parameters
		//...
		
		changeInTxParams = false;
	}
	return;
}



void CommunicationClass::getTransceiverParams()
{
	// power
	settingsConfirmation.b7 = switchesR.b6;
	settingsConfirmation.b6 = switchesR.b5;
	
	// working modes
	settingsConfirmation.b5 = switchesR.b4;
	settingsConfirmation.b4 = switchesR.b3;
	
	// OTA speed
	settingsConfirmation.b3 = switchesR.b2;
}
