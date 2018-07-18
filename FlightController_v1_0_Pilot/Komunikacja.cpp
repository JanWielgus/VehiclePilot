// Author: Jan Wielgus
// Date: 24.10.2017
// 

#include "Komunikacja.h"
KomunikacjaClass kom;

void _protOdbierz(const uint8_t* buffer, size_t size)
{
	kom.odbierzPriv(buffer, size);
}



void KomunikacjaClass::init(SoftwareSerial *_softwareserial)
{
	pSerial.setPacketHandler(_protOdbierz);
	_softwareserial->begin(BAUD_RATE);
	pSerial.begin(_softwareserial);
}



void KomunikacjaClass::odbierz()
{
	pSerial.update();
}



void KomunikacjaClass::odbierzPriv(const uint8_t* bufferR, size_t PacketSize)
{
	if (bufferR[1] == DRON_RAMKA_TEST_TYPE && PacketSize == DRON_RAMKA_TEST_SIZE && sprawdzSumeKontr(bufferR, PacketSize))
	{
		zmiennaTestowa.bajt[0] = bufferR[2];
		zmiennaTestowa.bajt[1] = bufferR[3];
		zmiennaTestowa.bajt[2] = bufferR[4];
		zmiennaTestowa.bajt[3] = bufferR[5];
	}
	
	if (bufferR[1] == DRON_RAMKA_POZYCJA_TYPE && PacketSize == DRON_RAMKA_POZYCJA_SIZE && sprawdzSumeKontr(bufferR, PacketSize))
	{
		/*
		jakisOsiemT = bufferR[2];
		jakisSzesnT = word(bufferR[4], bufferR[3]);
		jakisSzesnT = word(bufferR[6], bufferR[5]);
		jakisOsiemT = bufferR[7];
		jakisOsiemT = bufferR[8];
		jakisOsiemT = bufferR[9];
		jakisOsiemT = bufferR[10];
		*/
	}
}



void KomunikacjaClass::wyslij(uint8_t typRamki)
{
	buforT[1] = typRamki;
	
	if (typRamki == PILOT_RAMKA_TEST_TYPE)
	{
		buforT[2] = lowByte(pilot.throttle);
		buforT[3] = highByte(pilot.throttle);
		buforT[4] = pilot.tilt_TB;
		buforT[5] = pilot.tilt_LR;
		buforT[6] = pilot.rotate;
		buforT[7] = ping.bajt;
		
		buforT[0] = liczSumeKontr(buforT, PILOT_RAMKA_TEST_SIZE);
		pSerial.send(buforT, PILOT_RAMKA_TEST_SIZE);
	}
	
	else if (typRamki == PILOT_RAMKA_STEROWANIE_TYPE)
	{
		//ustawianie zmiennych...
		
		buforT[0] = liczSumeKontr(buforT, PILOT_RAMKA_STEROWANIE_SIZE);
		pSerial.send(buforT, PILOT_RAMKA_STEROWANIE_SIZE);
	}
	
	else if (typRamki == PILOT_RAMKA_CONFIG_TYPE)
	{
		buforT[2] =  conf.kP_level.bajt[0];
		buforT[3] =  conf.kP_level.bajt[1];
		buforT[4] =  conf.kP_level.bajt[2];
		buforT[5] =  conf.kP_level.bajt[3];
		
		buforT[6] =  conf.kI_level.bajt[0];
		buforT[7] =  conf.kI_level.bajt[1];
		buforT[8] =  conf.kI_level.bajt[2];
		buforT[9] =  conf.kI_level.bajt[3];
		
		buforT[10] = conf.kD_level.bajt[0];
		buforT[11] = conf.kD_level.bajt[1];
		buforT[12] = conf.kD_level.bajt[2];
		buforT[13] = conf.kD_level.bajt[3];
		
		buforT[14] = conf.I_level_limiter;
		
		buforT[15] = conf.kP_yaw.bajt[0];
		buforT[16] = conf.kP_yaw.bajt[1];
		buforT[17] = conf.kP_yaw.bajt[2];
		buforT[18] = conf.kP_yaw.bajt[3];
		
		buforT[19] = conf.kD_yaw.bajt[0];
		buforT[20] = conf.kD_yaw.bajt[1];
		buforT[21] = conf.kD_yaw.bajt[2];
		buforT[22] = conf.kD_yaw.bajt[3];
		
		
		buforT[0] = liczSumeKontr(buforT, PILOT_RAMKA_CONFIG_SIZE);
		pSerial.send(buforT, PILOT_RAMKA_CONFIG_SIZE);
	}
}



bool KomunikacjaClass::sprawdzSumeKontr(const uint8_t* buffer, size_t PacketSize)
{
	uint8_t suma_kontrolna = buffer[1];
	for(int i=2; i<PacketSize; i++)
	suma_kontrolna ^= buffer[i];	//xor'owanie kolejnych bajt?w

	if(suma_kontrolna==buffer[0]) return true;
	else return false;
}



uint8_t KomunikacjaClass::liczSumeKontr(const uint8_t* buffer, size_t PacketSize)
{
	uint8_t suma_kontrolna = buffer[1];
	for(int i=2; i<PacketSize; i++)
	suma_kontrolna ^= buffer[i];	//xor'owanie kolejnych bajt?w

	return suma_kontrolna;
}



void KomunikacjaClass::setupConfigPacket()
{
	conf.kP_level.value = 0;
	conf.kI_level.value = 0;
	conf.kD_level.value = 0.0;
	conf.I_level_limiter = 80; // Max: 255
	
	conf.kP_yaw.value = 0;
	conf.kD_yaw.value = 0;
}


