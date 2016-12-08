/*********************************************************************
 *
 *       LCD Access Routines
 *
 *********************************************************************
 * FileName:        LCDBlocking.c
 * Dependencies:    None
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.00 or higher
 *					Microchip C30 v3.01 or higher
 *					Microchip C18 v3.13 or higher
 *					HI-TECH PICC-18 STD 9.50PL3 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2008 Microchip Technology Inc.  All rights 
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and 
 * distribute: 
 * (i)  the Software when embedded on a Microchip microcontroller or 
 *      digital signal controller product ("Device") which is 
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c and 
 *      ENC28J60.h ported to a non-Microchip device used in 
 *      conjunction with a Microchip ethernet controller for the 
 *      sole purpose of interfacing with the ethernet controller. 
 *
 * You should refer to the license agreement accompanying this 
 * Software for additional information regarding your rights and 
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT 
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT 
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A 
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL 
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR 
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF 
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS 
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE 
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER 
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT 
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date         Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder     4/03/06		Original
 * Howard Schlunder     4/12/06		Changed from using PMP to LCDWrite()
 * Howard Schlunder		8/10/06		Fixed a delay being too short 
 *									when CLOCK_FREQ was a smaller 
 *									value, added FOUR_BIT_MODE
 ********************************************************************/
#define __LCDBLOCKING_C

#include "TCPIP Stack/TCPIP.h"

#if defined(POWER_SUPPLY_12)
BYTE LCDText[16*4];
#elif defined(POWER_SUPPLY_6) || defined(POWER_SUPPLY_12_1602)
BYTE LCDText[32];
#endif

extern BYTE AVs[12][4];


#if defined(POWER_SUPPLY_12)
/******************************************************************************
 * Function:        static void LCDWrite(BYTE RS, BYTE Data)
 *
 * PreCondition:    None
 *
 * Input:           RS - Register Select - 1:RAM, 0:Config registers
 *					Data - 8 bits of data to write
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Controls the Port I/O pins to cause an LCD write
 *
 * Note:            None
 *****************************************************************************/
static void LCDWrite(BYTE RS, BYTE Data)
{
	LCD_RS_IO = RS;
	LCD_RD_WR_IO = 0;
	LCD_DATA_IO = Data;	     				// Wait Data setup time (min 40ns)
	Nop();
	LCD_E_IO = 1;

	Nop();					// Wait E Pulse width time (min 230ns)
	Nop();
	Nop();
	LCD_E_IO = 0;
}
/******************************************************************************
 * Function:        void LCDInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        LCDText[] is blanked, port I/O pin TRIS registers are 
 *					configured, and the LCD is placed in the default state
 *
 * Note:            None
 *****************************************************************************/
void LCDInit(void)
{
	BYTE i;

	memset(LCDText, ' ', sizeof(LCDText)-1);
	LCDText[sizeof(LCDText)-1] = 0;
	LCD_E_TRIS = 0;
	LCD_RD_WR_TRIS = 0;
	LCD_DATA_TRIS = 0x00;
	LCD_BL_TRIS = 0;	//LCD背光灯控制
	LCD_RS_TRIS = 0;

	LCD_E_IO = 0;
	LCD_RD_WR_IO = 0;
	LCD_RS_IO=0;	
	LCD_BL_IO = 1;	//背光灯打开
	
	LCDWrite(0, 0x30);
	Delay10us(150);
	LCDWrite(0, 0x0e);	
	Delay10us(150);
	LCDWrite(0, 0x01);	
	DelayMs(10);
	LCDWrite(0, 0x06);
	Delay10us(150);	
}


/******************************************************************************
 * Function:        void LCDUpdate(void)
 *
 * PreCondition:    LCDInit() must have been called once
 *
 * Input:           LCDText[]
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Copies the contents of the local LCDText[] array into the 
 *					LCD's internal display buffer.  Null terminators in 
 *					LCDText[] terminate the current line, so strings may be 
 *					printed directly to LCDText[].
 *
 * Note:            None
 *****************************************************************************/
void LCDUpdate(int b)     
{
	BYTE i, j;
	LCDWrite(0, 0x01);
	DelayMs(5);
	// Output first line

	if(b>=1)
 {
	LCDWrite(0, 0x80);
	Delay10us(10);
	for(i = 0; i < 16u; i++)
	{
		// Erase the rest of the line if a null char is 
		// encountered (good for printing strings directly)
		if(LCDText[i] == 0u)
		{
			for(j=i; j < 16u; j++)
			{
				LCDText[j] = ' ';
			}
		}
		Delay10us(10);
		LCDWrite(1, LCDText[i]);

	}
		Delay10us(10);
 }

	if(b>=2)
 {
	LCDWrite(0, 0x90);
	Delay10us(9);
	for(i = 16; i < 32u; i++)
	{
		// Erase the rest of the line if a null char is 
		// encountered (good for printing strings directly)
		if(LCDText[i] == 0u)
		{
			for(j=i; j < 32u; j++)
			{
				LCDText[j] = ' ';
			}
		}
	Delay10us(10);
		LCDWrite(1, LCDText[i]);
	}
	Delay10us(10);
 }
	if(b>=3)
 { 
	LCDWrite(0, 0x88);
		Delay10us(10);
	for(i = 32; i < 48u; i++)
	{
		// Erase the rest of the line if a null char is 
		// encountered (good for printing strings directly)
		if(LCDText[i] == 0u)
		{
			for(j=i; j < 48u; j++)
			{
				LCDText[j] = ' ';
			}
		}
		Delay10us(10);
		LCDWrite(1, LCDText[i]);

	}
		Delay10us(10);
 }
	if(b>=4)
 {
	LCDWrite(0, 0x98);
		Delay10us(10);
	for(i = 48; i < 64u; i++)
	{
		// Erase the rest of the line if a null char is 
		// encountered (good for printing strings directly)
		if(LCDText[i] == 0u)
		{
			for(j=i; j < 64u; j++)
			{
				LCDText[j] = ' ';
			}
		}
		Delay10us(10);
		LCDWrite(1, LCDText[i]);
	}
 }		
}

/******************************************************************************
 * Function:        void LCDErase(void)
 *
 * PreCondition:    LCDInit() must have been called once
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Clears LCDText[] and the LCD's internal display buffer
 *
 * Note:            None
 *****************************************************************************/
void LCDErase(void)
{
	// Clear display
	LCDWrite(0, 0x01);
	DelayMs(5);

	// Clear local copy
	memset(LCDText, ' ', 64);
}

////////////////////////////////////////////////////////////////cct十六进制转换10进制 2 CHAR TO 1 INT 
WORD HtoD(BYTE h,BYTE l)						
{
	WORD sum = 0;
	sum = h;
	sum<<=8;
	sum+=l;
	return sum;
}

///////////////////////////////////////////////////////////////////cct 把AVs 中的数据显示到液晶屏上
void LCDshowAV(void)						
{
	static int temp = 0;
	WORD sum = 0,add16,a;
    BYTE ctemp ;
	memset(LCDText, ' ', 64);
  for(a=0;a<=3;a++)                      //CCT 这地方没有写好。
  {

	if(a==0)
		add16=0;
	else if(a==1)
		add16=16;
	else if(a==2)
		add16=32;
	else if(a==3)
		add16=48;
	if(temp<9)
	{	
		LCDText[0+add16] = ' ';
		LCDText[1+add16] = (BYTE)((temp%10)+'1');	
	}
	else
    {
		LCDText[0+add16] = '1';
		if(temp==9)
			LCDText[1+add16] ='0';
		else	
			LCDText[1+add16] =(BYTE)(temp%10)+'1';
	}
	LCDText[2+add16] = 0xc2;
	LCDText[3+add16] = 0xb7; 

	sum = HtoD(AVs[temp][0],AVs[temp][1]);

	ctemp=sum/10000;
	sum = sum %10000;
	ctemp+='0';
	if(ctemp == '0')
		LCDText[4+add16] = ' ' ;
	else
		LCDText[4+add16] = ctemp;
	ctemp=sum/1000;
	sum = sum %1000;
	ctemp+='0';
	if(ctemp == '0'&&LCDText[4+add16]==' ')
		LCDText[5+add16] = ' ' ;
	else
		LCDText[5+add16] = ctemp;
	ctemp=sum/100;
	sum = sum %100;
	ctemp+='0';		
	LCDText[6+add16] = ctemp;
	LCDText[7+add16] = '.';
	ctemp=sum/10;
	sum = sum %10;
	ctemp+='0';		
	LCDText[8+add16] = ctemp;


	LCDText[9+add16] = 'V';

	sum = HtoD(AVs[temp][2],AVs[temp][3]);

	ctemp=sum/10000;
	sum = sum %10000;

	ctemp=sum/1000;
	sum = sum %1000;
	ctemp+='0';
	if(ctemp == '0')
		LCDText[10+add16] = ' ';
	else
		LCDText[10+add16] = ctemp;

	ctemp=sum/100;
	sum = sum %100;
	ctemp+='0';
	LCDText[11+add16] = ctemp;	

	LCDText[12+add16] = '.';

	ctemp=sum/10;
	sum = sum %10;
	ctemp+='0';		
	LCDText[13+add16] = ctemp;
	
	LCDText[14+add16] = sum+'0`';
	LCDText[15+add16] = 'A';

	temp++;
	if(temp==12)
		temp=0;
  }
	LCDUpdate(4);
}

#elif defined(POWER_SUPPLY_6)||defined(POWER_SUPPLY_12_1602)
/******************************************************************************
 * Function:        static void LCDWrite(BYTE RS, BYTE Data)
 *
 * PreCondition:    None
 *
 * Input:           RS - Register Select - 1:RAM, 0:Config registers
 *					Data - 8 bits of data to write
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Controls the Port I/O pins to cause an LCD write
 *
 * Note:            None
 *****************************************************************************/
static void LCDWrite(BYTE RS, BYTE Data)
{
	#if defined(LCD_DATA_TRIS)
		LCD_DATA_TRIS = 0x00;
	#else
		LCD_DATA0_TRIS = 0;
		LCD_DATA1_TRIS = 0;
		LCD_DATA2_TRIS = 0;
		LCD_DATA3_TRIS = 0;
		#if !defined(FOUR_BIT_MODE)
		LCD_DATA4_TRIS = 0;
		LCD_DATA5_TRIS = 0;
		LCD_DATA6_TRIS = 0;
		LCD_DATA7_TRIS = 0;
		#endif
	#endif
	LCD_RS_TRIS = 0;
	LCD_RD_WR_TRIS = 0;
	LCD_RD_WR_IO = 0;
	LCD_RS_IO = RS;

#if defined(FOUR_BIT_MODE)
	#if defined(LCD_DATA_IO)
		LCD_DATA_IO = Data>>4;
	#else
		LCD_DATA0_IO = Data & 0x10;
		LCD_DATA1_IO = Data & 0x20;
		LCD_DATA2_IO = Data & 0x40;
		LCD_DATA3_IO = Data & 0x80;
	#endif
	Nop();					// Wait Data setup time (min 40ns)
	Nop();
	LCD_E_IO = 1;
	Nop();					// Wait E Pulse width time (min 230ns)
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	LCD_E_IO = 0;
#endif

	#if defined(LCD_DATA_IO)
		LCD_DATA_IO = Data;
	#else
		LCD_DATA0_IO = ((Data & 0x01) == 0x01);
		LCD_DATA1_IO = ((Data & 0x02) == 0x02);
		LCD_DATA2_IO = ((Data & 0x04) == 0x04);
		LCD_DATA3_IO = ((Data & 0x08) == 0x08);
		#if !defined(FOUR_BIT_MODE)
		LCD_DATA4_IO = ((Data & 0x10) == 0x10);
		LCD_DATA5_IO = ((Data & 0x20) == 0x20);
		LCD_DATA6_IO = ((Data & 0x40) == 0x40);
		LCD_DATA7_IO = ((Data & 0x80) == 0x80);
		#endif
	#endif
	Nop();					// Wait Data setup time (min 40ns)
	Nop();
	LCD_E_IO = 1;
	Nop();					// Wait E Pulse width time (min 230ns)
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	LCD_E_IO = 0;

}
/******************************************************************************
 * Function:        void LCDInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        LCDText[] is blanked, port I/O pin TRIS registers are 
 *					configured, and the LCD is placed in the default state
 *
 * Note:            None
 *****************************************************************************/

void LCDInit(void)
{
	BYTE i;

	memset(LCDText, ' ', sizeof(LCDText)-1);
	LCDText[sizeof(LCDText)-1] = 0;

	// Setup the I/O pins
	LCD_E_IO = 0;
	LCD_RD_WR_IO = 0;


	#if defined(LCD_DATA_TRIS)
		LCD_DATA_TRIS = 0x00;
	#else
		LCD_DATA0_TRIS = 0;
		LCD_DATA1_TRIS = 0;
		LCD_DATA2_TRIS = 0;
		LCD_DATA3_TRIS = 0;
		#if !defined(FOUR_BIT_MODE)
		LCD_DATA4_TRIS = 0;
		LCD_DATA5_TRIS = 0;
		LCD_DATA6_TRIS = 0;
		LCD_DATA7_TRIS = 0;
		#endif
	#endif
	LCD_RD_WR_TRIS = 0;
	LCD_RS_TRIS = 0;
	LCD_E_TRIS = 0;
	LCD_BL_TRIS = 0;	//LCD背光灯控制
	LCD_BL_IO = 0;	//背光灯打开

	DelayMs(40);

	LCD_RS_IO = 0;
	#if defined(LCD_DATA_IO)
		LCD_DATA_IO = 0x03;
	#else
		LCD_DATA0_IO = 1;
		LCD_DATA1_IO = 1;
		LCD_DATA2_IO = 0;
		LCD_DATA3_IO = 0;
		#if !defined(FOUR_BIT_MODE)
		LCD_DATA4_IO = 0;
		LCD_DATA5_IO = 0;
		LCD_DATA6_IO = 0;
		LCD_DATA7_IO = 0;
		#endif
	#endif
	Nop();					// Wait Data setup time (min 40ns)
	Nop();
	for(i = 0; i < 3u; i++)
	{
		LCD_E_IO = 1;
		Delay10us(1);			// Wait E Pulse width time (min 230ns)
		LCD_E_IO = 0;
		Delay10us(5);
	}
	
#if defined(FOUR_BIT_MODE)
	#if defined(SAMSUNG_S6A0032)
		// Enter 4-bit mode (requires only 4-bits on the S6A0032)
		#if defined(LCD_DATA_IO)
			LCD_DATA_IO = 0x02;
		#else
			LCD_DATA0_IO = 0;
			LCD_DATA1_IO = 1;
			LCD_DATA2_IO = 0;
			LCD_DATA3_IO = 0;
		#endif
		Nop();					// Wait Data setup time (min 40ns)
		Nop();
		LCD_E_IO = 1;
		Delay10us(1);			// Wait E Pulse width time (min 230ns)
		LCD_E_IO = 0;
	#else
		// Enter 4-bit mode with two lines (requires 8-bits on most LCD controllers)
		LCDWrite(0, 0x28);
	#endif
#else
	// Use 8-bit mode with two lines
	LCDWrite(0, 0x38);
#endif
	Delay10us(5);
	
	// Set the entry mode
	LCDWrite(0, 0x06);	// Increment after each write, do not shift
	Delay10us(5);

	// Set the display control
	LCDWrite(0, 0x0C);		// Turn display on, no cusor, no cursor blink
	Delay10us(5);

	// Clear the display
	LCDWrite(0, 0x01);	
	DelayMs(2);
	
}

/******************************************************************************
 * Function:        void LCDUpdate(void)
 *
 * PreCondition:    LCDInit() must have been called once
 *
 * Input:           LCDText[]
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Copies the contents of the local LCDText[] array into the 
 *					LCD's internal display buffer.  Null terminators in 
 *					LCDText[] terminate the current line, so strings may be 
 *					printed directly to LCDText[].
 *
 * Note:            None
 *****************************************************************************/

void LCDUpdate(int b)
{
	BYTE i, j;

	// Go home
	LCDWrite(0, 0x02);
	DelayMs(2);

	// Output first line
	for(i = 0; i < 16u; i++)
	{
		// Erase the rest of the line if a null char is 
		// encountered (good for printing strings directly)
		if(LCDText[i] == 0u)
		{
			for(j=i; j < 16u; j++)
			{
				LCDText[j] = ' ';
			}
		}
		LCDWrite(1, LCDText[i]);
		Delay10us(5);
	}
	
	// Set the address to the second line
	LCDWrite(0, 0xC0);
	Delay10us(5);

	// Output second line
	for(i = 16; i < 32u; i++)
	{
		// Erase the rest of the line if a null char is 
		// encountered (good for printing strings directly)
		if(LCDText[i] == 0u)
		{
			for(j=i; j < 32u; j++)
			{
				LCDText[j] = ' ';
			}
		}
		LCDWrite(1, LCDText[i]);
		Delay10us(5);
	}
}

/******************************************************************************
 * Function:        void LCDErase(void)
 *
 * PreCondition:    LCDInit() must have been called once
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Clears LCDText[] and the LCD's internal display buffer
 *
 * Note:            None
 *****************************************************************************/

void LCDErase(void)
{
	// Clear display
	LCDWrite(0, 0x01);
	DelayMs(2);

	// Clear local copy
	memset(LCDText, ' ', 32);
}

////////////////////////////////////////////////////////////////cct十六进制转换10进制 2 CHAR TO 1 INT 
WORD HtoD(BYTE h,BYTE l)						
{
	WORD sum = 0;
	sum = h;
	sum<<=8;
	sum+=l;
	return sum;
}
#if defined(POWER_SUPPLY_6)
///////////////////////////////////////////////////////////////////cct 把AVs 中的数据显示到液晶屏上
void LCDshowAV(void)						
{
	static int temp = 0;
	WORD sum = 0,add16,a;
    BYTE ctemp ;
	memset(LCDText, ' ', 32);
  for(a=0;a<=1;a++)                      //CCT 这地方没有写好。
  {

	if(a==0)
		add16=0;
	else if(a==1)
		add16=16;

    LCDText[0+add16] = 'N';
    LCDText[1+add16] = 'o';
   // LCDText[2+add16] = '.';
	
	LCDText[2+add16] =temp+'1';


	sum = HtoD(AVs[temp][0],AVs[temp][1]);

	ctemp=sum/10000;
	sum = sum %10000;
	ctemp+='0';
	if(ctemp == '0')
		LCDText[4+add16] = ' ' ;
	else
		LCDText[4+add16] = ctemp;
	ctemp=sum/1000;
	sum = sum %1000;
	ctemp+='0';
	if(ctemp == '0'&&LCDText[4+add16]==' ')
		LCDText[5+add16] = ' ' ;
	else
		LCDText[5+add16] = ctemp;
	ctemp=sum/100;
	sum = sum %100;
	ctemp+='0';		
	LCDText[6+add16] = ctemp;
	LCDText[7+add16] = '.';
	ctemp=sum/10;
	sum = sum %10;
	ctemp+='0';		
	LCDText[8+add16] = ctemp;


	LCDText[9+add16] = 'V';

	sum = HtoD(AVs[temp][2],AVs[temp][3]);

	ctemp=sum/10000;
	sum = sum %10000;

	ctemp=sum/1000;
	sum = sum %1000;
	ctemp+='0';
	if(ctemp == '0')
		LCDText[10+add16] = ' ';
	else
		LCDText[10+add16] = ctemp;

	ctemp=sum/100;
	sum = sum %100;
	ctemp+='0';
	LCDText[11+add16] = ctemp;	

	LCDText[12+add16] = '.';

	ctemp=sum/10;
	sum = sum %10;
	ctemp+='0';		
	LCDText[13+add16] = ctemp;
	
	LCDText[14+add16] = sum+'0`';
	LCDText[15+add16] = 'A';

	temp++;
	if(temp==6)
		temp=0;
  }
	LCDUpdate(2);
}

#elif defined(POWER_SUPPLY_12_1602)
void LCDshowAV(void)						
{
	static int temp = 0;
	WORD sum = 0,add16,a;
    BYTE ctemp ;
	memset(LCDText, ' ', 32);
  for(a=0;a<=1;a++)                      //CCT 这地方没有写好。
  {

	if(a==0)
		add16=0;
	else if(a==1)
		add16=16;

	if(temp<9)
	{	
		LCDText[0+add16] = 'C';
		LCDText[1+add16] = '0';
		LCDText[2+add16] = (BYTE)((temp%10)+'1');	
	}
	else
    {
		LCDText[0+add16] = 'C';
		LCDText[1+add16] = '1';
		if(temp==9)
			LCDText[2+add16] ='0';
		else	
			LCDText[2+add16] =(BYTE)(temp%10)+'1';
	}


	sum = HtoD(AVs[temp][0],AVs[temp][1]);

	ctemp=sum/10000;
	sum = sum %10000;
	ctemp+='0';
	if(ctemp == '0')
		LCDText[4+add16] = ' ' ;
	else
		LCDText[4+add16] = ctemp;
	ctemp=sum/1000;
	sum = sum %1000;
	ctemp+='0';
	if(ctemp == '0'&&LCDText[4+add16]==' ')
		LCDText[5+add16] = ' ' ;
	else
		LCDText[5+add16] = ctemp;
	ctemp=sum/100;
	sum = sum %100;
	ctemp+='0';		
	LCDText[6+add16] = ctemp;
	LCDText[7+add16] = '.';
	ctemp=sum/10;
	sum = sum %10;
	ctemp+='0';		
	LCDText[8+add16] = ctemp;


	LCDText[9+add16] = 'V';

	sum = HtoD(AVs[temp][2],AVs[temp][3]);

	ctemp=sum/10000;
	sum = sum %10000;

	ctemp=sum/1000;
	sum = sum %1000;
	ctemp+='0';
	if(ctemp == '0')
		LCDText[10+add16] = ' ';
	else
		LCDText[10+add16] = ctemp;

	ctemp=sum/100;
	sum = sum %100;
	ctemp+='0';
	LCDText[11+add16] = ctemp;	

	LCDText[12+add16] = '.';

	ctemp=sum/10;
	sum = sum %10;
	ctemp+='0';		
	LCDText[13+add16] = ctemp;
	
	LCDText[14+add16] = sum+'0`';
	LCDText[15+add16] = 'A';

	temp++;
	if(temp==12)
		temp=0;
  }
	LCDUpdate(2);
}
#endif
#endif









