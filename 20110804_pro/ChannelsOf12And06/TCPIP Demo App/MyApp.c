#include "TCPIP Stack/TCPIP.h"


extern DS1302_VAL DS1302REG;
extern int TP;

WORD Valid_Add;
BYTE Valid_Data;
extern APP_CONFIG AppConfig;



void PerDevice_Init(void)
{
#if defined(STACK_USE_DS1302)
	DS1302_Init();
#endif

#if defined(STACK_USE_DS18B20)
	DS18B20_Init();
#endif
 	s_connectionreset();
#if defined(STACK_USE_IICEEPROM)
	IICEEPROM_Init();
#endif

#if defined(STACK_USE_RF315)
	RF315_Init();
#endif

#if defined(STACK_USE_BUZZER)
	Timer3_Init();
#endif

#if defined(STACK_USE_RS485)
	RS485_Init();
#endif
}

#if defined(STACK_USE_DS1302)

void DisplayTime(void)
{
//	DS1302_VAL DS1302REG;
	BYTE i;
	ReadRTC(&DS1302REG);
	for(i = 0; i < 16; i++)
	{
		LCDText[i] = ' ';
	}
	LCDText[1] = 48 + (DS1302REG.YEAR >> 4);
	LCDText[2] = 48 + (DS1302REG.YEAR & 0x0f);
	LCDText[3] = '-';
	LCDText[4] = 48 + (DS1302REG.MONTH >>4);
	LCDText[5] = 48 + (DS1302REG.MONTH & 0x0f);
	LCDText[6] = '-';
	LCDText[7] = 48 + (DS1302REG.DATE >>4);
	LCDText[8] = 48 + (DS1302REG.DATE & 0x0f);

	LCDText[11] = 48 + (DS1302REG.HR >> 4);
	LCDText[12] = 48 + (DS1302REG.HR & 0x0f);
	LCDText[13] = ':';
	LCDText[14] = 48 + (DS1302REG.MIN >> 4);
	LCDText[15] = 48 + (DS1302REG.MIN & 0x0f);

	BCD2Hex(&DS1302REG,8);
}
#endif

#if defined(STACK_USE_DS18B20)
void DisplayTemperature(void)
{
	char TPString[8];
	int TP_temp;
	INTCONbits.RBIF = 0;	
	INTCONbits.RBIE = 0;
	Read_Temperature();
	TP_temp = TP;
	if(TP_temp & 0xf800l)
	{
		TP_temp = ~TP_temp + 1;
		TP_temp = TP_temp >> 4;
		TPString[0] = '-';
    	uitoa(TP_temp, &TPString[1]);
		LCDText[29] = TPString[0];
		LCDText[30] = TPString[1];
		LCDText[31] = TPString[2];
	}
	else
	{
		TP_temp = TP_temp >> 4;
		TPString[0] = ' ';
    	uitoa(TP_temp, &TPString[1]);
		LCDText[29] = TPString[0];
		LCDText[30] = TPString[1];
		LCDText[31] = TPString[2];
	}
	Convert_T();
}
#endif

#if defined(STACK_USE_IICEEPROM)
void BuildDateSave(void)
{
	const char NewBuildDate[]={__DATE__ __TIME__};
	BOOL b_different;
	struct
	{
 		char BuildDate[19];
   	 	DWORD_VAL BuildCount;
	}EEPROM;
	char i;
	b_different = 0;
	LDSequentialReadI2C(EEPROM_ADD,0x20,&EEPROM.BuildCount.v[0],4);			//BuildCount
	LDSequentialReadI2C(EEPROM_ADD,0x28,&EEPROM.BuildDate[0],8);			//
	LDSequentialReadI2C(EEPROM_ADD,0x30,&EEPROM.BuildDate[8],8);			//
	LDSequentialReadI2C(EEPROM_ADD,0x38,&EEPROM.BuildDate[16],3);			//
	for(i=0;i<19;i++)
	{
 		if(EEPROM.BuildDate[i] != NewBuildDate[i])
 		{
  			EEPROM.BuildDate[i] = NewBuildDate[i];
			b_different = 1;
		}
	}
	if( b_different )
	{
		EEPROM.BuildCount.Val++;
		LDSequentialWriteI2C(EEPROM_ADD, 0x20, &EEPROM.BuildCount.v[0], 4);
		while (EEAckPolling(EEPROM_ADD));  //Wait for write cycle to complete
		LDSequentialWriteI2C(EEPROM_ADD,0x28,&EEPROM.BuildDate[0],8);			//
		while (EEAckPolling(EEPROM_ADD));  //Wait for write cycle to complete
		LDSequentialWriteI2C(EEPROM_ADD,0x30,&EEPROM.BuildDate[8],8);			//
		while (EEAckPolling(EEPROM_ADD));  //Wait for write cycle to complete
		LDSequentialWriteI2C(EEPROM_ADD,0x38,&EEPROM.BuildDate[16],3);			//
		while (EEAckPolling(EEPROM_ADD));  //Wait for write cycle to complete
	}

}

void UploadBuildDate(void)
{
	struct
	{
 		char BuildDate[19];
   	 	DWORD_VAL BuildCount;
	}EEPROM;
	char i;
	LDSequentialReadI2C(EEPROM_ADD,0x20,&EEPROM.BuildCount.v[0],4);			//BuildCount
	LDSequentialReadI2C(EEPROM_ADD,0x28,&EEPROM.BuildDate[0],8);			//
	LDSequentialReadI2C(EEPROM_ADD,0x30,&EEPROM.BuildDate[8],8);			//
	LDSequentialReadI2C(EEPROM_ADD,0x38,&EEPROM.BuildDate[16],3);			//
	for(i = 0; i < 4; i++)
	{
		RS485_Putch(EEPROM.BuildCount.v[i]);
	}
	for(i = 0; i < 19; i++)
	{
		RS485_Putch(EEPROM.BuildDate[i]);
	}
}

#endif











