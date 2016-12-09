#ifndef __H_Interface_H__
#define __H_Interface_H__

/*=======================================================================
					功率计硬件接口定义
=======================================================================*/

// 本机地址线设定端口
sbit LocalAddress_A0 = P1^4;
sbit LocalAddress_A1 = P1^5;
//sbit LocalAddress_A2 = P1^6;
//sbit LocalAddress_A3 = P1^7;

// CD4051通道控制端口
sbit CD4051_A = P0^1;
sbit CD4051_B = P3^4;
sbit CD4051_C = P3^5;
sbit CD4051_EN0 = P0^0;	 // 电流通道 低电平有效
sbit CD4051_EN1 = P3^2;  // 电压通道 低电平有效
//sbit  STL =P2^2;	      //ge li de dizhi		    5U YONG
sbit STL=P1^7;           //6   LU
// 程序指示灯
sbit System_LED = P2^3;
#define con_LEDOn  0
#define con_LEDOff 1

// 485使能端口
sbit MAX485_Enable = P2^1;
#define Max485_Tran  MAX485_Enable = 0	  // 6lu
#define Max485_Rece  MAX485_Enable = 1


//#define Max485_Tran  MAX485_Enable = 1	  // 加隔离	电源了的
//#define Max485_Rece  MAX485_Enable = 0



// CS5460端口定义
sbit CS5460_SDI   = P2^4; // CS560的串行数据输入端
sbit CS5460_SDO   = P0^2; // CS560的串行数据输出端
sbit CS5460_SCLK  = P1^0; // CS560的串行时钟输入端
sbit CS5460_CS    = P0^3; // CS560的片选控制端
sbit CS5460_SINT  = P3^3; // CS560的外部结果寄存器更新中断
sbit CS5460_EDIR  = P2^5; // 能量负值指示
sbit CS5460_EOUT  = P2^6; // 能量输出
sbit CS5460_RST   = P2^7; // CS560的复位控制端


#define CS5460_CSHigh   	CS5460_CS = 1
#define CS5460_CSLow    	CS5460_CS = 0

#define CS5460_SCLKHigh   	CS5460_SCLK = 1
#define CS5460_SCLKLow    	CS5460_SCLK = 0
								 
#define CS5460_SDIHigh   	CS5460_SDI = 1
#define CS5460_SDILow    	CS5460_SDI = 0

#define CS5460_RSTHigh   	CS5460_RST = 1
#define CS5460_RSTLow    	CS5460_RST = 0

/*=======================================================================
				CS5460命令字和寄存器地址预定义
=======================================================================*/
#define cmdStartCVTSigle   0xE0   
#define cmdStartCVTCtinu   0xE8   
#define cmdSYNC0           0xFE   
#define cmdSYNC1           0xFF   
#define cmdPowerUpHalt     0xA0   
#define cmdPowerIDLE       0x88   
#define cmdPowerSleep      0x90   

#define cmdCabBase		   0xC0   
#define cmdVCab            (0x01 << 4) 
#define cmdICab            (0x01 << 3) 
#define cmdACCab           (0x01 << 2) 
#define cmdGainCab         (0x01 << 1) 
#define cmdOffsetCab       (0x01 << 2) 

#define cmdRegRead       	(0x00 << 6) 
#define cmdRegWrite         (0x01 << 6) 

#define addConfig          	(0x00 << 1) 
#define addIDCOffset        (0x01 << 1) 
#define addIGain            (0x02 << 1) 
#define addVDCOffset        (0x03 << 1) 
#define addVGain          	(0x04 << 1) 
#define addCycleCount       (0x05 << 1) 
#define addPulseRate        (0x06 << 1) 
#define addI          		(0x07 << 1) 
#define addV          		(0x08 << 1) 
#define addP          		(0x09 << 1) 
#define addE          		(0x0A << 1) 
#define addIRMS          	(0x0B << 1) 
#define addVRMS          	(0x0C << 1) 
#define addTBC          	(0x0D << 1) 
#define addPowerOffset      (0x0E << 1) 
#define addStatus          	(0x0F << 1) 
#define addIACOffset       	(0x10 << 1) 
#define addVACOffset      	(0x11 << 1) 

#define addMask          	(0x1A << 1) 
#define addControl          (0x1C << 1) 

#endif


