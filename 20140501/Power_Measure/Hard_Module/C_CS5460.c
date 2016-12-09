
/*=========================================================
版    本：V1.0
开发时间：2010年7月26日
版权所有：2010--2099,河北科技大学  信息科学与工程系  孟志永
  					                      手机：13933120973
									   Email：KDMZY@163.com
  文 件 名：C_CS5460.c
  作    者: 孟志永
  版    本: V1.0		
  日    期: 2010年7月26日
  描述信息: CS5460功率计芯片驱动程序
  其他说明: 芯片类型：STC12C5604AD         
  功能列表: 
  历史纪录:   
            1、日    期:
               作    者:
               修改内容:
=========================================================*/
#include "STC12C5620AD_H.h"	
#include "H_Interface.h"

/*=======================================================================


/*=======================================================================
函数名称：CS5460_Send
函数功能: CS5460 SPI 发送数据的函数
输入参数：要发送的数据
输出参数：返回当前SDO输出的数据
其它说明：无
=======================================================================*/
unsigned char CS5460_Send( unsigned char Send_Data ){

	unsigned char i;
	unsigned char Read_Data;

	Read_Data = 0;
	for( i = 0; i < 8; i++ ){					 // 把命令字发送出去
		CS5460_SCLKLow; //CS5460_SCLKLow; CS5460_SCLKLow; CS5460_SCLKLow;
		if( ( Send_Data & 0x80 ) == 0x80 ){ CS5460_SDIHigh; }
		else							  { CS5460_SDILow;  } 
		Read_Data <<= 1;
		CS5460_SDO = 1;
		if( CS5460_SDO ){ Read_Data |= 1; }
		CS5460_SCLKHigh;  //CS5460_SCLKHigh;  CS5460_SCLKHigh;  CS5460_SCLKHigh;  

	//	CS5460_SCLKLow;
		Send_Data <<= 1;
	}

	return Read_Data;

}

/*=======================================================================
函数名称：CS5460_SendCmd
函数功能: CS5460 SPI 发送命令的函数
输入参数：要发送的命令
输出参数：无
其它说明：无
=======================================================================*/
void CS5460_SendCmd( unsigned char Send_Data ){

	CS5460_CSLow;
	CS5460_Send( cmdSYNC1 );  // 
	CS5460_Send( cmdSYNC1 );  // 
	CS5460_Send( cmdSYNC1 );  // 
	CS5460_Send( cmdSYNC0 );  // 
	CS5460_CSHigh;

	CS5460_CSLow;
	CS5460_Send( Send_Data );
	CS5460_CSHigh;

}

/*=======================================================================
函数名称：CS5460_ReadReg
函数功能：读取CS5460的某一项寄存器的函数
输入参数：要获取的寄存器
输出参数：当前寄存器的内容
其它说明：无
=======================================================================*/
long CS5460_ReadReg( unsigned char Reg_Address ){

	long Read_Data;
	unsigned char Temp_Data0 = 0;
	unsigned char Temp_Data1 = 0;
	unsigned char Temp_Data2 = 0;
	unsigned char Temp_Data3 = 0;

	CS5460_SCLKLow;	
	CS5460_CSLow;
	CS5460_CSLow;
	CS5460_CSLow;
	CS5460_CSLow;

	CS5460_Send( cmdRegRead | Reg_Address );
	Temp_Data0 = CS5460_Send( cmdSYNC0 );	
	Temp_Data1 = CS5460_Send( cmdSYNC0 );
	Temp_Data2 = CS5460_Send( cmdSYNC0 );

	Read_Data = Temp_Data0;
	Read_Data <<= 8;
	Read_Data |= Temp_Data1;
	Read_Data <<= 8;
	Read_Data |= Temp_Data2;

	CS5460_CSHigh;	 

	return Read_Data;

}


 
 /*=======================================================================
函数名称：CS5460_ReadReg
函数功能：读取CS5460的某一项寄存器的函数
输入参数：要获取的寄存器
输出参数：当前寄存器的内容
其它说明：无
=======================================================================*/
/*
long CS5460_ReadReg( unsigned char Reg_Address ){

	long Read_Data;
	unsigned char Temp_Data0 = 0;
	unsigned char Temp_Data1 = 0;
	unsigned char Temp_Data2 = 0;
	unsigned char Temp_Data3 = 0;

	CS5460_SCLKLow;	
	CS5460_CSLow;
	CS5460_CSLow;
	CS5460_CSLow;
	CS5460_CSLow;

	CS5460_Send( cmdRegRead | Reg_Address );
	Temp_Data0 = CS5460_Send( cmdSYNC0 );	
	Temp_Data1 = CS5460_Send( cmdSYNC0 );
	Temp_Data2 = CS5460_Send( cmdSYNC0 );

	Read_Data = Temp_Data0;
	Read_Data <<= 8;
	Read_Data |= Temp_Data1;
	Read_Data <<= 8;
	Read_Data |= Temp_Data2;

	CS5460_CSHigh;	 

	return Read_Data;

}
 */



/*=======================================================================
函数名称：CS5460_WriteReg
函数功能：写CS5460的某一项寄存器的函数
输入参数：要写的寄存器地址,要写入的数据0、数据1，数据2
输出参数：无
其它说明：无
=======================================================================*/
void CS5460_WriteReg( unsigned char Reg_Address, unsigned char Reg_Data0, unsigned char Reg_Data1, unsigned char Reg_Data2 ){

	CS5460_CSLow;

	CS5460_Send( cmdRegWrite | Reg_Address );
	CS5460_Send( Reg_Data0 );
	CS5460_Send( Reg_Data1 );
	CS5460_Send( Reg_Data2 );  

	CS5460_CSHigh;

}
















