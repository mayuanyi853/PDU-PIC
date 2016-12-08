////////////////////////////////////////////////////////////////CCT 这个程序现在没有什么用，不过说一点
//这这个程序的哥们其实水平也不是太高。
#include "TCPIP Stack/TCPIP.h"
#if defined(STACK_USE_BUZZER)
extern BYTE T_1s_Acc;
extern BYTE T_3s_Acc;

extern BOOL b_RF315Key_Press;
extern BOOL b_TMeasure;
extern BOOL b_Flash500ms;
extern BYTE T_100ms_Acc;
static BYTE T_200ms_Acc;
static BYTE T_500ms_Acc;
static BYTE Trigger200_Num;
static BYTE Trigger1000_Num;


extern BOOL b_3s ;

void Timer3_Init(void)
{
	// Timer3
    TMR3H = 0x01;
    TMR3L = 0xAF;
	// Set up the timer interrupt
//	IPR2bits.TMR3IP = 0;		// Low priority
	IPR2bits.TMR3IP = 1;		// CCT How priority 
    PIR2bits.TMR3IF = 0;
    PIE2bits.TMR3IE = 1;		// Enable interrupt
    // Timer3 on, 8-bit, internal timer, 1:8 prescalar
    T3CON = 0x31;
	T_100ms_Acc = 1;
	T_200ms_Acc = 3;
	T_500ms_Acc = 5;
	T_1s_Acc = 7;
	T_3s_Acc = 11;
	Trigger200_Num = 0;
	Trigger1000_Num = 0;
	b_RF315Key_Press = 0;
	b_TMeasure = 0;
	b_Flash500ms = 0;
    b_3s=0;


}

void Timer3_ISR(void)
{
	if(PIR2bits.TMR3IF)	//50ms
	{
		PIR2bits.TMR3IF = 0;
    	TMR3H = 0x01;
		TMR3L = 0xAF;
		T_100ms_Acc++;
		T_200ms_Acc++;
		T_500ms_Acc++;
		T_1s_Acc++;
		T_3s_Acc++;

		if(T_100ms_Acc >= 4)	//use for RF315 key state detect
		{
			T_100ms_Acc = 0;
			b_RF315Key_Press = 0;
		}
		if(T_200ms_Acc >= 2)	//100ms
		{
			T_200ms_Acc = 0;
			if(Trigger200_Num > 0)
			{
				Trigger200_Num--;
	//			SPK_IO = SPK_IO ^ 1;
			}
		}
		if(T_500ms_Acc >= 10)
		{
			T_500ms_Acc = 0;
			b_Flash500ms = b_Flash500ms ^ 1;
		}
		if(T_1s_Acc >= 20)
		{
			T_1s_Acc = 0;
			if(Trigger1000_Num > 0)
			{
				Trigger1000_Num--;
		//		SPK_IO = SPK_IO ^ 1;
			}
		}
		if(T_3s_Acc >= 25)
		{
			T_3s_Acc = 0;
			b_TMeasure = 1;
			b_3s = 1;

		}
	}
}
#endif


