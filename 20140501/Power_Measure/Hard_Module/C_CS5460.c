
/*=========================================================
��    ����V1.0
����ʱ�䣺2010��7��26��
��Ȩ���У�2010--2099,�ӱ��Ƽ���ѧ  ��Ϣ��ѧ�빤��ϵ  ��־��
  					                      �ֻ���13933120973
									   Email��KDMZY@163.com
  �� �� ����C_CS5460.c
  ��    ��: ��־��
  ��    ��: V1.0		
  ��    ��: 2010��7��26��
  ������Ϣ: CS5460���ʼ�оƬ��������
  ����˵��: оƬ���ͣ�STC12C5604AD         
  �����б�: 
  ��ʷ��¼:   
            1����    ��:
               ��    ��:
               �޸�����:
=========================================================*/
#include "STC12C5620AD_H.h"	
#include "H_Interface.h"

/*=======================================================================


/*=======================================================================
�������ƣ�CS5460_Send
��������: CS5460 SPI �������ݵĺ���
���������Ҫ���͵�����
������������ص�ǰSDO���������
����˵������
=======================================================================*/
unsigned char CS5460_Send( unsigned char Send_Data ){

	unsigned char i;
	unsigned char Read_Data;

	Read_Data = 0;
	for( i = 0; i < 8; i++ ){					 // �������ַ��ͳ�ȥ
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
�������ƣ�CS5460_SendCmd
��������: CS5460 SPI ��������ĺ���
���������Ҫ���͵�����
�����������
����˵������
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
�������ƣ�CS5460_ReadReg
�������ܣ���ȡCS5460��ĳһ��Ĵ����ĺ���
���������Ҫ��ȡ�ļĴ���
�����������ǰ�Ĵ���������
����˵������
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
�������ƣ�CS5460_ReadReg
�������ܣ���ȡCS5460��ĳһ��Ĵ����ĺ���
���������Ҫ��ȡ�ļĴ���
�����������ǰ�Ĵ���������
����˵������
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
�������ƣ�CS5460_WriteReg
�������ܣ�дCS5460��ĳһ��Ĵ����ĺ���
���������Ҫд�ļĴ�����ַ,Ҫд�������0������1������2
�����������
����˵������
=======================================================================*/
void CS5460_WriteReg( unsigned char Reg_Address, unsigned char Reg_Data0, unsigned char Reg_Data1, unsigned char Reg_Data2 ){

	CS5460_CSLow;

	CS5460_Send( cmdRegWrite | Reg_Address );
	CS5460_Send( Reg_Data0 );
	CS5460_Send( Reg_Data1 );
	CS5460_Send( Reg_Data2 );  

	CS5460_CSHigh;

}
















