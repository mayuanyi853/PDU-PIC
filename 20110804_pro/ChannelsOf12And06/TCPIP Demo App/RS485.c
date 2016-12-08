#include "TCPIP Stack/TCPIP.h"

#if defined(STACK_USE_RS485)

void RS485_Init(void)
{
	RS485_DIR_IO = Receive;
	RS485_DIR_TRIS = 0;
	RS485_RX_TRIS = 1;
	RS485_TX_TRIS = 0;
	RS485_RX_IO = 0;
	RS485_TX_IO = 1;

    TXSTA2 = 0x20;
    RCSTA2 = 0x90;
	// See if we can use the high baud rate setting
	#if ((GetPeripheralClock()+2*BAUD_RATE2)/BAUD_RATE2/4 - 1) <= 255
		SPBRG2 = (GetPeripheralClock()+2*BAUD_RATE2)/BAUD_RATE2/4 - 1;
		TXSTA2bits.BRGH = 1;
	#else	// Use the low baud rate setting
		SPBRG2 = (GetPeripheralClock()+8*BAUD_RATE2)/BAUD_RATE2/16 - 1;
	#endif

	PIE3bits.RC2IE = 1;
	PIR3bits.RC2IF = 0;
	IPR3bits.RC2IP = 0;
	TXSTA2bits.TXEN = 1;
	RCONbits.IPEN = 1;		// Enable interrupt priorities
	INTCONbits.GIEH = 1;
	INTCONbits.GIEL = 1;
}

void RS485_Putch(BYTE byte)
{

	/* output one byte */
	while(!PIR3bits.TX2IF)	/* set when register is empty */
		continue;
	TXREG2 = byte;
	while(!TXSTA2bits.TRMT)	/* set when register is empty */
		;
}



BYTE RS485_Getch(void)
{
	/* retrieve one byte */
	while(!PIR3bits.RC2IF)	/* set when register is not empty */
		continue;
	PIR3bits.RC2IF = 0;
	return RCREG2;
}

BYTE RS485_RW(void)
{
	BYTE temp;
	RS485_Putch(temp = RS485_Getch());
	return temp;
}
#endif
