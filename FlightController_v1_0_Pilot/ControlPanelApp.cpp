// Author: Jan Wielgus
// Date: 19.02.2018
// 

#include "ControlPanelApp.h"

ControlPanelAppClass cpa;


void _protOdbierzUSB(const uint8_t* buffer, size_t size)
{
	cpa.odbierzPriv(buffer, size);
}


void ControlPanelAppClass::init()
{
	pSerialUSB.setPacketHandler(_protOdbierzUSB);
	pSerialUSB.begin(9600);
}


void ControlPanelAppClass::odbierz()
{
	pSerialUSB.update();
}


void ControlPanelAppClass::odbierzPriv(const uint8_t* bufferR, size_t PacketSize)
{
	if (bufferR[1]==KOMUN_RAMKA_PC_PARAMETERS_TYPE && PacketSize==KOMUN_RAMKA_PC_PARAMETERS_SIZE && sprawdzSumeKontr(bufferR, PacketSize))
	{
		kom.conf.kP_level.bajt[0] = bufferR[2];
		kom.conf.kP_level.bajt[1] = bufferR[3];
		kom.conf.kP_level.bajt[2] = bufferR[4];
		kom.conf.kP_level.bajt[3] = bufferR[5];
		
		kom.conf.kI_level.bajt[0] = bufferR[6];
		kom.conf.kI_level.bajt[1] = bufferR[7];
		kom.conf.kI_level.bajt[2] = bufferR[8];
		kom.conf.kI_level.bajt[3] = bufferR[9];
		
		kom.conf.kD_level.bajt[0] = bufferR[10];
		kom.conf.kD_level.bajt[1] = bufferR[11];
		kom.conf.kD_level.bajt[2] = bufferR[12];
		kom.conf.kD_level.bajt[3] = bufferR[13];
		
		kom.conf.I_level_limiter = bufferR[14];
		
		kom.conf.kP_yaw.bajt[0] = bufferR[15];
		kom.conf.kP_yaw.bajt[1] = bufferR[16];
		kom.conf.kP_yaw.bajt[2] = bufferR[17];
		kom.conf.kP_yaw.bajt[3] = bufferR[18];
		
		kom.conf.kD_yaw.bajt[0] = bufferR[19];
		kom.conf.kD_yaw.bajt[1] = bufferR[20];
		kom.conf.kD_yaw.bajt[2] = bufferR[21];
		kom.conf.kD_yaw.bajt[3] = bufferR[22];
		
		kom.wyslij(PILOT_RAMKA_CONFIG_TYPE);
	}
}


void ControlPanelAppClass::wyslij(uint8_t typRamki)
{
	buforT[1] = typRamki;
	
	if (typRamki == KOMUN_RAMKA_ARDU_LIVE_TYPE)
	{
		//buforT[2] = --;
		buforT[3] = throttlePCapp;
		buforT[4] = kom.pilot.rotate;
		buforT[5] = kom.pilot.tilt_TB;
		buforT[6] = kom.pilot.tilt_LR;
		buforT[7] = 0;
		buforT[8] = 0;
		buforT[9] = 0;
		buforT[10] = 0;
		
		buforT[0] = liczSumeKontr(buforT, KOMUN_RAMKA_ARDU_LIVE_SIZE);
		pSerialUSB.send(buforT, KOMUN_RAMKA_ARDU_LIVE_SIZE);
	}
}


bool ControlPanelAppClass::sprawdzSumeKontr(const uint8_t* buffer, size_t PacketSize)
{
	uint8_t suma_kontrolna = buffer[1];
	for(int i=2; i<PacketSize; i++)
	suma_kontrolna ^= buffer[i];	//xor'owanie kolejnych bajt?w

	if(suma_kontrolna==buffer[0]) return true;
	else return false;
}


uint8_t ControlPanelAppClass::liczSumeKontr(const uint8_t* buffer, size_t PacketSize)
{
	uint8_t suma_kontrolna = buffer[1];
	for(int i=2; i<PacketSize; i++)
	suma_kontrolna ^= buffer[i];	//xor'owanie kolejnych bajt?w

	return suma_kontrolna;
}



