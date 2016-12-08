/*********************************************************************
 *
 *	Hardware specific definitions
 *
 *********************************************************************
 * FileName:        HardwareProfile.h
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
 * Author               Date		Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder		10/03/06	Original, copied from Compiler.h
 ********************************************************************/

#ifndef __HARDWARE_PROFILE_H
#define __HARDWARE_PROFILE_H

#include "MyApp.h"
// Choose which hardware profile to compile for here.  See 
// the hardware profiles below for meaning of various options.  
//#define PICDEMNET2
//#define HPC_EXPLORER
//#define PICDEMZ
//#define PIC24FJ64GA004_PIM	// Explorer 16, but with the PIC24FJ64GA004 PIM module, which has significantly differnt pin mappings
//#define EXPLORER_16		// PIC24FJ128GA010, PIC24HJ256GP610, dsPIC33FJ256GP710 PIMs
//#define DSPICDEM11
//#define YOUR_BOARD

// If no hardware profiles are defined, assume that we are using 
// a Microchip demo board and try to auto-select the correct profile
// based on processor selected in MPLAB
#if !defined(PICDEMNET2) && !defined(HPC_EXPLORER) && !defined(PICDEMZ) && !defined(EXPLORER_16) && !defined(PIC24FJ64GA004_PIM) && !defined(DSPICDEM11) && !defined(PICDEMNET2) && !defined(INTERNET_RADIO) && !defined(YOUR_BOARD)
	#if defined(__18F97J60) || defined(_18F97J60)
		#define PICDEMNET2	//自动选择为PICDEMNET2
	#elif defined(__18F67J60) || defined(_18F67J60)
		#define INTERNET_RADIO
	#elif defined(__18F8722) || defined(__18F87J10) || defined(_18F8722) || defined(_18F87J10) || defined(__18F87J50) || defined(_18F87J50)
		#define HPC_EXPLORER
	#elif defined(__18F4620) || defined(_18F4620)
		#define PICDEMZ
	#elif defined(__PIC24FJ64GA004__)
		#define PIC24FJ64GA004_PIM
	#elif defined(__PIC24F__) || defined(__PIC24H__) || defined(__dsPIC33F__) || defined(__PIC32MX__)
		#define EXPLORER_16
	#elif defined(__dsPIC30F__)
		#define DSPICDEM11
	#endif
#endif

// Set configuration fuses (but only once)
#if defined(THIS_IS_STACK_APPLICATION)
	#if defined(__18CXX)
		#if defined(__EXTENDED18__)
			#pragma config XINST=ON
		#elif !defined(HI_TECH_C)
			#pragma config XINST=OFF
		#endif
	
		#if defined(__18F8722)	//__表示MCC18编译器，_表示HIPICC18,后面都是以此来区分选用哪个编译器来编译的
			// PICDEM HPC Explorer board
			#pragma config OSC=HSPLL, FCMEN=OFF, IESO=OFF, PWRT=OFF, WDT=OFF, LVP=OFF
		#elif defined(_18F8722)	// HI-TECH PICC-18 compiler
			// PICDEM HPC Explorer board
			__CONFIG(1, HSPLL);
			__CONFIG(2, WDTDIS);
			__CONFIG(3, MCLREN);
			__CONFIG(4, XINSTDIS & LVPDIS);
		#elif defined(__18F87J10) || defined(__18F86J15) || defined(__18F86J10) || defined(__18F85J15) || defined(__18F85J10) || defined(__18F67J10) || defined(__18F66J15) || defined(__18F66J10) || defined(__18F65J15) || defined(__18F65J10)
			// PICDEM HPC Explorer board
			#pragma config WDTEN=OFF, FOSC2=ON, FOSC=HSPLL
		#elif defined(__18F97J60) || defined(__18F96J65) || defined(__18F96J60) || defined(__18F87J60) || defined(__18F86J65) || defined(__18F86J60) || defined(__18F67J60) || defined(__18F66J65) || defined(__18F66J60) 
			// PICDEM.net 2 or any other PIC18F97J60 family device
			#pragma config WDT=OFF, FOSC2=ON, FOSC=HSPLL, ETHLED=ON	//选用MCC18编译器来进行编译
		#elif defined(_18F97J60) || defined(_18F96J65) || defined(_18F96J60) || defined(_18F87J60) || defined(_18F86J65) || defined(_18F86J60) || defined(_18F67J60) || defined(_18F66J65) || defined(_18F66J60) 
			// PICDEM.net 2 board with HI-TECH PICC-18 compiler
			__CONFIG(1, WDTDIS & XINSTDIS);		//选用HIPICC18编译器来进行编译
			__CONFIG(2, HSPLL);
			__CONFIG(3, ETHLEDEN);
		#elif defined(__18F4620)	
			// PICDEM Z board
			#pragma config OSC=HSPLL, WDT=OFF, MCLRE=ON, PBADEN=OFF, LVP=OFF
		#endif
	#elif defined(__PIC24F__)
		// Explorer 16 board
		_CONFIG2(FNOSC_PRIPLL & POSCMOD_XT)		// Primary XT OSC with 4x PLL
		_CONFIG1(JTAGEN_OFF & FWDTEN_OFF)		// JTAG off, watchdog timer off
	#elif defined(__dsPIC33F__) || defined(__PIC24H__)
		// Explorer 16 board
		_FOSCSEL(FNOSC_PRIPLL)			// PLL enabled
		_FOSC(OSCIOFNC_OFF & POSCMD_XT)	// XT Osc
		_FWDT(FWDTEN_OFF)				// Disable Watchdog timer
		// JTAG should be disabled as well
	#elif defined(__dsPIC30F__)
		// dsPICDEM 1.1 board
		_FOSC(XT_PLL16)					// XT Osc + 16X PLL
		_FWDT(WDT_OFF)					// Disable Watchdog timer
		_FBORPOR(MCLR_EN & PBOR_OFF & PWRT_OFF)
	#elif defined(__PIC32MX__)
		#pragma config FPLLODIV = DIV_1, FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FWDTEN = OFF, FPBDIV = DIV_1, POSCMOD = XT, FNOSC = PRIPLL, CP = OFF
	#endif
#endif // Prevent more than one set of config fuse definitions

// Clock frequency value.
// This value is used to calculate Tick Counter value
#if defined(__18CXX)
	// All PIC18 processors
	#if defined(PICDEMNET2) || defined(INTERNET_RADIO)		//因为采用了25M晶振，CUP内部频率被倍频至41666667Hz
		#define GetSystemClock()		(41666667ul)      // Hz
		#define GetInstructionClock()	(GetSystemClock()/4)
		#define GetPeripheralClock()	GetInstructionClock()
	#elif defined(PICDEMZ)
		#define GetSystemClock()		(16000000ul)      // Hz
		#define GetInstructionClock()	(GetSystemClock()/4)
		#define GetPeripheralClock()	GetInstructionClock()
	#else
		#define GetSystemClock()		(40000000ul)      // Hz
		#define GetInstructionClock()	(GetSystemClock()/4)
		#define GetPeripheralClock()	GetInstructionClock()
	#endif
#elif defined(__PIC24F__)	
	// PIC24F processor
	#define GetSystemClock()		(32000000ul)      // Hz
	#define GetInstructionClock()	(GetSystemClock()/2)
	#define GetPeripheralClock()	GetInstructionClock()
#elif defined(__PIC24H__)	
	// PIC24H processor
	#define GetSystemClock()		(80000000ul)      // Hz
	#define GetInstructionClock()	(GetSystemClock()/2)
	#define GetPeripheralClock()	GetInstructionClock()
#elif defined(__dsPIC33F__)	
	// dsPIC33F processor
	#define GetSystemClock()		(80000000ul)      // Hz
	#define GetInstructionClock()	(GetSystemClock()/2)
	#define GetPeripheralClock()	GetInstructionClock()
#elif defined(__dsPIC30F__)
	// dsPIC30F processor
	#define GetSystemClock()		(117920000ul)      // Hz
	#define GetInstructionClock()	(GetSystemClock()/4)
	#define GetPeripheralClock()	GetInstructionClock()
#elif defined(__PIC32MX__)
	// PIC32MX processor
	#define GetSystemClock()		(80000000ul)      // Hz
	#define GetInstructionClock()	(GetSystemClock()/1)
	#define GetPeripheralClock()	(GetInstructionClock()/1)	// Set your divider according to your Peripheral Bus Frequency configuration fuse setting
#endif

// Hardware mappings




#if defined(POWER_SUPPLY_12)
	//RELAY I/O pins		继电器
	#define RELAY1_TRIS			(TRISAbits.TRISA3)
	#define RELAY1_IO			(LATAbits.LATA3)
	#define RELAY2_TRIS			(TRISFbits.TRISF1)
	#define RELAY2_IO			(LATFbits.LATF1)
	#define RELAY3_TRIS			(TRISHbits.TRISH4)
	#define RELAY3_IO			(LATHbits.LATH4)
	#define RELAY4_TRIS			(TRISHbits.TRISH5)
	#define RELAY4_IO			(LATHbits.LATH5)
	#define RELAY5_TRIS			(TRISHbits.TRISH6)
	#define RELAY5_IO			(LATHbits.LATH6)
	#define RELAY6_TRIS			(TRISHbits.TRISH7)
	#define RELAY6_IO			(LATHbits.LATH7)
	#define RELAY7_TRIS			(TRISFbits.TRISF2)
	#define RELAY7_IO			(LATFbits.LATF2)
	#define RELAY8_TRIS			(TRISFbits.TRISF3)
	#define RELAY8_IO			(LATFbits.LATF3)
	#define RELAY9_TRIS			(TRISFbits.TRISF4)
	#define RELAY9_IO			(LATFbits.LATF4)
	#define RELAYA_TRIS			(TRISFbits.TRISF5)
	#define RELAYA_IO			(LATFbits.LATF5)
	#define RELAYB_TRIS			(TRISFbits.TRISF6)     //CCT 11路
	#define RELAYB_IO			(LATFbits.LATF6)
	#define RELAYC_TRIS			(TRISFbits.TRISF7)
	#define RELAYC_IO			(LATFbits.LATF7)


//	#define BUTTON0_TRIS		(TRISFbits.TRISF5)//	跟Microchip原版有所改变
//	#define	BUTTON0_IO			(PORTFbits.RF5)
////-------------------------------------------------------------------------------------/////
	#define BUTTON0_TRIS		(TRISAbits.TRISA5)
	#define	BUTTON0_IO			(PORTAbits.RA5)
////-------------------------------------------------------------------------------------/////	
	// ENC28J60 I/O pins
	#define ENC_RST_TRIS		(TRISDbits.TRISD3)	// OK    Not connected by default
	#define ENC_RST_IO			(LATDbits.LATD3)
//	#define ENC_CS_TRIS			(TRISDbits.TRISD1)	// Uncomment this line if you wish to use the ENC28J60 on the PICDEM.net 2 board instead of the internal PIC18F97J60 Ethernet module
	#define ENC_CS_IO			(LATDbits.LATD1)
	#define ENC_SCK_TRIS		(TRISDbits.TRISD6)
	#define ENC_SDI_TRIS		(TRISDbits.TRISD5)
	#define ENC_SDO_TRIS		(TRISDbits.TRISD4)
	#define ENC_SPI_IF			(PIR3bits.SSP2IF)
	#define ENC_SSPBUF			(SSP2BUF)
	#define ENC_SPISTAT			(SSP2STAT)
	#define ENC_SPISTATbits		(SSP2STATbits)
	#define ENC_SPICON1			(SSP2CON1)
	#define ENC_SPICON1bits		(SSP2CON1bits)
	#define ENC_SPICON2			(SSP2CON2)

	// 25LC256 I/O pins
	#define EEPROM_CS_TRIS		(TRISCbits.TRISC0)	//OK   E2PROM
	#define EEPROM_CS_IO		(LATCbits.LATC0)
	#define EEPROM_SCK_TRIS		(TRISCbits.TRISC3)
	#define EEPROM_SDI_TRIS		(TRISCbits.TRISC4)
	#define EEPROM_SDO_TRIS		(TRISCbits.TRISC5)
	#define EEPROM_SPI_IF		(PIR1bits.SSPIF)
	#define EEPROM_SSPBUF		(SSPBUF)
	#define EEPROM_SPICON1		(SSP1CON1)
	#define EEPROM_SPICON1bits	(SSP1CON1bits)
	#define EEPROM_SPICON2		(SSP1CON2)
	#define EEPROM_SPISTAT		(SSP1STAT)
	#define EEPROM_SPISTATbits	(SSP1STATbits)

	// LCD I/O pins
	#define LCD_BL_TRIS			(TRISHbits.TRISH3)	//OK LCD背光灯控制		
	#define LCD_BL_IO			(LATHbits.LATH3)
	#define LCD_DATA_TRIS		(TRISE)
	#define LCD_DATA_IO			(LATE)
    #define LCD_DATA_IN			(PORTE)
	#define LCD_RD_WR_TRIS		(TRISHbits.TRISH1)   //OK
	#define LCD_RD_WR_IO		(LATHbits.LATH1)
	#define LCD_RS_TRIS			(TRISHbits.TRISH2)   //OK
	#define LCD_RS_IO			(LATHbits.LATH2)
	#define LCD_E_TRIS			(TRISHbits.TRISH0)   //OK
	#define LCD_E_IO			(LATHbits.LATH0)

	//LED_RUN I/O pins		系统运行指示灯
	#define LED_RUN_TRIS		(TRISGbits.TRISG3)   //OK
	#define LED_RUN_IO			(LATGbits.LATG3)     //OK              

	//DS1302 I/O pins		实时时钟
	#define RTC_RST_TRIS		(TRISGbits.TRISG6)   //OK复位
	#define RTC_RST_IO			(LATGbits.LATG6)     //OK
	#define RTC_SCL_TRIS		(TRISGbits.TRISG4)   //OK输出时钟
	#define RTC_SCL_IO			(LATGbits.LATG4)     //OK
	#define RTC_SDA_TRIS		(TRISGbits.TRISG5)   //OK数据输出
	#define RTC_SDA_IN			(PORTGbits.RG5)      //OK
	#define RTC_SDA_OUT			(LATGbits.LATG5)     //OK

	//DS18B20 I/O pins		单总线温湿度传感器
	#define THemp_SDA_TRIS		(TRISBbits.TRISB1)   //ok
	#define THemp_SDA_IN		(PORTBbits.RB1)
	#define THemp_SDA_OUT		(LATBbits.LATB1)
	#define THemp_SCL_TRIS		(TRISBbits.TRISB0)
	#define THemp_SCL_IO		(LATBbits.LATB0)



	//IR I/O pins			红线报警器接入点
	#define IR_RXD_TRIS			(TRISBbits.TRISB4)
	#define IR_RXD_IO			(PORTBbits.RB4)

	//Buzzer I/O pins		蜂鸣器
	#define SPK_TRIS			(TRISAbits.TRISA4)    //OK
	#define SPK_IO				(LATAbits.LATA4)     //OK

	// Photosensitive resistance  I/O pins	光敏电阻
	#define PR_TRIS				(TRISAbits.TRISA2)
	#define PR_IO				(LATAbits.LATA2)

	//RS485 I/O pins		MAX485接口
	#define RS485_DIR_TRIS		(TRISGbits.TRISG0)
	#define RS485_DIR_IO		(LATGbits.LATG0)
	#define RS485_RX_TRIS		(TRISGbits.TRISG2)
	#define RS485_RX_IO			(PORTGbits.RG2)
	#define RS485_TX_TRIS		(TRISGbits.TRISG1)
	#define RS485_TX_IO			(LATGbits.LATG1)

#elif defined(POWER_SUPPLY_12_1602)

	//RELAY I/O pins		继电器
	#define RELAY1_TRIS			(TRISAbits.TRISA3)
	#define RELAY1_IO			(LATAbits.LATA3)
	#define RELAY2_TRIS			(TRISFbits.TRISF1)
	#define RELAY2_IO			(LATFbits.LATF1)
	#define RELAY3_TRIS			(TRISHbits.TRISH4)
	#define RELAY3_IO			(LATHbits.LATH4)
	#define RELAY4_TRIS			(TRISHbits.TRISH5)
	#define RELAY4_IO			(LATHbits.LATH5)
	#define RELAY5_TRIS			(TRISHbits.TRISH6)
	#define RELAY5_IO			(LATHbits.LATH6)
	#define RELAY6_TRIS			(TRISHbits.TRISH7)
	#define RELAY6_IO			(LATHbits.LATH7)
	#define RELAY7_TRIS			(TRISFbits.TRISF2)
	#define RELAY7_IO			(LATFbits.LATF2)
	#define RELAY8_TRIS			(TRISFbits.TRISF3)
	#define RELAY8_IO			(LATFbits.LATF3)
	#define RELAY9_TRIS			(TRISFbits.TRISF4)
	#define RELAY9_IO			(LATFbits.LATF4)
	#define RELAYA_TRIS			(TRISFbits.TRISF5)
	#define RELAYA_IO			(LATFbits.LATF5)
	#define RELAYB_TRIS			(TRISFbits.TRISF6)     //CCT 11路
	#define RELAYB_IO			(LATFbits.LATF6)
	#define RELAYC_TRIS			(TRISFbits.TRISF7)
	#define RELAYC_IO			(LATFbits.LATF7)


//	#define BUTTON0_TRIS		(TRISFbits.TRISF5)//	跟Microchip原版有所改变
//	#define	BUTTON0_IO			(PORTFbits.RF5)
////-------------------------------------------------------------------------------------/////
	#define BUTTON0_TRIS		(TRISAbits.TRISA5)
	#define	BUTTON0_IO			(PORTAbits.RA5)
////-------------------------------------------------------------------------------------/////	
	// ENC28J60 I/O pins
	#define ENC_RST_TRIS		(TRISDbits.TRISD3)	// OK    Not connected by default
	#define ENC_RST_IO			(LATDbits.LATD3)
//	#define ENC_CS_TRIS			(TRISDbits.TRISD1)	// Uncomment this line if you wish to use the ENC28J60 on the PICDEM.net 2 board instead of the internal PIC18F97J60 Ethernet module
	#define ENC_CS_IO			(LATDbits.LATD1)
	#define ENC_SCK_TRIS		(TRISDbits.TRISD6)
	#define ENC_SDI_TRIS		(TRISDbits.TRISD5)
	#define ENC_SDO_TRIS		(TRISDbits.TRISD4)
	#define ENC_SPI_IF			(PIR3bits.SSP2IF)
	#define ENC_SSPBUF			(SSP2BUF)
	#define ENC_SPISTAT			(SSP2STAT)
	#define ENC_SPISTATbits		(SSP2STATbits)
	#define ENC_SPICON1			(SSP2CON1)
	#define ENC_SPICON1bits		(SSP2CON1bits)
	#define ENC_SPICON2			(SSP2CON2)

	// 25LC256 I/O pins
	#define EEPROM_CS_TRIS		(TRISCbits.TRISC0)	//OK   E2PROM
	#define EEPROM_CS_IO		(LATCbits.LATC0)
	#define EEPROM_SCK_TRIS		(TRISCbits.TRISC3)
	#define EEPROM_SDI_TRIS		(TRISCbits.TRISC4)
	#define EEPROM_SDO_TRIS		(TRISCbits.TRISC5)
	#define EEPROM_SPI_IF		(PIR1bits.SSPIF)
	#define EEPROM_SSPBUF		(SSPBUF)
	#define EEPROM_SPICON1		(SSP1CON1)
	#define EEPROM_SPICON1bits	(SSP1CON1bits)
	#define EEPROM_SPICON2		(SSP1CON2)
	#define EEPROM_SPISTAT		(SSP1STAT)
	#define EEPROM_SPISTATbits	(SSP1STATbits)

	// LCD I/O pins
	#define LCD_BL_TRIS			(TRISHbits.TRISH3)	//OK LCD背光灯控制		
	#define LCD_BL_IO			(LATHbits.LATH3)
	#define LCD_DATA_TRIS		(TRISE)
	#define LCD_DATA_IO			(LATE)
    #define LCD_DATA_IN			(PORTE)
	#define LCD_RD_WR_TRIS		(TRISHbits.TRISH1)   //OK
	#define LCD_RD_WR_IO		(LATHbits.LATH1)
	#define LCD_RS_TRIS			(TRISHbits.TRISH2)   //OK
	#define LCD_RS_IO			(LATHbits.LATH2)
	#define LCD_E_TRIS			(TRISHbits.TRISH0)   //OK
	#define LCD_E_IO			(LATHbits.LATH0)

	//LED_RUN I/O pins		系统运行指示灯
	#define LED_RUN_TRIS		(TRISGbits.TRISG3)   //OK
	#define LED_RUN_IO			(LATGbits.LATG3)     //OK              

	//DS1302 I/O pins		实时时钟
	#define RTC_RST_TRIS		(TRISGbits.TRISG6)   //OK复位
	#define RTC_RST_IO			(LATGbits.LATG6)     //OK
	#define RTC_SCL_TRIS		(TRISGbits.TRISG4)   //OK输出时钟
	#define RTC_SCL_IO			(LATGbits.LATG4)     //OK
	#define RTC_SDA_TRIS		(TRISGbits.TRISG5)   //OK数据输出
	#define RTC_SDA_IN			(PORTGbits.RG5)      //OK
	#define RTC_SDA_OUT			(LATGbits.LATG5)     //OK

	//DS18B20 I/O pins		单总线温湿度传感器
	#define THemp_SDA_TRIS		(TRISBbits.TRISB1)   //ok
	#define THemp_SDA_IN		(PORTBbits.RB1)
	#define THemp_SDA_OUT		(LATBbits.LATB1)
	#define THemp_SCL_TRIS		(TRISBbits.TRISB0)
	#define THemp_SCL_IO		(LATBbits.LATB0)



	//IR I/O pins			红线报警器接入点
	#define IR_RXD_TRIS			(TRISBbits.TRISB4)
	#define IR_RXD_IO			(PORTBbits.RB4)

	//Buzzer I/O pins		蜂鸣器
	#define SPK_TRIS			(TRISAbits.TRISA4)    //OK
	#define SPK_IO				(LATAbits.LATA4)     //OK

	// Photosensitive resistance  I/O pins	光敏电阻
	#define PR_TRIS				(TRISAbits.TRISA2)
	#define PR_IO				(LATAbits.LATA2)

	//RS485 I/O pins		MAX485接口
	#define RS485_DIR_TRIS		(TRISGbits.TRISG0)
	#define RS485_DIR_IO		(LATGbits.LATG0)
	#define RS485_RX_TRIS		(TRISGbits.TRISG2)
	#define RS485_RX_IO			(PORTGbits.RG2)
	#define RS485_TX_TRIS		(TRISGbits.TRISG1)
	#define RS485_TX_IO			(LATGbits.LATG1)

#elif defined(POWER_SUPPLY_6)
	//RELAY I/O pins		继电器
	#define RELAY7_TRIS			(TRISAbits.TRISA3)
	#define RELAY7_IO			(LATAbits.LATA3)
	#define RELAY1_TRIS			(TRISFbits.TRISF1)
	#define RELAY1_IO			(LATFbits.LATF1)
	#define RELAY2_TRIS			(TRISHbits.TRISH4)
	#define RELAY2_IO			(LATHbits.LATH4)
	#define RELAY3_TRIS			(TRISHbits.TRISH5)
	#define RELAY3_IO			(LATHbits.LATH5)
	#define RELAY4_TRIS			(TRISHbits.TRISH6)
	#define RELAY4_IO			(LATHbits.LATH6)
	#define RELAY5_TRIS			(TRISHbits.TRISH7)
	#define RELAY5_IO			(LATHbits.LATH7)
	#define RELAY6_TRIS			(TRISFbits.TRISF2)
	#define RELAY6_IO			(LATFbits.LATF2)
	#define RELAY8_TRIS			(TRISFbits.TRISF3)
	#define RELAY8_IO			(LATFbits.LATF3)
	#define RELAY9_TRIS			(TRISFbits.TRISF4)
	#define RELAY9_IO			(LATFbits.LATF4)
	#define RELAYA_TRIS			(TRISFbits.TRISF5)
	#define RELAYA_IO			(LATFbits.LATF5)
	#define RELAYB_TRIS			(TRISFbits.TRISF6)     //CCT 11路
	#define RELAYB_IO			(LATFbits.LATF6)
	#define RELAYC_TRIS			(TRISFbits.TRISF7)
	#define RELAYC_IO			(LATFbits.LATF7)


//	#define BUTTON0_TRIS		(TRISFbits.TRISF5)//	跟Microchip原版有所改变
//	#define	BUTTON0_IO			(PORTFbits.RF5)
////-------------------------------------------------------------------------------------/////
	#define BUTTON0_TRIS		(TRISAbits.TRISA5)
	#define	BUTTON0_IO			(PORTAbits.RA5)
////-------------------------------------------------------------------------------------/////	
	// ENC28J60 I/O pins
	#define ENC_RST_TRIS		(TRISDbits.TRISD3)	// OK    Not connected by default
	#define ENC_RST_IO			(LATDbits.LATD3)
//	#define ENC_CS_TRIS			(TRISDbits.TRISD1)	// Uncomment this line if you wish to use the ENC28J60 on the PICDEM.net 2 board instead of the internal PIC18F97J60 Ethernet module
	#define ENC_CS_IO			(LATDbits.LATD1)
	#define ENC_SCK_TRIS		(TRISDbits.TRISD6)
	#define ENC_SDI_TRIS		(TRISDbits.TRISD5)
	#define ENC_SDO_TRIS		(TRISDbits.TRISD4)
	#define ENC_SPI_IF			(PIR3bits.SSP2IF)
	#define ENC_SSPBUF			(SSP2BUF)
	#define ENC_SPISTAT			(SSP2STAT)
	#define ENC_SPISTATbits		(SSP2STATbits)
	#define ENC_SPICON1			(SSP2CON1)
	#define ENC_SPICON1bits		(SSP2CON1bits)
	#define ENC_SPICON2			(SSP2CON2)

	// 25LC256 I/O pins
	#define EEPROM_CS_TRIS		(TRISCbits.TRISC0)	//OK   E2PROM
	#define EEPROM_CS_IO		(LATCbits.LATC0)
	#define EEPROM_SCK_TRIS		(TRISCbits.TRISC3)
	#define EEPROM_SDI_TRIS		(TRISCbits.TRISC4)
	#define EEPROM_SDO_TRIS		(TRISCbits.TRISC5)
	#define EEPROM_SPI_IF		(PIR1bits.SSPIF)
	#define EEPROM_SSPBUF		(SSPBUF)
	#define EEPROM_SPICON1		(SSP1CON1)
	#define EEPROM_SPICON1bits	(SSP1CON1bits)
	#define EEPROM_SPICON2		(SSP1CON2)
	#define EEPROM_SPISTAT		(SSP1STAT)
	#define EEPROM_SPISTATbits	(SSP1STATbits)

	// LCD I/O pins
	#define LCD_BL_TRIS			(TRISHbits.TRISH3)	//OK LCD背光灯控制		
	#define LCD_BL_IO			(LATHbits.LATH3)
	#define LCD_DATA_TRIS		(TRISE)
	#define LCD_DATA_IO			(LATE)
    #define LCD_DATA_IN			(PORTE)
	#define LCD_RD_WR_TRIS		(TRISHbits.TRISH1)   //OK
	#define LCD_RD_WR_IO		(LATHbits.LATH1)
	#define LCD_RS_TRIS			(TRISHbits.TRISH2)   //OK
	#define LCD_RS_IO			(LATHbits.LATH2)
	#define LCD_E_TRIS			(TRISHbits.TRISH0)   //OK
	#define LCD_E_IO			(LATHbits.LATH0)

	//LED_RUN I/O pins		系统运行指示灯
	#define LED_RUN_TRIS		(TRISGbits.TRISG3)   //OK
	#define LED_RUN_IO			(LATGbits.LATG3)     //OK              

	//DS1302 I/O pins		实时时钟
	#define RTC_RST_TRIS		(TRISGbits.TRISG6)   //OK复位
	#define RTC_RST_IO			(LATGbits.LATG6)     //OK
	#define RTC_SCL_TRIS		(TRISGbits.TRISG4)   //OK输出时钟
	#define RTC_SCL_IO			(LATGbits.LATG4)     //OK
	#define RTC_SDA_TRIS		(TRISGbits.TRISG5)   //OK数据输出
	#define RTC_SDA_IN			(PORTGbits.RG5)      //OK
	#define RTC_SDA_OUT			(LATGbits.LATG5)     //OK

	//DS18B20 I/O pins		单总线温湿度传感器
//	#define Temp_DQ_TRIS		(TRISBbits.TRISB1)
//	#define Temp_DQ_IN			(PORTBbits.RB1)
//	#define Temp_DQ_OUT			(LATBbits.LATB1)
	#define THemp_SDA_TRIS		(TRISBbits.TRISB1)   //ok
	#define THemp_SDA_IN		(PORTBbits.RB1)
	#define THemp_SDA_OUT		(LATBbits.LATB1)
	#define THemp_SCL_TRIS		(TRISBbits.TRISB0)
	#define THemp_SCL_IO		(LATBbits.LATB0)



	//IR I/O pins			红线报警器接入点
	#define IR_RXD_TRIS			(TRISBbits.TRISB4)
	#define IR_RXD_IO			(PORTBbits.RB4)

	//Buzzer I/O pins		蜂鸣器
	#define SPK_TRIS			(TRISAbits.TRISA4)    //OK
	#define SPK_IO				(LATAbits.LATA4)     //OK

	// Photosensitive resistance  I/O pins	光敏电阻
	#define PR_TRIS				(TRISAbits.TRISA2)
	#define PR_IO				(LATAbits.LATA2)

	//RS485 I/O pins		MAX485接口
	#define RS485_DIR_TRIS		(TRISGbits.TRISG0)
	#define RS485_DIR_IO		(LATGbits.LATG0)
	#define RS485_RX_TRIS		(TRISGbits.TRISG2)
	#define RS485_RX_IO			(PORTGbits.RG2)
	#define RS485_TX_TRIS		(TRISGbits.TRISG1)
	#define RS485_TX_IO			(LATGbits.LATG1)

#endif





#if defined(__18CXX)	// PIC18
	// UART mapping functions for consistent API names across 8-bit and 16 or 
	// 32 bit compilers.  For simplicity, everything will use "UART" instead 
	// of USART/EUSART/etc.
	#define BusyUART()				BusyUSART()
	#define CloseUART()				CloseUSART()
	#define ConfigIntUART(a)		ConfigIntUSART(a)
	#define DataRdyUART()			DataRdyUSART()
	#define OpenUART(a,b,c)			OpenUSART(a,b,c)
	#define ReadUART()				ReadUSART()
	#define WriteUART(a)			WriteUSART(a)
	#define getsUART(a,b,c)			getsUSART(b,a)
	#define putsUART(a)				putsUSART(a)
	#define getcUART()				ReadUSART()
	#define putcUART(a)				WriteUSART(a)
	#define putrsUART(a)			putrsUSART((far rom char*)a)

#endif
#endif

