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
	// VER 1 - pelna
	if (bufferR[1]==DRON_RAMKA_VER1_TYPE && PacketSize==DRON_RAMKA_VER1_SIZE && sprawdzSumeKontr(bufferR, PacketSize))
	{
		for (int i=0; i<6; i++)
			cellVoltage[i] = bufferR[i+2] / 10.0;
		pitch = bufferR[8];
		roll = bufferR[9];
		heading = word(bufferR[11], bufferR[10]);
		altitude = word(bufferR[13], bufferR[12]);
		for (int i=0; i<4; i++)
			pos_longInt.bajt[i] = bufferR[i+14];
		for (int i=0; i<4; i++)
			pos_longInt.bajt[i] = bufferR[i+18];
		randomRxValue = bufferR[22];
		errorList1.bajt = bufferR[23];
		errorList2.bajt = bufferR[24];
		bitsRx1 = bufferR[25];
		// zapas 6 x uint8 (ostatnie [31])
		
		if_odbierzPriv = true;
	}
	
	/*
		VER 2 - podstawowa
	*/
	
	// VER 3 - po uzbrojeniu i PID request
	else if (bufferR[1]==DRON_RAMKA_VER3_TYPE && PacketSize==DRON_RAMKA_VER3_SIZE && sprawdzSumeKontr(bufferR, PacketSize))
	{
		for (int i=0; i<4; i++)
			takeoff.posLongInt.bajt[i] = bufferR[i+2];
		for (int i=0; i<4; i++)
			takeoff.posLatInt.bajt[i] = bufferR[i+6];
		for (int i=0; i<4; i++)
			takeoff.pressure.bajt[i] = bufferR[i+10];
		// zapas 5 x uint8
		pid_params_request = bufferR[19];
		
		// if_odbierzPriv = true; // Tu raczej nie ma znaczenia, musi byc odebrana VER1
	}
}



void CommunicationClass::wyslij(uint8_t typRamki)
{
	buforT[1] = typRamki;
	
	// VER1 - pelna
	if (typRamki == PILOT_RAMKA_VER1_TYPE)
	{
		buforT[2] = lowByte(pilot.throttle); // Ewentualnie this->...
		buforT[3] = highByte(pilot.throttle);
		buforT[4] = lowByte(pilot.rotate);
		buforT[5] = highByte(pilot.rotate);
		buforT[6] = lowByte(pilot.tilt_TB);
		buforT[7] = highByte(pilot.tilt_TB);
		buforT[8] = lowByte(pilot.tilt_LR);
		buforT[9] = highByte(pilot.tilt_LR);
		buforT[10] = lowByte(distanceFromPilot);
		buforT[11] = highByte(distanceFromPilot);
		buforT[12] = lowByte(directionToPilot);
		buforT[13] = highByte(directionToPilot);
		buforT[14] = flightMode;
		buforT[15] = armState;
		buforT[16] = randomTxValue;
		buforT[17] = bitsTx1.bajt;
		buforT[18] = bitsTx2.bajt;
		buforT[19] = signalLostScenario;
		for (int i=0; i<6; i++) // zerowanie zapasu
			buforT[i+20] = 0;
		// zapas 6 x uint8 (ostatnie [25])
		
		buforT[0] = liczSumeKontr(buforT, PILOT_RAMKA_VER1_SIZE);
		pSerial.send(buforT, PILOT_RAMKA_VER1_SIZE);
	}
	
	/*
		VER2 - podstawowa
	*/
	
	// VER3 - parametry PID
	else if (typRamki == PILOT_RAMKA_VER3_TYPE)
	{
		// Leveling
		for (int i=0; i<4; i++)
			buforT[i+2] = conf.levelPID.kP.bajt[i];
		for (int i=0; i<4; i++)
			buforT[i+6] = conf.levelPID.kI.bajt[i];
		for (int i=0; i<4; i++)
			buforT[i+10] = conf.levelPID.kD.bajt[i];
		buforT[14] = conf.levelPID.Imax;
		
		// Yaw
		for (int i=0; i<4; i++)
			buforT[i+15] = conf.yawPID.kP.bajt[i];
		for (int i=0; i<4; i++)
			buforT[i+19] = conf.yawPID.kI.bajt[i];
		for (int i=0; i<4; i++)
			buforT[i+23] = conf.yawPID.kD.bajt[i];
		buforT[27] = conf.yawPID.Imax;
		
		// Alt hold
		for (int i=0; i<4; i++)
			buforT[i+28] = conf.altHoldPID.kP.bajt[i];
		for (int i=0; i<4; i++)
			buforT[i+32] = conf.altHoldPID.kI.bajt[i];
		for (int i=0; i<4; i++)
			buforT[i+36] = conf.altHoldPID.kD.bajt[i];
		buforT[40] = conf.altHoldPID.Imax;
		
		buforT[0] = liczSumeKontr(buforT, PILOT_RAMKA_VER3_SIZE);
		pSerial.send(buforT, PILOT_RAMKA_VER3_SIZE);
	}
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
