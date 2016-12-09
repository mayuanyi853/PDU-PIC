
/*=========================================================
                    功率计       ----有限公司
版    本：V1.0
开发时间：2010年7月26日
版权所有：2010--2099,河北科技大学  信息科学与工程系  孟志永
  					                      手机：13933120973
									   Email：KDMZY@163.com
  文 件 名：C_Main.c
  作    者: 孟志永
  版    本: V1.0
  完成日期: 2010年7月29日
  描述信息: 功率计
  其他说明: 芯片类型：STC12C5604AD  主频：11.0592MHz       
  功能列表: 
  			1、系统初始化
			2、CS5460芯片初始化及配置
			3、UART0串行口收发处理
			4、6通道的电流真有效值、电压真有效值
			5、485收发控制
  历史纪录:   
            1、日    期:
               作    者:
               修改内容:
=========================================================*/

/*=============头文件声明================================*/
#include "STC12C5620AD_H.h"

#include "H_STC12C5404ADMdu.H"
#include "H_STCE2PROM.h"
#include "H_Interface.h"
#include "H_CS5460.h"
#include "intrins.h"

/*=======================================================================
				系统公共变量声明
=======================================================================*/
		  bit Flag_10ms   = 0;		// 10ms标志
		 // bit Flag_100ms  = 0;  	// 100ms标志
		 // bit Flag_1000ms = 0;  	// 1000ms标志
		 // bit Flag_5s     = 0;      // 5秒标志

//unsigned char Counter_10ms   = 0;   // 10ms计数器
//unsigned char Counter_100ms  = 0;  	// 100ms计数器
//unsigned char Counter_1000ms = 0; 	// 1000ms计数器


#define conSysPowerOnState  0x00	
#define conSysIDLEState     0x01  	
#define conSysCabState      0x02    

unsigned char System_State   = conSysPowerOnState;
unsigned  int State_Timer    = 0;
		  bit SystemLED_Flag;

/*=======================================================================
数据上传格式如下：
[@ZZ$D#YY-XX:0000.00V|0000.00A|0000.00W|0000.00Q|0000.00@|0000.00H|0000.00S|CRC]
1、以'['开始，以']'结束
2、@从机地址
3、#通道号
4、参数序号：00：电压
             01：电流
			 02：功率
			 03：功率因数
			 04：相位差角
			 05：频率
			 06：周期
			 07：预留
			 08：预留
			 09：预留
			 FF：表示以上全部参数
	每个参数采用固定格式，即0000.00单位，各个参数以‘|’隔开
	各个参数的具体范围如下：
		电    压：0～45.0V
		电    流：0～5A
		功 率 值：0～2250W

5、除[]符号以外的其他数据的和然后转换为ASCII码，范围从000~256

[@ZZ$R# R-XX:FFFFFF|CRC]:表示上传的ZZ单片机的XX地址寄存器的内容
[@ZZ$S#  ACK       |CRC]：
[@ZZ$S# nACK       |CRC]：
=======================================================================*/
/*=======================================================================
数据下传格式如下：
0123456789ABCDEF
[@ZZ$D#YY-XX       CRC]：表示读取ZZ地址单片机的YY通道的第XX项参数
				         当XX=FF时表示，一次获取其YY通道的所有参数
[@ZZ$R#R-XX        CRC]：表示读取F地址单片机的XX地址寄存器的内容

[@ZZ$W#R-XX|FFFFFF CRC]：表示写ZZ地址单片机的XX寄存器的内容

[@ZZ$C# 0-VIDC      CRC]：表示写ZZ地址单片机的电压电流进行DC校准
[@ZZ$C# 1-VIAC      CRC]：表示写ZZ地址单片机的电压电流进行AC校准
[@ZZ$C# 2-VIGain    CRC]：表示写ZZ地址单片机的电压电流进行增益校准
[@ZZ$C# 3-VIOffset  CRC]：表示写ZZ地址单片机的电压电流进行偏置校准

[@ZZ$S#T-Cnn       CRC]：表示写ZZ地址单片机的数据自动上传功能的设置
                         nn表示时间间隔单位秒
				         nn=00时表示禁止自动连续上传功能
[@ZZ$S#S-4800,8,1,0CRC]：串行口波特率设置
[@ZZ$S#A-XX        CRC]：设置ZZ单片机的通讯地址为XX
=======================================================================*/
unsigned char idata Device_Address;    // 本机地址

#define con_UART0TranMaxNum      38                          //  30
unsigned char idata UART0_TranBuf[con_UART0TranMaxNum]; //  = { "[ 0000.00V|0000.00A|0000.00W|0000.00Q|0000.00@|0000.00H|0000.00S|]" }; // 串行口0的发送缓冲区
unsigned char idata UART0_TranMaxNum;
unsigned char idata UART0_TranCounter; 		// 串行口0的发送计数器

#define con_UART0ReceMaxNum	   7
unsigned char  data UART0_ReceBuf[con_UART0ReceMaxNum]; 		// 串行口0接收缓冲区
unsigned char idata UART0_ReceCounter; 		// 串行口0的接收计数器
		  bit       UART0_ReceOKFlag = 0;	// 串行口0的接收好标志
unsigned char idata UART0_TranState;        // 串行口上传状态字
unsigned char idata UART0_TranChannel;      // 要上传的通道号

unsigned char code con_HexCode[]  = { "0123456789ABCDEF" }; 
unsigned char idata CS5460_Channel = 0;

/*
unsigned char xdata Measure_Buf[8][3][8] = {    // 数据结果缓冲区
											// 0通道
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0',
											// 1通道 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											// 2通道
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											// 3通道
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0',
											// 4通道 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											// 5通道
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											// 6通道
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											// 7通道
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0', 
											' ', '0', '0', '0', '0', '.', '0', '0'
										};

		*/


unsigned  char	 xdata  Measure_Buf[8][4]  = {
											   0x00 ,0x00 ,0x00 ,0x00 ,	   //1路电压值, 电流值	//功率
                                               0x00 ,0x00 ,0x00 ,0x00 ,
										       0x00 ,0x00 ,0x00 ,0x00 ,
										       0x00 ,0x00 ,0x00 ,0x00 ,
										       0x00 ,0x00 ,0x00 ,0x00 ,
									           0x00 ,0x00 ,0x00 ,0x00 ,
											   0x00 ,0x00 ,0x00 ,0x00 ,
										       0x00 ,0x00 ,0x00 ,0x00 	  //8 路
                                              };


unsigned  int xdata    measure_ma[8][2]={
										    	0x0000 ,0x0000 ,	   //1路电压码 ， 电流码	
                                                0x0000 ,0x0000 ,
										        0x0000 ,0x0000 ,
										        0x0000 ,0x0000 ,
										        0x0000 ,0x0000 ,
									            0x0000 ,0x0000 ,
												0x0000 , 0x0000,
												0x0000 , 0x0000
											};


//unsigned  char xdata    measure_sys[6][4]={
//										    	0x00 ,0x00 ,0x00 ,0x00 ,	   //1路电压 系数电   流 ，	系数
 //                                               0x00 ,0x00 ,0x00 ,0x00 ,								
//										        0x00 ,0x00 ,0x00 ,0x00 ,
//										        0x00 ,0x00 ,0x00 ,0x00 ,
//										        0x00 ,0x00 ,0x00 ,0x00 ,
//									            0x00 ,0x00 ,0x00 ,0x00 
//											};

//unsigned  int   idata	  v_sys[6]={ 0xd9c7 ,0xd9c7 ,0xd9c7,0xd9c7,0xd9c7,0xd9c7  };	//六路电压系数	  r=215k系数=d9c7  (26000*65536/30563)
  unsigned  int   idata	  v_sys= 0xd9c7 ;


//	float code conVoltage_Cof = (25000.0*0.98* 5.0 / 16777216.0);
	//	float code conCurrent_Cof = (25000.0 * 5.0 / 16777216.0);
	//	float code   conPower_Cof = (250.0*250.0 / 8388608.0);

//	    unsigned  int  code   conVoltage_sys=0xf522;   	 //	    25400*65536/26526		=0xf522			r=250k	
unsigned   int	 code    conCurrent_sys=0x4ed; 		 // 408*65536/52ce		  a=4.08		   ma=52ce
unsigned char xdata Parameter_Buf[ 6 * 3 + 1 ]; // 参数缓冲区

unsigned char idata Read_State = 0;

/*=======================================================================
			CS5460标志位处理
=======================================================================*/
unsigned char bdata CS5460_Flag0;
sbit DRDY_Flag = CS5460_Flag0^7;   // 数据有效标志 在连续或单次转换模式下表明计算结果已经就绪，在校准模式下表示校准数据已经写入到相应的寄存器
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

long idata CS5460_Status;  // CS5460的状态字

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
函数名称：strTostr
函数功能: 字符串传送函数
输入参数：
输出参数：无
返 回 值：无
其它说明：
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
函数名称：HexASCIIToBin
函数功能：
输入参数：
输出参数：无
返 回 值：无
其它说明：
=======================================================================*/
unsigned char HexASCIIToBin( unsigned char CVT_Data ){

	if( CVT_Data >= '0' && CVT_Data <= '9' ){ return ( CVT_Data - '0'); }
	else                                    { return ( CVT_Data - 'A' + 10 ); }

}

/*=======================================================================
函数名称：LongToASCII
函数功能：整形数转换为ASCII
输入参数：
输出参数：无
返 回 值：无
其它说明：
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
函数名称：UART0_UnPacket
函数功能：串行口解包函数
输入参数：无
输出参数：无
返 回 值：无
其它说明：
=======================================================================*/
/*=======================================================================
数据下传格式如下：
0123456789ABCDEF0123456
[@0F$D#01-00        CR]：表示读取ZZ地址单片机的YY通道的第XX项参数
				         当XX=FF时表示，一次获取其YY通道的所有参数
[@0F$R# R-0C        CR]：表示读取F地址单片机的XX地址寄存器的内容

[@ZZ$W# R-XX|FFFFFF CR]：表示写ZZ地址单片机的XX寄存器的内容

[@ZZ$S# T-Cnn       CR]：表示写ZZ地址单片机的数据自动上传功能的设置
                         nn表示时间间隔单位秒
				         nn=00时表示禁止自动连续上传功能
[@ZZ$S# S-4800,8,1,0CR]：串行口波特率设置
[@ZZ$S# A-XX        CR]：设置ZZ单片机的通讯地址为XX
[FDF]
[FCX]
01234
=======================================================================*/
/*
void UART0_UnPacket( void ){	

	switch( UART0_ReceBuf[2] ){
		case 'D': // 要数据的命令
				UART0_TranChannel   = HexASCIIToBin( UART0_ReceBuf[3] );
				if( UART0_TranChannel < 6 ){
				    UART0_TranState = 'D';
				}				 
				break;
		case 'C': 
				CS5460_CabNum = HexASCIIToBin( UART0_ReceBuf[3] );
				UART0_TranState = 'A'; // 回复正确应答命令
				System_State = conSysCabState; State_Timer = 0;  
				break;
		default: UART0_TranState = 'Z'; break;

	}

}

*/



void UART0_UnPacket( void ){	

	switch(	 UART0_ReceBuf[3]){
		case   0x03: // 要数据的命令
			//	UART0_TranChannel   = HexASCIIToBin( UART0_ReceBuf[3] );
			//	if( UART0_TranChannel < 6 ){
			//	    UART0_TranState = 'D';
			//	}	
			 	UART0_TranState = 'D';
				break;
		case 0x05: 	   //系统标定
			              //	CS5460_CabNum = HexASCIIToBin( UART0_ReceBuf[3] );
                CS5460_CabNum = UART0_ReceBuf[4]; // 找con_CabTab[]	   中对应的内容
				UART0_TranState = 'C'; // 回复正确应答命令
				System_State = conSysCabState; State_Timer = 0;  //
		
				break;	 //
		case 0x06: 
		          	vself_cab( );
					break;

		default: UART0_TranState = 'Z'; break;

	}

}





/*=======================================================================
函数名称：UART0_Packet
函数功能：处理串行口要发送的数据包 并自动启动发送
输入参数：
输出参数：无
返 回 值：无
其它说明：
=======================================================================*/
/*
void UART0_Packet( void ){

	UART0_TranBuf[0] = '[';					  // 放入起始字符
	UART0_TranBuf[1] = Device_Address;  // 放入地址
	// 上传数据打包过程
	switch( UART0_TranState ){		
		case 'D': // 上传采集到的数据命令
		        UART0_TranBuf[ 2] = con_HexCode[ UART0_TranChannel ];
				// 全部参数上传
				strTostr( &UART0_TranBuf[3],  &Measure_Buf[UART0_TranChannel][0], 8 );
				strTostr( &UART0_TranBuf[11], &Measure_Buf[UART0_TranChannel][1], 8 );
			//	strTostr( &UART0_TranBuf[19], &Measure_Buf[UART0_TranChannel][2], 8 );
				UART0_TranBuf[19] = ']';
				UART0_TranMaxNum = 20;
				break;	 
		case 'C':  // 上传完成状态 [@ZZ$C# XOK     |CRC]：
				UART0_TranBuf[ 2] = 'C'; 
				UART0_TranBuf[ 3] = con_HexCode[ CS5460_CabNum ];
				UART0_TranBuf[ 4] = 'O'; 
				UART0_TranBuf[ 5] = 'K'; 
				UART0_TranBuf[ 6] = ']'; 
				UART0_TranMaxNum = 7;
				break;
		case 'A':  // 上传正确应答状态 [@ZZ$S# ACK|CRC]：
				UART0_TranBuf[ 2] = 'A'; 
				UART0_TranBuf[ 3] = 'C'; 
				UART0_TranBuf[ 4] = 'K'; 
				UART0_TranBuf[ 5] = ']'; 
				UART0_TranMaxNum = 6;
				break;
		case 'Z':  // 上传无法应答状态 
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
	Max485_Tran;             // 485发送使能
	TI = 1;	 // 把数据发送出去
	UART0_TranState = 0;

}

  */



void UART0_Packet( void ){
	 unsigned  char	  chkdata;
	UART0_TranBuf[0] = 0x6c;					  // 放入起始字符
	UART0_TranBuf[1] = 0x63;
	UART0_TranBuf[2] = Device_Address;  // 放入地址
	// 上传数据打包过程
	switch( UART0_TranState ){		
		case 'D': // 上传采集到的数据命令
		      //  UART0_TranBuf[ 2] = con_HexCode[ UART0_TranChannel ];
				// 全部参数上传
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
		case 'C':  // 上传完成状态 [@ZZ$C# XOK     |CRC]：
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
		case 'A':  // 上传正确应答状态 [@ZZ$S# ACK|CRC]：
				UART0_TranBuf[ 3] = 'A'; 
				UART0_TranBuf[ 4] = 'C'; 
				UART0_TranBuf[ 5] = 'K'; 
				UART0_TranBuf[ 6] = ']'; 
				UART0_TranMaxNum = 7;
				break;
		case 'Z':  // 上传无法应答状态 
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
	Max485_Tran;             // 485发送使能
	TI = 1;	 // 把数据发送出去
	UART0_TranState = 0;

}




/*=======================================================================
函数名称：Get_Parameter
函数功能：获取参数值, 同时配置CS5460
输入参数：无
输出参数：无
返 回 值：无
其它说明：
=======================================================================*/
void Get_Parameter( void ){

	unsigned char i;
	unsigned  int Parameter_Address;

	Parameter_Address = 0;

	for( i = 0; i < (6 * 3 + 1); i++ ){
		Parameter_Buf[i] = ISP_Byte_Read( Parameter_Address++ );
	}

	// 如果参数正确 则配置CS5460
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

	// 如果参数正确 则配置CS5460
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
函数名称：Save_Parameter
函数功能：保存参数值
输入参数：无
输出参数：无
返 回 值：无
其它说明：
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
函数名称：System_PowerOn
函数功能：系统刚上电需要处理的事情
输入参数：无
输出参数：无
返 回 值：无
其它说明：1、延时1S
		  2、指示灯以0.3S闪烁
		  3、使能电压通道，电流通道，切换到0通道
		  4、复位CS5460
		  5、跳转到系统空闲状态
=======================================================================*/
void System_PowerOn( void ){

	// 指示灯以0.3S闪烁
	if( (State_Timer % 30) == 0 ) { SystemLED_Flag = ~SystemLED_Flag; }

	// 使能电压通道，电流通道，切换到0通道 复位CS5460
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
	if( State_Timer == 103 ){ CS5460_RSTHigh; } // 复位完成

	// 同步CS5460的串行口
	if( State_Timer == 104 ){
		CS5460_CSLow;
		CS5460_Send( cmdSYNC1 );  // 
		CS5460_Send( cmdSYNC1 );  // 
		CS5460_Send( cmdSYNC1 );  // 
		CS5460_Send( cmdSYNC0 );  // 
		CS5460_CSHigh;
	
		CS5460_WriteReg( addConfig, 0x00, 0x10, 0x63 );	// 配置Config
	//	CS5460_WriteReg( addConfig, 0x00, 0x10, 0x03 );	// 配置Config
	//	Get_Parameter();   //读标定参数	 到cs5460中
	    read_vsys(); //从eeprom   中读人系数到v———sys【】数组中
	}					   

	if( State_Timer == 106 ){			
		CS5460_SendCmd( cmdStartCVTCtinu );  // 发送连续启动转换命令  
		// 获取设备地址
		Device_Address = P1;
		Device_Address >>= 4;
	//	Device_Address = Device_Address & 0x03;//p1.4 p1.5
	//	Device_Address =  con_HexCode[Device_Address];
		System_State = conSysIDLEState; State_Timer = 0;  // 1秒后 跳转到系统空闲状态
		Max485_Rece;
	}

}

 /*
void System_PowerOn( void ){

	// 指示灯以0.3S闪烁
	if( (State_Timer % 30) == 0 ) { SystemLED_Flag = ~SystemLED_Flag; }

	// 使能电压通道，电流通道，切换到0通道 复位CS5460
	if( State_Timer == 100 ){
		CS5460_RSTLow;
		CS5460_Channel = 0;
		CD4051_A = 1;
		CD4051_B = 0;
		CD4051_C = 0;
		CD4051_EN0 = 0;
		CD4051_EN1 = 0;

	}
	if( State_Timer == 103 ){ CS5460_RSTHigh; } // 复位完成

	// 同步CS5460的串行口
	if( State_Timer == 104 ){
		CS5460_CSLow;
		CS5460_Send( cmdSYNC1 );  // 
		CS5460_Send( cmdSYNC1 );  // 
		CS5460_Send( cmdSYNC1 );  // 
		CS5460_Send( cmdSYNC0 );  // 
		CS5460_CSHigh;
	
		CS5460_WriteReg( addConfig, 0x00, 0x10, 0x63 );	// 配置Config
	}

	if( State_Timer == 106 ){			
		CS5460_SendCmd( cmdStartCVTCtinu );  // 发送连续启动转换命令  
		// 获取设备地址
		Device_Address = P1;
		Device_Address >>= 4;
	//	Device_Address =  con_HexCode[Device_Address];
		System_State = conSysIDLEState; State_Timer = 0;  // 1秒后 跳转到系统空闲状态
		Max485_Rece;
	}

}


 */

/*=======================================================================
函数名称：System_IDLE
函数功能：系统空闲需要处理的事情
输入参数：无
输出参数：无
返 回 值：无
其它说明：1、指示灯以1S闪烁
		  2、每一秒切换一次通道，启动CS5460转换一次，读取一次转换结果，并把结果更新到相应的缓冲区
		  3、如果收到串行口数据包，则解包并处理
		  4、每一秒数据打包一次，并主动上传一次（调试用）
=======================================================================*/
/*
void System_IDLE( void ){	

	  bit Plus_Flag;
	float Temp_f;
	unsigned long Temp_l;

	// 每10ms读一次CS5460的状态字，根据状态字来判断下一步的操作
	CS5460_Status = CS5460_ReadReg( addStatus );
	CS5460_Flag2  = CS5460_Status;	 
	CS5460_Flag1  = ( CS5460_Status >> 8 );
	CS5460_Flag0  = ( CS5460_Status	>> 16 );
	if( DRDY_Flag == 1 ){ // 读取当前转换结果		  	
		Read_State++;
		if( Read_State != 3 ){
			// 清除标志
			CS5460_WriteReg( addStatus, CS5460_Flag0, CS5460_Flag1, CS5460_Flag2 );
			Temp_l = CS5460_ReadReg( addVRMS );
			Temp_l = CS5460_ReadReg( addIRMS );
			Temp_l = CS5460_ReadReg( addP );
		}
		if( Read_State >= 3 ){ Read_State = 0;
			// 获取电压真有效值			
			Temp_f = CS5460_ReadReg( addVRMS ); // 转换为浮点数
			Temp_f *= conVoltage_Cof;		// 转换为小数 乘上系数
			Temp_l  = Temp_f;			// 转换为长整形数
			// 拆分到相应通道的相应项缓冲区		
			LongToASCII( &Measure_Buf[CS5460_Channel][0][0], Temp_l, 'V' );
			// 获取电流真有效值
			Temp_f = CS5460_ReadReg( addIRMS );	// 转换为浮点数
			Temp_f *= conCurrent_Cof;		// 转换为小数 乘上系数
			Temp_l  = Temp_f;			// 转换为长整形数
			// 拆分到相应通道的相应项缓冲区
			LongToASCII( &Measure_Buf[CS5460_Channel][1][0], Temp_l, 'I' );
			// 获取功率值
		  	Temp_l = CS5460_ReadReg( addP );
			Plus_Flag = 0;
			if( Temp_l & 0x00800000 ){ 
				Plus_Flag = 1; 
				Temp_l &= 0x007FFFFF; 
			}
			Temp_f = Temp_l;	// 转换为浮点数
			Temp_f *= conPower_Cof;		// 转换为小数 乘上系数
			Temp_l  = Temp_f;			// 转换为长整形数
			// 拆分到相应通道的相应项缓冲区
			LongToASCII( &Measure_Buf[CS5460_Channel][2][0], Temp_l, 'P' );
	 		if( Plus_Flag ) { Measure_Buf[CS5460_Channel][2][0] = '-'; }
			// 清除标志
			CS5460_WriteReg( addStatus, CS5460_Flag0, CS5460_Flag1, CS5460_Flag2 );
			// 切换通道
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

	// 1s到
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

	// 每10ms读一次CS5460的状态字，根据状态字来判断下一步的操作
	CS5460_Status = CS5460_ReadReg( addStatus );
	CS5460_Flag2  = CS5460_Status;	 
	CS5460_Flag1  = ( CS5460_Status >> 8 );
	CS5460_Flag0  = ( CS5460_Status	>> 16 );
	if( DRDY_Flag == 1 ){ // 读取当前转换结果		  	
		Read_State++;
		if( Read_State != 3 ){
			// 清除标志
			CS5460_WriteReg( addStatus, CS5460_Flag0, CS5460_Flag1, CS5460_Flag2 );
			Temp_l = CS5460_ReadReg( addVRMS );
			Temp_l = CS5460_ReadReg( addIRMS );
			Temp_l = CS5460_ReadReg( addP );
		}
		if( Read_State >= 3 ){ Read_State = 0;
			// 获取电压真有效值			
		   //	Temp_f = CS5460_ReadReg( addVRMS ); // 转换为浮点数
		   //	Temp_f *= conVoltage_Cof;		// 转换为小数 乘上系数
		  //	Temp_l  = Temp_f;		   	// 转换为长整形数

				Temp_l=CS5460_ReadReg(addVRMS);
				Temp_l=Temp_l>>8;			      // yong  2  zhi jie
			
				measure_ma[CS5460_Channel][0]=Temp_l;   // cun ri ma 存人电压码到 measure_ma[][] 中
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



			// 拆分到相应通道的相应项缓冲区		
		//	LongToASCII( &Measure_Buf[CS5460_Channel][0][0], Temp_l, 'V' );
		     Measure_Buf[CS5460_Channel][1]=Temp_l;
             Measure_Buf[CS5460_Channel][0]=Temp_l>>8;	 //存入电压值到measure_buf[][]中 
             
			// 获取电流真有效值
		//	Temp_f = CS5460_ReadReg( addIRMS );	// 转换为浮点数
		//	Temp_f *= conCurrent_Cof;		// 转换为小数 乘上系数
		//	Temp_l  = Temp_f;			// 转换为长整形数
			// 拆分到相应通道的相应项缓冲区
		//	LongToASCII( &Measure_Buf[CS5460_Channel][1][0], Temp_l, 'I' );
			// 获取功率值
		   Temp_l=CS5460_ReadReg(addIRMS);
           Temp_l=Temp_l>>8;
		   Temp_l=Temp_l*conCurrent_sys;
		   Temp_l=Temp_l>>16;
           Measure_Buf[CS5460_Channel][3]=Temp_l;
           Measure_Buf[CS5460_Channel][2]=Temp_l>>8;  //电流值 存入到measure_buf[][]中 



		 // 	Temp_l = CS5460_ReadReg( addP );
		//	Plus_Flag = 0;
		//	if( Temp_l & 0x00800000 ){ 
		//		Plus_Flag = 1; 
		//		Temp_l &= 0x007FFFFF; 
		//	}
		//	Temp_f = Temp_l;	// 转换为浮点数
		//	Temp_f *= conPower_Cof;		// 转换为小数 乘上系数
		//	Temp_l  = Temp_f;			// 转换为长整形数
			// 拆分到相应通道的相应项缓冲区
		//	LongToASCII( &Measure_Buf[CS5460_Channel][2][0], Temp_l, 'P' );
	 	//	if( Plus_Flag ) { Measure_Buf[CS5460_Channel][2][0] = '-'; }
			// 清除标志
			CS5460_WriteReg( addStatus, CS5460_Flag0, CS5460_Flag1, CS5460_Flag2 );
			// 切换通道
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

	// 1s到
	if( ( State_Timer % 100 ) == 0 ){ State_Timer = 0;
		SystemLED_Flag = ~SystemLED_Flag;
	}

}  





/*=======================================================================
函数名称：Wait_DRDY
函数功能: 
输入参数：无
输出参数：无
返 回 值：无
其它说明：1、校准期间指示灯常亮
		  2、校准完成后指示灯闪3次,并上传校准完成消息
		  3、校准后的系数存入E2PROM中
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
函数名称：System_Cab
函数功能：系统校准函数
输入参数：无
输出参数：无
返 回 值：无
其它说明：1、校准期间指示灯常亮
		  2、校准完成后指示灯闪3次,并上传校准完成消息
		  3、校准后的系数存入E2PROM中
=======================================================================*/
/*
void System_Cab( void ){

	SystemLED_Flag = con_LEDOn;

	// 清标记
	CS5460_WriteReg( addStatus, 0xFF, 0xFF, 0xFF );
	// 1、发送Power_Up命令
	CS5460_SendCmd( cmdPowerUpHalt );
	// 清标记
	CS5460_WriteReg( addStatus, 0xFF, 0xFF, 0xFF );	

	CS5460_SendCmd( con_CabTab[ CS5460_CabNum ]  );
	DRDY_Flag = 0;
	while( DRDY_Flag == 0 ){
	   Wait_DRDY();
	}

	CS5460_WriteReg( addStatus, CS5460_Flag0, CS5460_Flag1, CS5460_Flag2 );

	Save_Parameter( );
	CS5460_SendCmd( cmdStartCVTCtinu );  // 发送连续启动转换命令

	UART0_TranState = 'C';
	// 熄灭LED
	SystemLED_Flag = con_LEDOff;
	// 切换到系统空闲状态
	System_State = conSysIDLEState; State_Timer = 0;

}

 */

/*=======================================================================
函数名称：System_Cab
函数功能：系统校准函数
输入参数：无
输出参数：无
返 回 值：无
其它说明：1、校准期间指示灯常亮
		  2、校准完成后指示灯闪3次,并上传校准完成消息
		  3、校准后的系数存入E2PROM中
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

	// 清标记
	CS5460_WriteReg( addStatus, 0xFF, 0xFF, 0xFF );

	_nop_();
	_nop_();
	_nop_();
	_nop_();
		delaynus (200) ;
	// 1、发送Power_Up命令
	CS5460_SendCmd( cmdPowerUpHalt );

	_nop_();
	_nop_();
	_nop_();
	_nop_();
	// 清标记
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
	CS5460_SendCmd( cmdStartCVTCtinu );  // 发送连续启动转换命令

	UART0_TranState = 'C';
	// 熄灭LED
	SystemLED_Flag = con_LEDOff;
	// 切换到系统空闲状态
	System_State = conSysIDLEState; State_Timer = 0;
	

}



 /*======================================================
    自标定
   把采出的数 用220v    的标准      x=22000 *65536 /	   码
   计算后的数   右移两个字节		然后存人200 开始的第二扇区
   212  存入01 
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
		      v_sys[i]=temp_sys;	  //存人电压系数
			  templ=temp_sys;
			  temph=temp_sys>>8;
              ISP_Byte_Write(ee_address++ ,temph);//
		  	  ISP_Byte_Write(ee_address++ ,templ);	//
	       }
*/	
	 
		   	  temp_sys= 0x55f00000 / measure_ma[0][0] ;	//22000*65536
		      v_sys=temp_sys;	  //存人电压系数
			  templ=temp_sys;
			  temph=temp_sys>>8;
              ISP_Byte_Write(ee_address++ ,temph);  //v_sys 的 高8位存入 200h中
		  	  ISP_Byte_Write(ee_address++ ,templ);	// v_sys 的低8位存入 201h中
	       
	  	  ISP_Byte_Write(0x212 ,0x01);	 //eeprom   把01h 存入到 0x212中 

	  
	}


 /*================================
  先读212地址的数据是否01
  是 ，读入数据放入对应的系数中



 ==================================*/

 void read_vsys(void)
   { unsigned  char i,	temp_data ,temp_datah ,temp_datal;
     unsigned int  temp_w;
     unsigned int ee_address;
	 ee_address=0x200;
     temp_data= ISP_Byte_Read( 0x212 );//判212是否是01h
	 if(temp_data==0x01)
	   /* {for(i=0;i<=5 ;i++)
		   { temp_datah = ISP_Byte_Read(ee_address++);
		     temp_datal=  ISP_Byte_Read(ee_address++);
			 temp_w=temp_datah;
			 temp_w<<=8;
			 temp_w+=temp_datal;
			 v_sys[i]=temp_w;	//存入系数到内存数组中
		   }
		*/
	   {     temp_datah = ISP_Byte_Read(ee_address++);
		     temp_datal=  ISP_Byte_Read(ee_address++); //从flash读人电压系数到v_sys中
			 temp_w=temp_datah;
			 temp_w<<=8;
			 temp_w+=temp_datal;
			 v_sys=temp_w;	//存入系数到内存数组中
       	}
     
   }


/*====================================================================       ===
函数名称：Output_Port
函数功能：端口刷新函数
输入参数：无
输出参数：无
返 回 值：无
其它说明：无
=======================================================================*/
void Output_Port( void ){

	System_LED = SystemLED_Flag;

}

/*=======================================================================
函数名称：main
函数功能：
输入参数：无
输出参数：无
返 回 值：无
其它说明：无
=======================================================================*/
void main( void ){
	bit bdata biao_flag=0;
	Max485_Rece;             // 485接收使能
	System_LED = con_LEDOn;	 // 点亮系统指示灯
	Init_System( );

	while(1){ 
	    	WDT_CONTR = 0x38;
			if((STL==0 ) && (biao_flag==0) )
			  {	vself_cab( );
			    biao_flag=1;
			  }	 
		// 每10ms要做的事情
		if( Flag_10ms ){ Flag_10ms = 0;		
		           WDT_CONTR = 0x38;
					// 总要做的事情
			if( UART0_ReceOKFlag ){ // 串行口接收好
				UART0_UnPacket( );	// 调用串行口解包命令
				UART0_ReceOKFlag = 0; // 清接收好标记
			}
			if( UART0_TranState ){ UART0_Packet(); }
			// 系统状态机处理
			if( State_Timer < 65530 ) { State_Timer++; }			
			switch( System_State ){
				case conSysPowerOnState: System_PowerOn(); break;
				case conSysIDLEState:    System_IDLE();    break;
				case conSysCabState:     System_Cab();     break;
				default: System_State = conSysPowerOnState; State_Timer = 0;  break;
			} // 系统状态机处理结束

			// 10ms刷新一次输出端口
			Output_Port( );

		} // 10ms事件处理结束
	}

}



/*=======================================================================
函数名称：ISR_INT0  外部中断0的中断服务
函数功能：红外线输入检测          
输入参数：无
输出参数：无
返 回 值：无
其它说明：无
=======================================================================*/
void ISR_INT0( void ) interrupt 0  using 1{
	
		 
}

/*=======================================================================
函数名称：ISR_Timer0  定时器T0中断服务
函数功能：实现10ms定时中断          
输入参数：无
输出参数：无
返 回 值：无
其它说明：无
=======================================================================*/
void ISR_Timer0( void ) interrupt 1  //using 1
{
			  
	TH0   = 0xD5;
	TL0   = 0x9D;

	Flag_10ms = 1;

} 

/*=======================================================================
函数名称：ISR_INT1  外部中断1的中断服务
函数功能：键盘输入检测          
输入参数：无
输出参数：无
返 回 值：无
其它说明：无
=======================================================================*/
void ISR_INT1( void ) interrupt 2  //using 1
{
	 
}

/*=======================================================================
函数名称：ISR_Timer1  定时器T1中断服务
函数功能：实现1ms定时中断          
输入参数：无
输出参数：无
返 回 值：无
其它说明：无
=======================================================================*/
void ISR_Timer1( void ) interrupt 3 // using 1
{

}

/*=======================================================================
函数名称：ISR_Serial0 串行口0中断服务
函数功能：系统调试      
输入参数：无
输出参数：无
返 回 值：无
其它说明：无
=======================================================================*/




void ISR_UART0( void ) interrupt 4 // using 1
{
    unsigned char Temp_Data;

	// 接收中断
    if( RI ){  RI = 0;
		if( UART0_ReceOKFlag == 0 ){
		    Temp_Data = SBUF;
		    if( Temp_Data == 0x4c ){	UART0_ReceCounter = 0; }
		    UART0_ReceBuf[ UART0_ReceCounter++ ] = Temp_Data;
		    if( UART0_ReceCounter == con_UART0ReceMaxNum ){
		        UART0_ReceCounter = 0;
		        if( (UART0_ReceBuf[1] == 0x57 )&& ((UART0_ReceBuf[3]==0x03) ||(UART0_ReceBuf[3]==0x05) ||(UART0_ReceBuf[3]==0x06))	   &&(UART0_ReceBuf[6]==0x0d)){	// 包完整性校验
					// 本机地址校验
					if( UART0_ReceBuf[2] == Device_Address ){
					    UART0_ReceOKFlag = 1; // 调试用,暂不做CRC校验
					}			       
			    }
		    }
		}
	}

	// 发送中断
	if( TI ){ TI = 0; 
		if( UART0_TranCounter < UART0_TranMaxNum ){
		    delaynus(10);
			SBUF = UART0_TranBuf[ UART0_TranCounter++ ]; 			
		}
		else{
			Max485_Rece;             // 485接收使能
		}
	}

}





/*
void ISR_UART0( void ) interrupt 4 // using 1
{
    unsigned char Temp_Data;

	// 接收中断
    if( RI ){  RI = 0;
		if( UART0_ReceOKFlag == 0 ){
		    Temp_Data = SBUF;
		    if( Temp_Data == '[' ){	UART0_ReceCounter = 0; }
		    UART0_ReceBuf[ UART0_ReceCounter++ ] = Temp_Data;
		    if( UART0_ReceCounter == con_UART0ReceMaxNum ){
		        UART0_ReceCounter = 0;
		        if( Temp_Data == ']' ){	// 包完整性校验
					// 本机地址校验
					if( UART0_ReceBuf[1] == con_HexCode[Device_Address] ){
					    UART0_ReceOKFlag = 1; // 调试用,暂不做CRC校验
					}			       
			    }
		    }
		}
	}

	// 发送中断
	if( TI ){ TI = 0; 
		if( UART0_TranCounter < UART0_TranMaxNum ){
			SBUF = UART0_TranBuf[ UART0_TranCounter++ ]; 			
		}
		else{
			Max485_Rece;             // 485接收使能
		}
	}

}

*/


