
/*=========================================================
                    ���ʼ�       ----���޹�˾
��    ����V1.0
����ʱ�䣺2010��7��26��
��Ȩ���У�2010--2099,�ӱ��Ƽ���ѧ  ��Ϣ��ѧ�빤��ϵ  ��־��
  					                      �ֻ���13933120973
									   Email��KDMZY@163.com
  �� �� ����C_Main.c
  ��    ��: ��־��
  ��    ��: V1.0
  �������: 2010��7��29��
  ������Ϣ: ���ʼ�
  ����˵��: оƬ���ͣ�STC12C5604AD  ��Ƶ��11.0592MHz       
  �����б�: 
  			1��ϵͳ��ʼ��
			2��CS5460оƬ��ʼ��������
			3��UART0���п��շ�����
			4��6ͨ���ĵ�������Чֵ����ѹ����Чֵ
			5��485�շ�����
  ��ʷ��¼:   
            1����    ��:
               ��    ��:
               �޸�����:
=========================================================*/

/*=============ͷ�ļ�����================================*/
#include "STC12C5620AD_H.h"

#include "H_STC12C5404ADMdu.H"
#include "H_STCE2PROM.h"
#include "H_Interface.h"
#include "H_CS5460.h"
#include "intrins.h"

/*=======================================================================
				ϵͳ������������
=======================================================================*/
		  bit Flag_10ms   = 0;		// 10ms��־
		 // bit Flag_100ms  = 0;  	// 100ms��־
		 // bit Flag_1000ms = 0;  	// 1000ms��־
		 // bit Flag_5s     = 0;      // 5���־

//unsigned char Counter_10ms   = 0;   // 10ms������
//unsigned char Counter_100ms  = 0;  	// 100ms������
//unsigned char Counter_1000ms = 0; 	// 1000ms������


#define conSysPowerOnState  0x00	
#define conSysIDLEState     0x01  	
#define conSysCabState      0x02    

unsigned char System_State   = conSysPowerOnState;
unsigned  int State_Timer    = 0;
		  bit SystemLED_Flag;

/*=======================================================================
�����ϴ���ʽ���£�
[@ZZ$D#YY-XX:0000.00V|0000.00A|0000.00W|0000.00Q|0000.00@|0000.00H|0000.00S|CRC]
1����'['��ʼ����']'����
2��@�ӻ���ַ
3��#ͨ����
4��������ţ�00����ѹ
             01������
			 02������
			 03����������
			 04����λ���
			 05��Ƶ��
			 06������
			 07��Ԥ��
			 08��Ԥ��
			 09��Ԥ��
			 FF����ʾ����ȫ������
	ÿ���������ù̶���ʽ����0000.00��λ�����������ԡ�|������
	���������ľ��巶Χ���£�
		��    ѹ��0��45.0V
		��    ����0��5A
		�� �� ֵ��0��2250W

5����[]����������������ݵĺ�Ȼ��ת��ΪASCII�룬��Χ��000~256

[@ZZ$R# R-XX:FFFFFF|CRC]:��ʾ�ϴ���ZZ��Ƭ����XX��ַ�Ĵ���������
[@ZZ$S#  ACK       |CRC]��
[@ZZ$S# nACK       |CRC]��
=======================================================================*/
/*=======================================================================
�����´���ʽ���£�
0123456789ABCDEF
[@ZZ$D#YY-XX       CRC]����ʾ��ȡZZ��ַ��Ƭ����YYͨ���ĵ�XX�����
				         ��XX=FFʱ��ʾ��һ�λ�ȡ��YYͨ�������в���
[@ZZ$R#R-XX        CRC]����ʾ��ȡF��ַ��Ƭ����XX��ַ�Ĵ���������

[@ZZ$W#R-XX|FFFFFF CRC]����ʾдZZ��ַ��Ƭ����XX�Ĵ���������

[@ZZ$C# 0-VIDC      CRC]����ʾдZZ��ַ��Ƭ���ĵ�ѹ��������DCУ׼
[@ZZ$C# 1-VIAC      CRC]����ʾдZZ��ַ��Ƭ���ĵ�ѹ��������ACУ׼
[@ZZ$C# 2-VIGain    CRC]����ʾдZZ��ַ��Ƭ���ĵ�ѹ������������У׼
[@ZZ$C# 3-VIOffset  CRC]����ʾдZZ��ַ��Ƭ���ĵ�ѹ��������ƫ��У׼

[@ZZ$S#T-Cnn       CRC]����ʾдZZ��ַ��Ƭ���������Զ��ϴ����ܵ�����
                         nn��ʾʱ������λ��
				         nn=00ʱ��ʾ��ֹ�Զ������ϴ�����
[@ZZ$S#S-4800,8,1,0CRC]�����пڲ���������
[@ZZ$S#A-XX        CRC]������ZZ��Ƭ����ͨѶ��ַΪXX
=======================================================================*/
unsigned char idata Device_Address;    // ������ַ

#define con_UART0TranMaxNum      38                          //  30
unsigned char idata UART0_TranBuf[con_UART0TranMaxNum]; //  = { "[ 0000.00V|0000.00A|0000.00W|0000.00Q|0000.00@|0000.00H|0000.00S|]" }; // ���п�0�ķ��ͻ�����
unsigned char idata UART0_TranMaxNum;
unsigned char idata UART0_TranCounter; 		// ���п�0�ķ��ͼ�����

#define con_UART0ReceMaxNum	   7
unsigned char  data UART0_ReceBuf[con_UART0ReceMaxNum]; 		// ���п�0���ջ�����
unsigned char idata UART0_ReceCounter; 		// ���п�0�Ľ��ռ�����
		  bit       UART0_ReceOKFlag = 0;	// ���п�0�Ľ��պñ�־
unsigned char idata UART0_TranState;        // ���п��ϴ�״̬��
unsigned char idata UART0_TranChannel;      // Ҫ�ϴ���ͨ����

unsigned char code con_HexCode[]  = { "0123456789ABCDEF" }; 
unsigned char idata CS5460_Channel = 0;

/*
unsigned char xdata Measure_Buf[8][3][8] = {    // ���ݽ��������
											// 0ͨ��
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0',
											// 1ͨ�� 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											// 2ͨ��
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											// 3ͨ��
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0',
											// 4ͨ�� 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											// 5ͨ��
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											// 6ͨ��
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											// 7ͨ��
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0'
										};

		*/


unsigned  char	 xdata  Measure_Buf[8][4]  = {
											   0x00 ,0x00 ,0x00 ,0x00 ,	   //1·��ѹֵ, ����ֵ	//����
                                               0x00 ,0x00 ,0x00 ,0x00 ,
										       0x00 ,0x00 ,0x00 ,0x00 ,
										       0x00 ,0x00 ,0x00 ,0x00 ,
										       0x00 ,0x00 ,0x00 ,0x00 ,
									           0x00 ,0x00 ,0x00 ,0x00 ,
											   0x00 ,0x00 ,0x00 ,0x00 ,
										       0x00 ,0x00 ,0x00 ,0x00 	  //8 ·
                                              };


unsigned  int xdata    measure_ma[8][2]={
										    	0x0000 ,0x0000 ,	   //1·��ѹ�� �� ������	
                                                0x0000 ,0x0000 ,
										        0x0000 ,0x0000 ,
										        0x0000 ,0x0000 ,
										        0x0000 ,0x0000 ,
									            0x0000 ,0x0000 ,
												0x0000 , 0x0000,
												0x0000 , 0x0000
											};


//unsigned  char xdata    measure_sys[6][4]={
//										    	0x00 ,0x00 ,0x00 ,0x00 ,	   //1·��ѹ ϵ����   �� ��	ϵ��
 //                                               0x00 ,0x00 ,0x00 ,0x00 ,								
//										        0x00 ,0x00 ,0x00 ,0x00 ,
//										        0x00 ,0x00 ,0x00 ,0x00 ,
//										        0x00 ,0x00 ,0x00 ,0x00 ,
//									            0x00 ,0x00 ,0x00 ,0x00 
//											};

//unsigned  int   idata	  v_sys[6]={ 0xd9c7 ,0xd9c7 ,0xd9c7,0xd9c7,0xd9c7,0xd9c7  };	//��·��ѹϵ��	  r=215kϵ��=d9c7  (26000*65536/30563)
  unsigned  int   idata	  v_sys= 0xd9c7 ;


//	float code conVoltage_Cof = (25000.0*0.98* 5.0 / 16777216.0);
	//	float code conCurrent_Cof = (25000.0 * 5.0 / 16777216.0);
	//	float code   conPower_Cof = (250.0*250.0 / 8388608.0);

//	    unsigned  int  code   conVoltage_sys=0xf522;   	 //	    25400*65536/26526		=0xf522			r=250k	
unsigned   int	 code    conCurrent_sys=0x4ed; 		 // 408*65536/52ce		  a=4.08		   ma=52ce
unsigned char xdata Parameter_Buf[ 6 * 3 + 1 ]; // ����������

unsigned char idata Read_State = 0;

/*=======================================================================
			CS5460��־λ����
=======================================================================*/
unsigned char bdata CS5460_Flag0;
sbit DRDY_Flag = CS5460_Flag0^7;   // ������Ч��־ �������򵥴�ת��ģʽ�±����������Ѿ���������У׼ģʽ�±�ʾУ׼�����Ѿ�д�뵽��Ӧ�ļĴ���
sbit EOUT_Flag = CS5460_Flag0^6; 
sbit EDIR_Flag = CS5460_Flag0^5; 
sbit CRDY_Flag = CS5460_Flag0^4; 
sbit MATH_Flag = CS5460_Flag0^3; 
//sbit RES_Flag  = CS5460_Flag0^2; 
sbit IOR_Flag  = CS5460_Flag0^1; 
sbit VOR_Flag  = CS5460_Flag0^0; 

unsigned char bdata CS5460_Flag1;
sbit PWOR_Flag = CS5460_Flag1^7; 
sbit IROR_Flag = CS5460_Flag1^6; 
sbit VROR_Flag = CS5460_Flag1^5; 
sbit EOR_Flag  = CS5460_Flag1^4; 
sbit EOOR_Flag = CS5460_Flag1^3; 
//sbit RES_Flag  = CS5460_Flag1^2; 
sbit ID3_Flag  = CS5460_Flag1^1; 
sbit ID2_Flag  = CS5460_Flag1^0;

unsigned char bdata CS5460_Flag2;
sbit ID1_Flag = CS5460_Flag2^7; 
sbit ID0_Flag = CS5460_Flag2^6; 
sbit WDT_Flag = CS5460_Flag2^5; 
sbit VOD_Flag = CS5460_Flag2^4; 
sbit IOD_Flag = CS5460_Flag2^3; 
sbit LSD_Flag = CS5460_Flag2^2; 
//sbit 0_Flag   = CS5460_Flag2^1; 
sbit nIC_Flag = CS5460_Flag2^0;

long idata CS5460_Status;  // CS5460��״̬��

unsigned char idata CS5460_CabNum;  
										
//unsigned char code con_ChannelChange[] = { 1, 4, 5, 7, 3 , 6 };

//unsigned char code con_ChannelChange[] = { 1, 4, 5, 2, 3 , 6 };


//unsigned char code con_ChannelChange[] = { 1,2, 3, 4, 5 , 6 };
unsigned char code con_ChannelChange[] = { 0,1,2, 3, 4, 5 , 6,7 };

unsigned char idata Output_Channel;



 void read_vsys(void) ;
 void vself_cab(void) ;

void delaynus (unsigned   char   n)
  { unsigned  char i;
      for(i=0 ;i<=n ;i++) 
	  {_nop_();
	   _nop_();
	   }
	    
  }



/*=======================================================================
�������ƣ�strTostr
��������: �ַ������ͺ���
���������
�����������
�� �� ֵ����
����˵����
=======================================================================*/
/*
void strTostr( unsigned char *srcP, unsigned char *dstP, unsigned char Number ){

	unsigned char i;

	for( i = 0; i < Number; i++ ){ *srcP++ = *dstP++; }

}

 */




unsigned   char     strTostr( unsigned char *srcP, unsigned char *dstP, unsigned char Number ){

	unsigned char i ,temp  =0;
	for( i = 0; i < Number; i++ )
     { 
	   temp += *dstP;
	   *srcP++ = *dstP++; 
	   
      }
	return(temp);

}





/*=======================================================================
�������ƣ�HexASCIIToBin
�������ܣ�
���������
�����������
�� �� ֵ����
����˵����
=======================================================================*/
unsigned char HexASCIIToBin( unsigned char CVT_Data ){

	if( CVT_Data >= '0' && CVT_Data <= '9' ){ return ( CVT_Data - '0'); }
	else                                    { return ( CVT_Data - 'A' + 10 ); }

}

/*=======================================================================
�������ƣ�LongToASCII
�������ܣ�������ת��ΪASCII
���������
�����������
�� �� ֵ����
����˵����
=======================================================================*/
void LongToASCII( unsigned char *p, unsigned long CVT_Data, unsigned char Type_Flag ){

	*p++ = ' ';
	*p++ = con_HexCode[ CVT_Data / 100000 ];
	*p++ = con_HexCode[ CVT_Data % 100000 / 10000 ];
	if( Type_Flag == 'I' ){ *p++ = '.'; }
	*p++ = con_HexCode[ CVT_Data % 10000 / 1000 ];
	*p++ = con_HexCode[ CVT_Data % 1000 / 100 ];
	if( Type_Flag == 'V' || Type_Flag == 'P' ){ *p++ = '.'; }
	*p++ = con_HexCode[ CVT_Data % 100 / 10 ];
	*p++ = con_HexCode[ CVT_Data % 10 ];

}

/*=======================================================================
�������ƣ�UART0_UnPacket
�������ܣ����пڽ������
�����������
�����������
�� �� ֵ����
����˵����
=======================================================================*/
/*=======================================================================
�����´���ʽ���£�
0123456789ABCDEF0123456
[@0F$D#01-00        CR]����ʾ��ȡZZ��ַ��Ƭ����YYͨ���ĵ�XX�����
				         ��XX=FFʱ��ʾ��һ�λ�ȡ��YYͨ�������в���
[@0F$R# R-0C        CR]����ʾ��ȡF��ַ��Ƭ����XX��ַ�Ĵ���������

[@ZZ$W# R-XX|FFFFFF CR]����ʾдZZ��ַ��Ƭ����XX�Ĵ���������

[@ZZ$S# T-Cnn       CR]����ʾдZZ��ַ��Ƭ���������Զ��ϴ����ܵ�����
                         nn��ʾʱ������λ��
				         nn=00ʱ��ʾ��ֹ�Զ������ϴ�����
[@ZZ$S# S-4800,8,1,0CR]�����пڲ���������
[@ZZ$S# A-XX        CR]������ZZ��Ƭ����ͨѶ��ַΪXX
[FDF]
[FCX]
01234
=======================================================================*/
/*
void UART0_UnPacket( void ){	

	switch( UART0_ReceBuf[2] ){
		case 'D': // Ҫ���ݵ�����
				UART0_TranChannel   = HexASCIIToBin( UART0_ReceBuf[3] );
				if( UART0_TranChannel < 6 ){
				    UART0_TranState = 'D';
				}				 
				break;
		case 'C': 
				CS5460_CabNum = HexASCIIToBin( UART0_ReceBuf[3] );
				UART0_TranState = 'A'; // �ظ���ȷӦ������
				System_State = conSysCabState; State_Timer = 0;  
				break;
		default: UART0_TranState = 'Z'; break;

	}

}

*/



void UART0_UnPacket( void ){	

	switch(	 UART0_ReceBuf[3]){
		case   0x03: // Ҫ���ݵ�����
			//	UART0_TranChannel   = HexASCIIToBin( UART0_ReceBuf[3] );
			//	if( UART0_TranChannel < 6 ){
			//	    UART0_TranState = 'D';
			//	}	
			 	UART0_TranState = 'D';
				break;
		case 0x05: 	   //ϵͳ�궨
			              //	CS5460_CabNum = HexASCIIToBin( UART0_ReceBuf[3] );
                CS5460_CabNum = UART0_ReceBuf[4]; // ��con_CabTab[]	   �ж�Ӧ������
				UART0_TranState = 'C'; // �ظ���ȷӦ������
				System_State = conSysCabState; State_Timer = 0;  //
		
				break;	 //
		case 0x06: 
		          	vself_cab( );
					break;

		default: UART0_TranState = 'Z'; break;

	}

}





/*=======================================================================
�������ƣ�UART0_Packet
�������ܣ������п�Ҫ���͵����ݰ� ���Զ���������
���������
�����������
�� �� ֵ����
����˵����
=======================================================================*/
/*
void UART0_Packet( void ){

	UART0_TranBuf[0] = '[';					  // ������ʼ�ַ�
	UART0_TranBuf[1] = Device_Address;  // �����ַ
	// �ϴ����ݴ������
	switch( UART0_TranState ){		
		case 'D': // �ϴ��ɼ�������������
		        UART0_TranBuf[ 2] = con_HexCode[ UART0_TranChannel ];
				// ȫ�������ϴ�
				strTostr( &UART0_TranBuf[3],  &Measure_Buf[UART0_TranChannel][0], 8 );
				strTostr( &UART0_TranBuf[11], &Measure_Buf[UART0_TranChannel][1], 8 );
			//	strTostr( &UART0_TranBuf[19], &Measure_Buf[UART0_TranChannel][2], 8 );
				UART0_TranBuf[19] = ']';
				UART0_TranMaxNum = 20;
				break;	 
		case 'C':  // �ϴ����״̬ [@ZZ$C# XOK     |CRC]��
				UART0_TranBuf[ 2] = 'C'; 
				UART0_TranBuf[ 3] = con_HexCode[ CS5460_CabNum ];
				UART0_TranBuf[ 4] = 'O'; 
				UART0_TranBuf[ 5] = 'K'; 
				UART0_TranBuf[ 6] = ']'; 
				UART0_TranMaxNum = 7;
				break;
		case 'A':  // �ϴ���ȷӦ��״̬ [@ZZ$S# ACK|CRC]��
				UART0_TranBuf[ 2] = 'A'; 
				UART0_TranBuf[ 3] = 'C'; 
				UART0_TranBuf[ 4] = 'K'; 
				UART0_TranBuf[ 5] = ']'; 
				UART0_TranMaxNum = 6;
				break;
		case 'Z':  // �ϴ��޷�Ӧ��״̬ 
				UART0_TranBuf[ 2] = 'n'; 
				UART0_TranBuf[ 3] = 'A'; 
				UART0_TranBuf[ 4] = 'C'; 
				UART0_TranBuf[ 5] = 'K'; 					 
				UART0_TranBuf[ 6] = ']';
				UART0_TranMaxNum = 7;
				break;
		default: break;
	}
    UART0_TranCounter = 0;
	Max485_Tran;             // 485����ʹ��
	TI = 1;	 // �����ݷ��ͳ�ȥ
	UART0_TranState = 0;

}

  */



void UART0_Packet( void ){
	 unsigned  char	  chkdata;
	UART0_TranBuf[0] = 0x6c;					  // ������ʼ�ַ�
	UART0_TranBuf[1] = 0x63;
	UART0_TranBuf[2] = Device_Address;  // �����ַ
	// �ϴ����ݴ������
	switch( UART0_TranState ){		
		case 'D': // �ϴ��ɼ�������������
		      //  UART0_TranBuf[ 2] = con_HexCode[ UART0_TranChannel ];
				// ȫ�������ϴ�
				  chkdata= strTostr( &UART0_TranBuf[3],  &Measure_Buf[0], 32 );//20140428 tan mayee
			     //  chkdata= strTostr( &UART0_TranBuf[3],  &Measure_Buf[0], 24 );
			         // 	strTostr( &UART0_TranBuf[11], &Measure_Buf[UART0_TranChannel][1], 8 );

			//	strTostr( &UART0_TranBuf[19], &Measure_Buf[UART0_TranChannel][2], 8 );
			  	chkdata += UART0_TranBuf[2];
				UART0_TranBuf[35] = chkdata;	  //chk
				UART0_TranBuf[36] = 0x0d;
				UART0_TranMaxNum = 37;
			   // chkdata += UART0_TranBuf[2];
			//	UART0_TranBuf[27] = chkdata;	  //chk
			//	UART0_TranBuf[28] = 0x0d;
			//	UART0_TranMaxNum = 29;
				break;	 
		case 'C':  // �ϴ����״̬ [@ZZ$C# XOK     |CRC]��
			//	UART0_TranBuf[ 3] = 'C'; 
			//	UART0_TranBuf[ 4] = con_HexCode[ CS5460_CabNum ];
			//	UART0_TranBuf[ 5] = 'O'; 
			//	UART0_TranBuf[ 6] = 'K'; 
			//	UART0_TranBuf[ 7] = ']'; 
				UART0_TranBuf[ 3] = Parameter_Buf[17]; 
				UART0_TranBuf[ 4] = Parameter_Buf[16];
				UART0_TranBuf[ 5] = Parameter_Buf[15]; 
				UART0_TranBuf[ 6] = Parameter_Buf[11]; 
				UART0_TranBuf[ 7] = Parameter_Buf[10];
				UART0_TranBuf[ 8] = Parameter_Buf[9];



				UART0_TranMaxNum = 9;
				break;
		case 'A':  // �ϴ���ȷӦ��״̬ [@ZZ$S# ACK|CRC]��
				UART0_TranBuf[ 3] = 'A'; 
				UART0_TranBuf[ 4] = 'C'; 
				UART0_TranBuf[ 5] = 'K'; 
				UART0_TranBuf[ 6] = ']'; 
				UART0_TranMaxNum = 7;
				break;
		case 'Z':  // �ϴ��޷�Ӧ��״̬ 
				UART0_TranBuf[ 3] = 'n'; 
				UART0_TranBuf[ 4] = 'A'; 
				UART0_TranBuf[ 5] = 'C'; 
				UART0_TranBuf[ 6] = 'K'; 					 
				UART0_TranBuf[ 7] = ']';
				UART0_TranMaxNum = 8;
				break;
		default: break;
	}
    UART0_TranCounter = 0;
	Max485_Tran;             // 485����ʹ��
	TI = 1;	 // �����ݷ��ͳ�ȥ
	UART0_TranState = 0;

}




/*=======================================================================
�������ƣ�Get_Parameter
�������ܣ���ȡ����ֵ, ͬʱ����CS5460
�����������
�����������
�� �� ֵ����
����˵����
=======================================================================*/
void Get_Parameter( void ){

	unsigned char i;
	unsigned  int Parameter_Address;

	Parameter_Address = 0;

	for( i = 0; i < (6 * 3 + 1); i++ ){
		Parameter_Buf[i] = ISP_Byte_Read( Parameter_Address++ );
	}

	// ���������ȷ ������CS5460
	if( Parameter_Buf[ ( 6*3 ) ] == 0x00 ){ 
		CS5460_WriteReg( addIDCOffset, Parameter_Buf[ 2 ], Parameter_Buf[ 1 ], Parameter_Buf[ 0 ] );
		CS5460_WriteReg( addIGain,     Parameter_Buf[ 5 ], Parameter_Buf[ 4 ], Parameter_Buf[ 3 ] );
		CS5460_WriteReg( addVDCOffset, Parameter_Buf[ 8 ], Parameter_Buf[ 7 ], Parameter_Buf[ 6 ] );
		CS5460_WriteReg( addVGain,     Parameter_Buf[ 11 ], Parameter_Buf[ 10 ], Parameter_Buf[ 9 ] );
		CS5460_WriteReg( addIACOffset, Parameter_Buf[ 14 ], Parameter_Buf[ 13 ], Parameter_Buf[ 12 ] );
		CS5460_WriteReg( addVACOffset, Parameter_Buf[ 17 ], Parameter_Buf[ 16 ], Parameter_Buf[ 15 ] );
	}

}

 /*
void Get_Parameter( void ){

	unsigned char i;
	unsigned  int Parameter_Address;

	Parameter_Address = 0;

	for( i = 0; i < (6 * 3 + 1); i++ ){
		Parameter_Buf[i] = ISP_Byte_Read( Parameter_Address++ );
	}

	// ���������ȷ ������CS5460
	if( Parameter_Buf[ ( 6*3 ) ] == 0x00 ){ 
		CS5460_WriteReg( addIDCOffset, Parameter_Buf[ 0 ], Parameter_Buf[ 1 ], Parameter_Buf[ 2 ] );
		CS5460_WriteReg( addIGain,     Parameter_Buf[ 3 ], Parameter_Buf[ 4 ], Parameter_Buf[ 5 ] );
		CS5460_WriteReg( addVDCOffset, Parameter_Buf[ 6 ], Parameter_Buf[ 7 ], Parameter_Buf[ 8 ] );
		CS5460_WriteReg( addVGain,     Parameter_Buf[ 9 ], Parameter_Buf[ 10 ], Parameter_Buf[ 11 ] );
		CS5460_WriteReg( addIACOffset, Parameter_Buf[ 12 ], Parameter_Buf[ 13 ], Parameter_Buf[ 14 ] );
		CS5460_WriteReg( addVACOffset, Parameter_Buf[ 15 ], Parameter_Buf[ 16 ], Parameter_Buf[ 17 ] );
	}

}

 */

/*=======================================================================
�������ƣ�Save_Parameter
�������ܣ��������ֵ
�����������
�����������
�� �� ֵ����
����˵����
=======================================================================*/
void Save_Parameter( void ){

	unsigned char i;
	unsigned  int Parameter_Address;

	Parameter_Address = 0;
	Sector_Erase( Parameter_Address );

	CS5460_Status = CS5460_ReadReg( addIDCOffset );
	Parameter_Buf[ 0 ]  = CS5460_Status;	 
	Parameter_Buf[ 1 ]  = ( CS5460_Status >> 8 );
	Parameter_Buf[ 2 ]  = ( CS5460_Status	>> 16 );

	CS5460_Status = CS5460_ReadReg( addIGain );
	Parameter_Buf[ 3 ]  = CS5460_Status;	 
	Parameter_Buf[ 4 ]  = ( CS5460_Status >> 8 );
	Parameter_Buf[ 5 ]  = ( CS5460_Status	>> 16 );

	CS5460_Status = CS5460_ReadReg( addVDCOffset );
	Parameter_Buf[ 6 ]  = CS5460_Status;	 
	Parameter_Buf[ 7 ]  = ( CS5460_Status >> 8 );
	Parameter_Buf[ 8 ]  = ( CS5460_Status	>> 16 );


	CS5460_Status = CS5460_ReadReg( addVGain );
	Parameter_Buf[ 9 ]  = CS5460_Status;	 
	Parameter_Buf[ 10 ]  = ( CS5460_Status >> 8 );
	Parameter_Buf[ 11 ]  = ( CS5460_Status	>> 16 );

	CS5460_Status = CS5460_ReadReg( addIACOffset );
	Parameter_Buf[ 12 ]  = CS5460_Status;	 
	Parameter_Buf[ 13 ]  = ( CS5460_Status >> 8 );
	Parameter_Buf[ 14 ]  = ( CS5460_Status	>> 16 );

	CS5460_Status = CS5460_ReadReg( addVACOffset );
	Parameter_Buf[ 15 ]  = CS5460_Status;	 
	Parameter_Buf[ 16 ]  = ( CS5460_Status >> 8 );
	Parameter_Buf[ 17 ]  = ( CS5460_Status	>> 16 );

	Parameter_Buf[ 18 ]  = 0x00;

	for( i = 0; i < (6 * 3 + 1); i++ ){
		ISP_Byte_Write( Parameter_Address++, Parameter_Buf[i] );
	}

}


/*=======================================================================
�������ƣ�System_PowerOn
�������ܣ�ϵͳ���ϵ���Ҫ���������
�����������
�����������
�� �� ֵ����
����˵����1����ʱ1S
		  2��ָʾ����0.3S��˸
		  3��ʹ�ܵ�ѹͨ��������ͨ�����л���0ͨ��
		  4����λCS5460
		  5����ת��ϵͳ����״̬
=======================================================================*/
void System_PowerOn( void ){

	// ָʾ����0.3S��˸
	if( (State_Timer % 30) == 0 ) { SystemLED_Flag = ~SystemLED_Flag; }

	// ʹ�ܵ�ѹͨ��������ͨ�����л���0ͨ�� ��λCS5460
	if( State_Timer == 100 ){
		CS5460_RSTLow;
		CS5460_Channel = 0;
		CD4051_A = 0;
	   	//CD4051_A = 1;
		CD4051_B = 0;
		CD4051_C = 0;
		CD4051_EN0 = 0;
		CD4051_EN1 = 0;

	}
	if( State_Timer == 103 ){ CS5460_RSTHigh; } // ��λ���

	// ͬ��CS5460�Ĵ��п�
	if( State_Timer == 104 ){
		CS5460_CSLow;
		CS5460_Send( cmdSYNC1 );  // 
		CS5460_Send( cmdSYNC1 );  // 
		CS5460_Send( cmdSYNC1 );  // 
		CS5460_Send( cmdSYNC0 );  // 
		CS5460_CSHigh;
	
		CS5460_WriteReg( addConfig, 0x00, 0x10, 0x63 );	// ����Config
	//	CS5460_WriteReg( addConfig, 0x00, 0x10, 0x03 );	// ����Config
	//	Get_Parameter();   //���궨����	 ��cs5460��
	    read_vsys(); //��eeprom   �ж���ϵ����v������sys����������
	}					   

	if( State_Timer == 106 ){			
		CS5460_SendCmd( cmdStartCVTCtinu );  // ������������ת������  
		// ��ȡ�豸��ַ
		Device_Address = P1;
		Device_Address >>= 4;
	//	Device_Address = Device_Address & 0x03;//p1.4 p1.5
	//	Device_Address =  con_HexCode[Device_Address];
		System_State = conSysIDLEState; State_Timer = 0;  // 1��� ��ת��ϵͳ����״̬
		Max485_Rece;
	}

}

 /*
void System_PowerOn( void ){

	// ָʾ����0.3S��˸
	if( (State_Timer % 30) == 0 ) { SystemLED_Flag = ~SystemLED_Flag; }

	// ʹ�ܵ�ѹͨ��������ͨ�����л���0ͨ�� ��λCS5460
	if( State_Timer == 100 ){
		CS5460_RSTLow;
		CS5460_Channel = 0;
		CD4051_A = 1;
		CD4051_B = 0;
		CD4051_C = 0;
		CD4051_EN0 = 0;
		CD4051_EN1 = 0;

	}
	if( State_Timer == 103 ){ CS5460_RSTHigh; } // ��λ���

	// ͬ��CS5460�Ĵ��п�
	if( State_Timer == 104 ){
		CS5460_CSLow;
		CS5460_Send( cmdSYNC1 );  // 
		CS5460_Send( cmdSYNC1 );  // 
		CS5460_Send( cmdSYNC1 );  // 
		CS5460_Send( cmdSYNC0 );  // 
		CS5460_CSHigh;
	
		CS5460_WriteReg( addConfig, 0x00, 0x10, 0x63 );	// ����Config
	}

	if( State_Timer == 106 ){			
		CS5460_SendCmd( cmdStartCVTCtinu );  // ������������ת������  
		// ��ȡ�豸��ַ
		Device_Address = P1;
		Device_Address >>= 4;
	//	Device_Address =  con_HexCode[Device_Address];
		System_State = conSysIDLEState; State_Timer = 0;  // 1��� ��ת��ϵͳ����״̬
		Max485_Rece;
	}

}


 */

/*=======================================================================
�������ƣ�System_IDLE
�������ܣ�ϵͳ������Ҫ���������
�����������
�����������
�� �� ֵ����
����˵����1��ָʾ����1S��˸
		  2��ÿһ���л�һ��ͨ��������CS5460ת��һ�Σ���ȡһ��ת����������ѽ�����µ���Ӧ�Ļ�����
		  3������յ����п����ݰ�������������
		  4��ÿһ�����ݴ��һ�Σ��������ϴ�һ�Σ������ã�
=======================================================================*/
/*
void System_IDLE( void ){	

	  bit Plus_Flag;
	float Temp_f;
	unsigned long Temp_l;

	// ÿ10ms��һ��CS5460��״̬�֣�����״̬�����ж���һ���Ĳ���
	CS5460_Status = CS5460_ReadReg( addStatus );
	CS5460_Flag2  = CS5460_Status;	 
	CS5460_Flag1  = ( CS5460_Status >> 8 );
	CS5460_Flag0  = ( CS5460_Status	>> 16 );
	if( DRDY_Flag == 1 ){ // ��ȡ��ǰת�����		  	
		Read_State++;
		if( Read_State != 3 ){
			// �����־
			CS5460_WriteReg( addStatus, CS5460_Flag0, CS5460_Flag1, CS5460_Flag2 );
			Temp_l = CS5460_ReadReg( addVRMS );
			Temp_l = CS5460_ReadReg( addIRMS );
			Temp_l = CS5460_ReadReg( addP );
		}
		if( Read_State >= 3 ){ Read_State = 0;
			// ��ȡ��ѹ����Чֵ			
			Temp_f = CS5460_ReadReg( addVRMS ); // ת��Ϊ������
			Temp_f *= conVoltage_Cof;		// ת��ΪС�� ����ϵ��
			Temp_l  = Temp_f;			// ת��Ϊ��������
			// ��ֵ���Ӧͨ������Ӧ�����		
			LongToASCII( &Measure_Buf[CS5460_Channel][0][0], Temp_l, 'V' );
			// ��ȡ��������Чֵ
			Temp_f = CS5460_ReadReg( addIRMS );	// ת��Ϊ������
			Temp_f *= conCurrent_Cof;		// ת��ΪС�� ����ϵ��
			Temp_l  = Temp_f;			// ת��Ϊ��������
			// ��ֵ���Ӧͨ������Ӧ�����
			LongToASCII( &Measure_Buf[CS5460_Channel][1][0], Temp_l, 'I' );
			// ��ȡ����ֵ
		  	Temp_l = CS5460_ReadReg( addP );
			Plus_Flag = 0;
			if( Temp_l & 0x00800000 ){ 
				Plus_Flag = 1; 
				Temp_l &= 0x007FFFFF; 
			}
			Temp_f = Temp_l;	// ת��Ϊ������
			Temp_f *= conPower_Cof;		// ת��ΪС�� ����ϵ��
			Temp_l  = Temp_f;			// ת��Ϊ��������
			// ��ֵ���Ӧͨ������Ӧ�����
			LongToASCII( &Measure_Buf[CS5460_Channel][2][0], Temp_l, 'P' );
	 		if( Plus_Flag ) { Measure_Buf[CS5460_Channel][2][0] = '-'; }
			// �����־
			CS5460_WriteReg( addStatus, CS5460_Flag0, CS5460_Flag1, CS5460_Flag2 );
			// �л�ͨ��
			CS5460_Channel++;
			if( CS5460_Channel >= 6 ){ CS5460_Channel = 0; }
			//CS5460_Channel = 1;
			Output_Channel = con_ChannelChange[ CS5460_Channel ];
			CD4051_EN0 = 1; 
			CD4051_EN1 = 1;
			if( ( Output_Channel & 0x01 ) == 0x01 ){ CD4051_A = 1; }
			else                                   { CD4051_A = 0; }
			if( ( Output_Channel & 0x02 ) == 0x02 ){ CD4051_B = 1; }
			else                                   { CD4051_B = 0; }
			if( ( Output_Channel & 0x04 ) == 0x04 ){ CD4051_C = 1; }
			else                                   { CD4051_C = 0; }
			CD4051_EN0 = 0; 
			CD4051_EN1 = 0;
		}
		
	}

	// 1s��
	if( ( State_Timer % 100 ) == 0 ){ State_Timer = 0;
		SystemLED_Flag = ~SystemLED_Flag;
	}

}  

 */



void System_IDLE( void ){	

//	  bit Plus_Flag;
//	float Temp_f;
	unsigned long Temp_l;


//	unsigned  char	 *p;

	// ÿ10ms��һ��CS5460��״̬�֣�����״̬�����ж���һ���Ĳ���
	CS5460_Status = CS5460_ReadReg( addStatus );
	CS5460_Flag2  = CS5460_Status;	 
	CS5460_Flag1  = ( CS5460_Status >> 8 );
	CS5460_Flag0  = ( CS5460_Status	>> 16 );
	if( DRDY_Flag == 1 ){ // ��ȡ��ǰת�����		  	
		Read_State++;
		if( Read_State != 3 ){
			// �����־
			CS5460_WriteReg( addStatus, CS5460_Flag0, CS5460_Flag1, CS5460_Flag2 );
			Temp_l = CS5460_ReadReg( addVRMS );
			Temp_l = CS5460_ReadReg( addIRMS );
			Temp_l = CS5460_ReadReg( addP );
		}
		if( Read_State >= 3 ){ Read_State = 0;
			// ��ȡ��ѹ����Чֵ			
		   //	Temp_f = CS5460_ReadReg( addVRMS ); // ת��Ϊ������
		   //	Temp_f *= conVoltage_Cof;		// ת��ΪС�� ����ϵ��
		  //	Temp_l  = Temp_f;		   	// ת��Ϊ��������

				Temp_l=CS5460_ReadReg(addVRMS);
				Temp_l=Temp_l>>8;			      // yong  2  zhi jie
			
				measure_ma[CS5460_Channel][0]=Temp_l;   // cun ri ma ���˵�ѹ�뵽 measure_ma[][] ��
		//		Temp_l=Temp_l* v_sys[CS5460_Channel];
				Temp_l=Temp_l* v_sys;
				Temp_l=Temp_l>>16;



			//	Temp_l=Temp_l* conVoltage_sys;
			//	Temp_l=Temp_l>>16;

			//	p=(unsigned  char *) (&Temp_l);
			//	UART0_TranBuf[4]=*p++;		  //add
			//	UART0_TranBuf[5]=*p++;		  //add
			//	UART0_TranBuf[6]=*p++;		  //add
			//	UART0_TranBuf[7]=*p++;		  //add



			// ��ֵ���Ӧͨ������Ӧ�����		
		//	LongToASCII( &Measure_Buf[CS5460_Channel][0][0], Temp_l, 'V' );
		     Measure_Buf[CS5460_Channel][1]=Temp_l;
             Measure_Buf[CS5460_Channel][0]=Temp_l>>8;	 //�����ѹֵ��measure_buf[][]�� 
             
			// ��ȡ��������Чֵ
		//	Temp_f = CS5460_ReadReg( addIRMS );	// ת��Ϊ������
		//	Temp_f *= conCurrent_Cof;		// ת��ΪС�� ����ϵ��
		//	Temp_l  = Temp_f;			// ת��Ϊ��������
			// ��ֵ���Ӧͨ������Ӧ�����
		//	LongToASCII( &Measure_Buf[CS5460_Channel][1][0], Temp_l, 'I' );
			// ��ȡ����ֵ
		   Temp_l=CS5460_ReadReg(addIRMS);
           Temp_l=Temp_l>>8;
		   Temp_l=Temp_l*conCurrent_sys;
		   Temp_l=Temp_l>>16;
           Measure_Buf[CS5460_Channel][3]=Temp_l;
           Measure_Buf[CS5460_Channel][2]=Temp_l>>8;  //����ֵ ���뵽measure_buf[][]�� 



		 // 	Temp_l = CS5460_ReadReg( addP );
		//	Plus_Flag = 0;
		//	if( Temp_l & 0x00800000 ){ 
		//		Plus_Flag = 1; 
		//		Temp_l &= 0x007FFFFF; 
		//	}
		//	Temp_f = Temp_l;	// ת��Ϊ������
		//	Temp_f *= conPower_Cof;		// ת��ΪС�� ����ϵ��
		//	Temp_l  = Temp_f;			// ת��Ϊ��������
			// ��ֵ���Ӧͨ������Ӧ�����
		//	LongToASCII( &Measure_Buf[CS5460_Channel][2][0], Temp_l, 'P' );
	 	//	if( Plus_Flag ) { Measure_Buf[CS5460_Channel][2][0] = '-'; }
			// �����־
			CS5460_WriteReg( addStatus, CS5460_Flag0, CS5460_Flag1, CS5460_Flag2 );
			// �л�ͨ��
			CS5460_Channel++;
			//if( CS5460_Channel >= 6 ){ CS5460_Channel = 0; }
			if( CS5460_Channel >= 8 ){ CS5460_Channel = 0; }
			//CS5460_Channel = 1;
			Output_Channel = con_ChannelChange[ CS5460_Channel ];
			CD4051_EN0 = 1; 
			CD4051_EN1 = 1;
			if( ( Output_Channel & 0x01 ) == 0x01 ){ CD4051_A = 1; }
			else                                   { CD4051_A = 0; }
			if( ( Output_Channel & 0x02 ) == 0x02 ){ CD4051_B = 1; }
			else                                   { CD4051_B = 0; }
			if( ( Output_Channel & 0x04 ) == 0x04 ){ CD4051_C = 1; }
			else                                   { CD4051_C = 0; }
			CD4051_EN0 = 0; 
			CD4051_EN1 = 0;
		}
		
	}

	// 1s��
	if( ( State_Timer % 100 ) == 0 ){ State_Timer = 0;
		SystemLED_Flag = ~SystemLED_Flag;
	}

}  





/*=======================================================================
�������ƣ�Wait_DRDY
��������: 
�����������
�����������
�� �� ֵ����
����˵����1��У׼�ڼ�ָʾ�Ƴ���
		  2��У׼��ɺ�ָʾ����3��,���ϴ�У׼�����Ϣ
		  3��У׼���ϵ������E2PROM��
=======================================================================*/
void Wait_DRDY( void ){
	CS5460_Status = CS5460_ReadReg( addStatus );
	CS5460_Flag2  = CS5460_Status;	 
	CS5460_Flag1  = ( CS5460_Status >> 8 );
	CS5460_Flag0  = ( CS5460_Status	>> 16 );
}
/*							 
unsigned char code con_CabTab[] = { (0xC1 | 0x01), (0xC1 | 0x05), (0xC1 | 0x02), (0xC1 | 0x06),
							  
									(0xD0 | 0x01), (0xD0 | 0x05), (0xD0 | 0x02), (0xD0 | 0x06)
								  };

 */
unsigned char code con_CabTab[] = { (0xC8 | 0x01), (0xC8 | 0x05), (0xC8 | 0x02), (0xC8 | 0x06),
							  
									(0xD0 | 0x01), (0xD0 | 0x05), (0xD0 | 0x02), (0xD0 | 0x06)
								  };


/*=======================================================================
�������ƣ�System_Cab
�������ܣ�ϵͳУ׼����
�����������
�����������
�� �� ֵ����
����˵����1��У׼�ڼ�ָʾ�Ƴ���
		  2��У׼��ɺ�ָʾ����3��,���ϴ�У׼�����Ϣ
		  3��У׼���ϵ������E2PROM��
=======================================================================*/
/*
void System_Cab( void ){

	SystemLED_Flag = con_LEDOn;

	// ����
	CS5460_WriteReg( addStatus, 0xFF, 0xFF, 0xFF );
	// 1������Power_Up����
	CS5460_SendCmd( cmdPowerUpHalt );
	// ����
	CS5460_WriteReg( addStatus, 0xFF, 0xFF, 0xFF );	

	CS5460_SendCmd( con_CabTab[ CS5460_CabNum ]  );
	DRDY_Flag = 0;
	while( DRDY_Flag == 0 ){
	   Wait_DRDY();
	}

	CS5460_WriteReg( addStatus, CS5460_Flag0, CS5460_Flag1, CS5460_Flag2 );

	Save_Parameter( );
	CS5460_SendCmd( cmdStartCVTCtinu );  // ������������ת������

	UART0_TranState = 'C';
	// Ϩ��LED
	SystemLED_Flag = con_LEDOff;
	// �л���ϵͳ����״̬
	System_State = conSysIDLEState; State_Timer = 0;

}

 */

/*=======================================================================
�������ƣ�System_Cab
�������ܣ�ϵͳУ׼����
�����������
�����������
�� �� ֵ����
����˵����1��У׼�ڼ�ָʾ�Ƴ���
		  2��У׼��ɺ�ָʾ����3��,���ϴ�У׼�����Ϣ
		  3��У׼���ϵ������E2PROM��
=======================================================================*/
void System_Cab( void ){

  //	SystemLED_Flag = con_LEDOn;
    System_LED  = 	con_LEDOn;

	CS5460_Channel=0;  //tong dap1 
	CD4051_A=1;
	CD4051_B=0;
	CD4051_C=0;	   // tong dao 1
	_nop_();
	_nop_();
	_nop_();
	_nop_();

	CD4051_EN0=0;
	CD4051_EN1=0; //4051  enable 

	_nop_();
	_nop_();
	_nop_();
	_nop_();

	_nop_();
	_nop_();
	_nop_();
	_nop_();
	delaynus (200) ;

	// ����
	CS5460_WriteReg( addStatus, 0xFF, 0xFF, 0xFF );

	_nop_();
	_nop_();
	_nop_();
	_nop_();
		delaynus (200) ;
	// 1������Power_Up����
	CS5460_SendCmd( cmdPowerUpHalt );

	_nop_();
	_nop_();
	_nop_();
	_nop_();
	// ����
	CS5460_WriteReg( addStatus, 0xFF, 0xFF, 0xFF );	
	_nop_();
	_nop_();
	_nop_();
	_nop_();
		delaynus (200) ;

	
	
	CS5460_SendCmd( con_CabTab[ CS5460_CabNum ]  );		  
    
	DRDY_Flag = 0;
	while( DRDY_Flag == 0 ){
	   Wait_DRDY();
	   WDT_CONTR = 0x38;	
	}

	CS5460_WriteReg( addStatus, CS5460_Flag0, CS5460_Flag1, CS5460_Flag2 );



     
	Save_Parameter( );
	CS5460_SendCmd( cmdStartCVTCtinu );  // ������������ת������

	UART0_TranState = 'C';
	// Ϩ��LED
	SystemLED_Flag = con_LEDOff;
	// �л���ϵͳ����״̬
	System_State = conSysIDLEState; State_Timer = 0;
	

}



 /*======================================================
    �Ա궨
   �Ѳɳ����� ��220v    �ı�׼      x=22000 *65536 /	   ��
   ��������   ���������ֽ�		Ȼ�����200 ��ʼ�ĵڶ�����
   212  ����01 
  =====================================================*/


 void vself_cab(void)
    {unsigned  char templ ,temph;
	 unsigned  int  temp_sys;
	 unsigned  int	  ee_address;
	     
	     System_LED  = 	con_LEDOn;
		 ee_address=0x200;
     	 Sector_Erase(ee_address);

/*	 	 for(i=0; i<=5; i++)
     	   {
		   	  temp_sys= 0x55f00000 / measure_ma[i][0] ;	//22000*65536
		      v_sys[i]=temp_sys;	  //���˵�ѹϵ��
			  templ=temp_sys;
			  temph=temp_sys>>8;
              ISP_Byte_Write(ee_address++ ,temph);//
		  	  ISP_Byte_Write(ee_address++ ,templ);	//
	       }
*/	
	 
		   	  temp_sys= 0x55f00000 / measure_ma[0][0] ;	//22000*65536
		      v_sys=temp_sys;	  //���˵�ѹϵ��
			  templ=temp_sys;
			  temph=temp_sys>>8;
              ISP_Byte_Write(ee_address++ ,temph);  //v_sys �� ��8λ���� 200h��
		  	  ISP_Byte_Write(ee_address++ ,templ);	// v_sys �ĵ�8λ���� 201h��
	       
	  	  ISP_Byte_Write(0x212 ,0x01);	 //eeprom   ��01h ���뵽 0x212�� 

	  
	}


 /*================================
  �ȶ�212��ַ�������Ƿ�01
  �� ���������ݷ����Ӧ��ϵ����



 ==================================*/

 void read_vsys(void)
   { unsigned  char i,	temp_data ,temp_datah ,temp_datal;
     unsigned int  temp_w;
     unsigned int ee_address;
	 ee_address=0x200;
     temp_data= ISP_Byte_Read( 0x212 );//��212�Ƿ���01h
	 if(temp_data==0x01)
	   /* {for(i=0;i<=5 ;i++)
		   { temp_datah = ISP_Byte_Read(ee_address++);
		     temp_datal=  ISP_Byte_Read(ee_address++);
			 temp_w=temp_datah;
			 temp_w<<=8;
			 temp_w+=temp_datal;
			 v_sys[i]=temp_w;	//����ϵ�����ڴ�������
		   }
		*/
	   {     temp_datah = ISP_Byte_Read(ee_address++);
		     temp_datal=  ISP_Byte_Read(ee_address++); //��flash���˵�ѹϵ����v_sys��
			 temp_w=temp_datah;
			 temp_w<<=8;
			 temp_w+=temp_datal;
			 v_sys=temp_w;	//����ϵ�����ڴ�������
       	}
     
   }


/*====================================================================       ===
�������ƣ�Output_Port
�������ܣ��˿�ˢ�º���
�����������
�����������
�� �� ֵ����
����˵������
=======================================================================*/
void Output_Port( void ){

	System_LED = SystemLED_Flag;

}

/*=======================================================================
�������ƣ�main
�������ܣ�
�����������
�����������
�� �� ֵ����
����˵������
=======================================================================*/
void main( void ){
	bit bdata biao_flag=0;
	Max485_Rece;             // 485����ʹ��
	System_LED = con_LEDOn;	 // ����ϵͳָʾ��
	Init_System( );

	while(1){ 
	    	WDT_CONTR = 0x38;
			if((STL==0 ) && (biao_flag==0) )
			  {	vself_cab( );
			    biao_flag=1;
			  }	 
		// ÿ10msҪ��������
		if( Flag_10ms ){ Flag_10ms = 0;		
		           WDT_CONTR = 0x38;
					// ��Ҫ��������
			if( UART0_ReceOKFlag ){ // ���пڽ��պ�
				UART0_UnPacket( );	// ���ô��пڽ������
				UART0_ReceOKFlag = 0; // ����պñ��
			}
			if( UART0_TranState ){ UART0_Packet(); }
			// ϵͳ״̬������
			if( State_Timer < 65530 ) { State_Timer++; }			
			switch( System_State ){
				case conSysPowerOnState: System_PowerOn(); break;
				case conSysIDLEState:    System_IDLE();    break;
				case conSysCabState:     System_Cab();     break;
				default: System_State = conSysPowerOnState; State_Timer = 0;  break;
			} // ϵͳ״̬���������

			// 10msˢ��һ������˿�
			Output_Port( );

		} // 10ms�¼��������
	}

}



/*=======================================================================
�������ƣ�ISR_INT0  �ⲿ�ж�0���жϷ���
�������ܣ�������������          
�����������
�����������
�� �� ֵ����
����˵������
=======================================================================*/
void ISR_INT0( void ) interrupt 0  using 1{
	
		 
}

/*=======================================================================
�������ƣ�ISR_Timer0  ��ʱ��T0�жϷ���
�������ܣ�ʵ��10ms��ʱ�ж�          
�����������
�����������
�� �� ֵ����
����˵������
=======================================================================*/
void ISR_Timer0( void ) interrupt 1  //using 1
{
			  
	TH0   = 0xD5;
	TL0   = 0x9D;

	Flag_10ms = 1;

} 

/*=======================================================================
�������ƣ�ISR_INT1  �ⲿ�ж�1���жϷ���
�������ܣ�����������          
�����������
�����������
�� �� ֵ����
����˵������
=======================================================================*/
void ISR_INT1( void ) interrupt 2  //using 1
{
	 
}

/*=======================================================================
�������ƣ�ISR_Timer1  ��ʱ��T1�жϷ���
�������ܣ�ʵ��1ms��ʱ�ж�          
�����������
�����������
�� �� ֵ����
����˵������
=======================================================================*/
void ISR_Timer1( void ) interrupt 3 // using 1
{

}

/*=======================================================================
�������ƣ�ISR_Serial0 ���п�0�жϷ���
�������ܣ�ϵͳ����      
�����������
�����������
�� �� ֵ����
����˵������
=======================================================================*/




void ISR_UART0( void ) interrupt 4 // using 1
{
    unsigned char Temp_Data;

	// �����ж�
    if( RI ){  RI = 0;
		if( UART0_ReceOKFlag == 0 ){
		    Temp_Data = SBUF;
		    if( Temp_Data == 0x4c ){	UART0_ReceCounter = 0; }
		    UART0_ReceBuf[ UART0_ReceCounter++ ] = Temp_Data;
		    if( UART0_ReceCounter == con_UART0ReceMaxNum ){
		        UART0_ReceCounter = 0;
		        if( (UART0_ReceBuf[1] == 0x57 )&& ((UART0_ReceBuf[3]==0x03) ||(UART0_ReceBuf[3]==0x05) ||(UART0_ReceBuf[3]==0x06))	   &&(UART0_ReceBuf[6]==0x0d)){	// ��������У��
					// ������ַУ��
					if( UART0_ReceBuf[2] == Device_Address ){
					    UART0_ReceOKFlag = 1; // ������,�ݲ���CRCУ��
					}			       
			    }
		    }
		}
	}

	// �����ж�
	if( TI ){ TI = 0; 
		if( UART0_TranCounter < UART0_TranMaxNum ){
		    delaynus(10);
			SBUF = UART0_TranBuf[ UART0_TranCounter++ ]; 			
		}
		else{
			Max485_Rece;             // 485����ʹ��
		}
	}

}





/*
void ISR_UART0( void ) interrupt 4 // using 1
{
    unsigned char Temp_Data;

	// �����ж�
    if( RI ){  RI = 0;
		if( UART0_ReceOKFlag == 0 ){
		    Temp_Data = SBUF;
		    if( Temp_Data == '[' ){	UART0_ReceCounter = 0; }
		    UART0_ReceBuf[ UART0_ReceCounter++ ] = Temp_Data;
		    if( UART0_ReceCounter == con_UART0ReceMaxNum ){
		        UART0_ReceCounter = 0;
		        if( Temp_Data == ']' ){	// ��������У��
					// ������ַУ��
					if( UART0_ReceBuf[1] == con_HexCode[Device_Address] ){
					    UART0_ReceOKFlag = 1; // ������,�ݲ���CRCУ��
					}			       
			    }
		    }
		}
	}

	// �����ж�
	if( TI ){ TI = 0; 
		if( UART0_TranCounter < UART0_TranMaxNum ){
			SBUF = UART0_TranBuf[ UART0_TranCounter++ ]; 			
		}
		else{
			Max485_Rece;             // 485����ʹ��
		}
	}

}

*/


