#include "TCPIP Stack/TCPIP.h"

#if defined(STACK_USE_RF315)
WORD Current_Add;
BYTE Current_Data;
extern WORD Valid_Add;
extern BYTE Valid_Data;
extern BOOL b_RF315_finish;
extern BYTE T_100ms_Acc;
extern BOOL b_RF315Key_Press;
extern BOOL b_SPK_Success;
extern BOOL b_SPK_Fail;
extern BOOL b_SPK_Attack;
extern BOOL b_Flash500ms;

static BOOL IOState_Old;
static BOOL IOState_New;
static BYTE RF_Step;
static BYTE Data_Sum;
static BYTE RF_Data;
static WORD RF_Address;
static WORD wd_T1,wd_T2,wd_T3,wd_T4;
static BOOL b_P1,b_P2,b_P3,b_P4;
static BYTE Standby_Add[20] = {0};
static BYTE Add_Num;

//////////////////////////////////////////////////////////////////////
//Function void RF315_Init(void)
//Intput:
//			NULL
//
//Output:	NULL
//			
//Overview: RF315接收功能的相应寄存器初始化，从EEPROM读入保存的遥控器地址
void RF315_Init(void)	
{
	RF315_RXD_TRIS = 1;
	RF315_RXD_IO = 1;
	RF_Step = 0;
	Data_Sum = 0;
	RF_Address = 0;
	RF_Data = 0;
	b_RF315_finish = 0;
	//	Interrupt_on_Change
	INTCON2bits.RBPU = 0;
	INTCONbits.RBIF = 0;	
	INTCON2bits.RBIP = 0;
	INTCONbits.RBIE = 1;
	// Timer1
    TMR1H = 0;
    TMR1L = 0;
	// Set up the timer interrupt
	IPR1bits.TMR1IP = 0;		// Low priority
    PIR1bits.TMR1IF = 0;
    PIE1bits.TMR1IE = 0;		// Disable interrupt
    // Timer1 on, 8-bit, internal timer, 1:8 prescalar
    T1CON = 0x31;
	IOState_Old = 0;
	IOState_New = 0;
	//Read standby address from 24c02
	LDByteReadI2C(EEPROM_ADD,0x00,&Add_Num);					//add number
	LDSequentialReadI2C(EEPROM_ADD,0x08,Standby_Add,8);			//add1~add4
	LDSequentialReadI2C(EEPROM_ADD,0x10,&Standby_Add[8],8);		//add5~add8
	LDSequentialReadI2C(EEPROM_ADD,0x18,&Standby_Add[16],4);	//add9,add10
}
//////////////////////////////////////////////////////////////////////
//Function void RF315_ISR(void)
//Intput:
//			NULL
//
//Output:	NULL
//			
//Overview: RF315信号使MCU进入电平变换中断，根据脉宽长度解析RF315数据
void RF315_ISR(void)
{
	if(INTCONbits.RBIF)
	{
		IOState_New = RF315_RXD_IO;
		INTCONbits.RBIF = 0;
		switch(RF_Step)
		{
			case RF_HOME:
				if(IOState_Old == 0 && IOState_New == 1)	//fall
				{
					IOState_Old = IOState_New;
					PIR1bits.TMR1IF = 0;
    				TMR1H = 0;
    				TMR1L = 0;
				}
				else if(IOState_Old == 1 && IOState_New == 0)	//rise
				{
					IOState_Old = IOState_New;
					if(TMR1H > 0x25 && PIR1bits.TMR1IF == 0)	//synchronization code detected
					{
						TMR1H = 0;
    					TMR1L = 0;
						wd_T1 = wd_T2 = wd_T3 = wd_T4 = 0; 
						Data_Sum = 0;
						RF_Address = 0;
						RF_Data = 0;
						RF_Step++;
					}
				}
				break;
			case RF_FIRST:
				wd_T1 = TMR1H;
				wd_T1 = wd_T1 << 8;
				wd_T1 |= TMR1L;
				if(wd_T1 < 250 || wd_T1 > 1800)
				{
					RF_Step = RF_HOME;
				}
				else		//available Pulse detected
				{
					if(wd_T1 > 900)
						b_P1 = 1;
					else
						b_P1 = 0;
					RF_Step++;
				}
				IOState_Old = IOState_New;
				break;
			case RF_SECOND:
				wd_T2 = TMR1H;
				wd_T2 = wd_T2 <<8;
				wd_T2 |= TMR1L;
				wd_T2 = wd_T2 - wd_T1;
				if(wd_T2 < 250 || wd_T2 > 1800)
				{
					RF_Step = RF_HOME;
				}
				else		//available Pulse detected
				{
					if(wd_T2 > 900)
						b_P2 = 1;
					else
						b_P2 = 0;
					RF_Step++;
				}
				IOState_Old = IOState_New;
				break;
			case RF_THIRD:
				wd_T3 = TMR1H;
				wd_T3 = wd_T3 << 8;
				wd_T3 |= TMR1L;
				wd_T3 = wd_T3 - wd_T2;
				wd_T3 = wd_T3- wd_T1;
				if(wd_T3 < 250 || wd_T3 > 1800)
				{
					RF_Step = RF_HOME;
				}
				else		//available Pulse detected
				{
					if(wd_T3 > 900)
						b_P3 = 1;
					else
						b_P3 = 0;
					RF_Step++;
				}
				IOState_Old = IOState_New;
				break;
			case RF_FOURTH:
				wd_T4 = TMR1H;
				wd_T4 = wd_T4 <<8;
				wd_T4 |= TMR1L;
    			TMR1H = 0;
    			TMR1L = 0;
				wd_T4 = wd_T4 - wd_T3 - wd_T2 - wd_T1;
				if(wd_T4 < 250 || wd_T4 > 1800)
				{
					RF_Step = RF_HOME;
				}
				else		//available Pulse detected
				{
					if(wd_T4 > 900)
						b_P4 = 1;
					else
						b_P4 = 0;
					if(b_P1 == 0 && b_P2 == 1 && b_P3 == 0 && b_P4 == 1)		//0
					{

					}
					else if(b_P1 == 1 && b_P2 == 0 && b_P3 == 1 && b_P4 == 0)	//1
					{
						if(Data_Sum < 8)
						{
							RF_Address |= (0x0003l << (2*Data_Sum));	// 0x0003 左移2位，注最后一个为字母l，表示word
						}
						else
						{
							RF_Data |= (0x03 << ( 2*(Data_Sum - 8)));	// 0x03 左移2位，
						}						
					}
					else if(b_P1 == 0 && b_P2 == 1 && b_P3 == 1 && b_P4 == 0)	//f
					{
						if(Data_Sum < 8)
						{
							RF_Address |= (0x0001l << (2*Data_Sum));
						}
						else
						{
							RF_Data |= (0x01 << ( 2*(Data_Sum - 8)));
						}	
					}
					else														//bad data
						RF_Step = RF_HOME;
				}
				if(Data_Sum < 11)
				{
					Data_Sum++;
					RF_Step = RF_FIRST;
				}
				else		//success
				{
					Current_Add = RF_Address;
					T_100ms_Acc = 0;	//if it does not receive new RF315 signal,b_RF315Key_Press will be cleared after 100ms
					if( !b_RF315Key_Press )	//0x5014
					{
						b_RF315Key_Press = 1;
						b_RF315_finish = 1;
						Current_Data = RF_Data;
						RF_Step = RF_HOME;
					}
				}
				IOState_Old = IOState_New;
				break;
			default :
				RF_Step = RF_HOME;
				break;
		}
	}
}
//////////////////////////////////////////////////////////////////////
//Function unsigned char Match_Address(unsigned int address)
//Intput:
//			遥控器地址
//
//Output:
//			0：该遥控器地址没学习过
//			1：该遥控器地址学习过
//Overview: 判断该遥控器地址是否已学习过
BYTE Match_Address(WORD address)
{
	BYTE i;
	BYTE Add_H,Add_L;
	Add_L = address & 0x00ffl;
	Add_H = address >> 8;
	for(i = 0; i < Add_Num; i++)
	{
		if( Add_H == Standby_Add[2 * i] && Add_L == Standby_Add[2 * i + 1])
			return ( 1 );
	}
	return ( 0 );
}
//////////////////////////////////////////////////////////////////////
//Function void Study_Address(WORD address)
//Intput:
//			遥控器地址
//
//Output:	NULL
//	
//Overview: 学习该遥控器地址
void Study_Address(WORD address)
{
	BYTE Address[2];
	Address[1] = address & 0x00ffl;
	Address[0] = address >> 8;
	LDSequentialWriteI2C(EEPROM_ADD, 0x08 + Add_Num * 2, Address, 2);
	while (EEAckPolling(EEPROM_ADD));  //Wait for write cycle to complete
	Add_Num++;
	if(Add_Num > 9)
	{
		Add_Num = 1;
	}
	LDByteWriteI2C(EEPROM_ADD,0x00,Add_Num);
	while (EEAckPolling(EEPROM_ADD));  //Wait for write cycle to complete
}
//////////////////////////////////////////////////////////////////////
//Function void Search_NewAddress(void)
//Intput:
//			NULL
//
//Output:	NULL
//	
//Overview: 搜索遥控器，并将学习新的遥控器地址
void Search_NewAddress(void)
{
	while(!b_RF315_finish && BUTTON0_IO && BUTTON2_IO && BUTTON3_IO)
	{
 		LED_PUT( 0xff * b_Flash500ms );
	}
	if( !Match_Address(Current_Add) )	//0x5014
	{
		Study_Address(Current_Add);
		Reset();
	}
	b_RF315Key_Press = 0;
	b_RF315_finish = 0;
}
//////////////////////////////////////////////////////////////////////
//Function void RF315_Function(void)
//Intput:
//			NULL
//
//Output:	NULL
//	
//Overview: 遥控器RF315信号接收完毕后执行的相应操作
void RF315_Function(void)
{
	if(b_RF315_finish)
	{
		b_RF315_finish = 0;
		if( Match_Address(Current_Add) )	//0x5555
		{
			Valid_Add = Current_Add;
			Valid_Data = Current_Data;
			switch(Current_Data)
			{
				case 0x03: RELAY1_IO = RELAY1_IO ^ 1; b_SPK_Attack = 1; break;	//按键A
				case 0x0c: RELAY2_IO = RELAY2_IO ^ 1; b_SPK_Attack = 1; break;	//按键B
				case 0x30: RELAY1_IO = 0; b_SPK_Attack = 1;  break;				//按键C
				case 0xc0: RELAY2_IO = 0; b_SPK_Attack = 1; break;				//按键D
				default	 : b_SPK_Fail = 1; break;
			}
		}
		else
			b_RF315Key_Press = 0;
	}
}

#endif
