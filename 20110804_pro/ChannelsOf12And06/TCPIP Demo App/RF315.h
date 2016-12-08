#ifndef __RF315_H
#define __RF315_H

#define RF_HOME				0
#define RF_FIRST			1
#define RF_SECOND			2
#define RF_THIRD			3
#define RF_FOURTH			4

#define EEPROM_ADD			0xa0

void RF315_Init(void);
void RF315_ISR(void);
BYTE Match_Address(WORD address);
void Study_Address(WORD address);
void RF315_Function(void);
void Search_NewAddress(void);
#endif