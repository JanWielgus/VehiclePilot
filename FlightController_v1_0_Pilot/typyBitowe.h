/*
 * typyBitowe.h
 *
 * Created: 12.03.2017 10:39:14
 *  Author: Filip
 */ 


#ifndef TYPYBITOWE_H_
#define TYPYBITOWE_H_


union bitByte
{
	uint8_t bajt;
	struct
	{
		bool b7	:1;
		bool b6	:1;
		bool b5	:1;
		bool b4	:1;
		bool b3	:1;
		bool b2	:1;
		bool b1	:1;
		bool b0	:1;
	};
};


union floatByte
{
	float value;
	char bajt[4];
};

//////////////////////////////////////////////////////////////////////////

union int32Byte
{
	int32_t value;
	char bajt[4];
};


union uint32Byte
{
	uint32_t value;
	char bajt[4];
};


union int16Byte
{
	int16_t value;
	char bajt[2];
};


union uint16Byte
{
	uint16_t value;
	char bajt[2];
};


#endif /* TYPYBITOWE_H_ */