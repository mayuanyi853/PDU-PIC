#ifndef _RS485_H_
#define _RS485_H_


#define BAUD_RATE2 9600      //485总线波特率设置

#define Receive			0
#define Send			1

void RS485_Init(void);
void RS485_Putch(BYTE byte);
BYTE RS485_Getch(void);
BYTE RS485_RW(void);
#endif
