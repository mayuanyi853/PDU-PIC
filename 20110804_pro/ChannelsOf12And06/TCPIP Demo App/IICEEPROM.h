#ifndef __IICEEPROM_H
#define __IICEEPROM_H


//Function Prototypes

unsigned char LDByteWriteI2C( unsigned char, unsigned char, unsigned char );
unsigned char LDByteReadI2C( unsigned char ControlByte, unsigned char address, unsigned char *data );
unsigned char LDSequentialWriteI2C( unsigned char ControlByte, unsigned char address, unsigned char *wrptr , unsigned char length);
unsigned char LDSequentialReadI2C(unsigned char, unsigned char, unsigned char *, unsigned char);
void IICEEPROM_Init(void);
void StartI2C(void);
void StopI2C(void);
void RestartI2C(void);
void NotAckI2C(void);
unsigned char EEAckPolling(unsigned char control);
void IdleI2C(void);
unsigned char ReadI2C(void);
unsigned char WriteI2C(unsigned char data_out);
unsigned char getstringI2C(unsigned char *rdptr, unsigned char length);
unsigned char putstringI2C( unsigned char *wrptr,unsigned char length);

//********************************************************************
//Constant Definitions
//********************************************************************


#endif