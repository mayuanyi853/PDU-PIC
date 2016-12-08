/*****************************************************
*作者：   CCT
*时间：   2010/11/09-
*功能：   在这个程序中，主要实现调用485访问两个12C5608，
		  同时接收5608发来的数据等等。
		  



******************************************************/
#include "TCPIP Stack/TCPIP.h"
void RS485_SetMAC(void);
void RS485_SetDS1302(void);
#if defined(STACK_USE_RS485)
BOOL b_RS485_Success;
struct _RS485Frame
{
	BYTE DATNum ;		//串口准备接收的数据个数
	BYTE RCBUF[29];		//串口接收缓冲区
	BYTE TXBUF[7];		//串口发送缓冲区
}RS485;

extern BYTE AVs[12][4];
extern BYTE rs485_check1,rs485_check2;

///玩具区//////////////////////////////////////////////////////////////////////////////总电流，总电压
extern BYTE testV[6]; 			//格式{'2','3','0','.','4',0x00} 电压
extern BYTE shareA[6];			//格式{' ','0','.','0','0',0x00} 电流

extern char DataArray1[3];    	//??


BYTE rs486send[]={0x4c,0x57,0x0f,0x03,0x00,0x00,0x0d};

///////////////////////////////////////////////////////////////////////////cct把所有485缓存中的数，按要求复制到AVs(AVs为电流电压数组)

void RS485_Function(void)        
{
	BYTE i;
	int a,b,c=3;
	if(b_RS485_Success == 0)                            //CCT以前这句话好像有用，现在用不上了，不过留着吧，不删了。
		return;
	b_RS485_Success = 0;
	if(RS485.RCBUF[0]==0x6c)
	{
#if defined(POWER_SUPPLY_12)||defined(POWER_SUPPLY_12_1602)
				for(a=0;a<6;a++)                       //CCT  AVs12个段赋值
				{
					for(b=0;b<4;b++,c++)
					{
						if(RS485.RCBUF[2] == 0x0e)     //CCT因为是两个485，对发来的数确认
						{
							AVs[a][b] = RS485.RCBUF[c];
							rs485_check1=1;            //CCT 用于检测是否485通信中断
						}
						else if(RS485.RCBUF[2] == 0x0f)
						{	AVs[a+6][b] = RS485.RCBUF[c];
							rs485_check2=1;
						}	
					}
				}
#elif defined(POWER_SUPPLY_6)
				for(a=0;a<6;a++)                       //CCT  AVs6个段赋值
				{
					for(b=0;b<4;b++,c++)
					{
							AVs[a][b] = RS485.RCBUF[c];	
					}
				}		   
	        	rs485_check1=1;           			   //CCT 用于检测是否485通信中断
#endif
	}  else if(RS485.RCBUF[0]==0x6d)
	{
        RS485_SetMAC();
		RS485_SetDS1302();	
	}
}

/////////////////////////////////////////////////////////////////////////////////CCT通过串口设置MAC地址
void RS485_SetMAC(void)
{

    BYTE host_name[16]={'W','Y','J','S','0','0','0','0','0','0','C','C','T','\0','\0','\0'};
    BYTE x,y;
	AppConfig.MyMACAddr.v[0]=RS485.RCBUF[1];
	AppConfig.MyMACAddr.v[1]=RS485.RCBUF[2];
	AppConfig.MyMACAddr.v[2]=RS485.RCBUF[3];
	AppConfig.MyMACAddr.v[3]=RS485.RCBUF[4];
	AppConfig.MyMACAddr.v[4]=RS485.RCBUF[5];
	AppConfig.MyMACAddr.v[5]=RS485.RCBUF[6];
    x = RS485.RCBUF[3];
    y = x/0x10;
    x = x%0x10;
	if(y>=10)
    	host_name[4] = y-10+'A';
	else
		host_name[4] = y+'0';
	if(x>=10)
    	host_name[5] = x-10+'A';
	else
		host_name[5] = x+'0';
    
    x = RS485.RCBUF[1];
    y = x/0x10;
    x = x%0x10;
	if(y>=10)
    	host_name[6] = y-10+'A';
	else
		host_name[6] = y+'0';
	if(x>=10)
    	host_name[7] = x-10+'A';
	else
		host_name[7] = x+'0';
 
    x = RS485.RCBUF[2];
    y = x/0x10;
    x = x%0x10;
	if(y>=10)
    	host_name[8] = y-10+'A';
	else
		host_name[8] = y+'0';
	if(x>=10)
    	host_name[9] = x-10+'A';
	else
		host_name[9] = x+'0';
    for(x=0;x<16;x++)
		AppConfig.NetBIOSName[x]=host_name[x];
 //	memcpypgm2ram(AppConfig.NetBIOSName,host_name, 16);
	FormatNetBIOSName(AppConfig.NetBIOSName);
 	SaveAppConfig();
}
/////////////////////////////////////////////////////////////////////////////////CCT通过串口设置时钟
void RS485_SetDS1302(void)
{
	BYTE i;
	RTC_RST_IO = 0;
	RTC_SCL_IO = 0;
	RTC_RST_TRIS = 0;
	RTC_SCL_TRIS = 0;
	RTC_SDA_TRIS = 1;

		WriteDS1302( Add_CONTROL,0x00 );		//关闭写保护
		WriteDS1302( Add_MIN,RS485.RCBUF[10] );	//预置分钟时间
		WriteDS1302( Add_HR,RS485.RCBUF[11] );	//预置小时，24小时制
		WriteDS1302( Add_DATE,RS485.RCBUF[12] );//预置日期
		WriteDS1302( Add_MONTH,RS485.RCBUF[13] );//预置月份
		WriteDS1302( Add_DAY,0x05 );			//预置星期
		WriteDS1302( Add_YEAR,RS485.RCBUF[14] );//预置年份
		WriteDS1302( Add_CHARGER,0x0a6 );		//写充电控制寄存器，1010 0110，I=1mA
		WriteDS1302( Add_SEC,0x08 );			//启动时钟
		WriteDS1302( Add_RAM0,0x50);			//RAM0单元写入0x55，防止再次初始化
		WriteDS1302( Add_CONTROL,0x80 );		//打开写保护 
	
	DelayMs(10);
}

/////////////////////////////////////////////////////////////////////////////////CCT这个地方是中断函数。。。从485发来的
void RS485_RC_ISR(void)
{
	BYTE temp;
	WORD i_delay;
	if(PIR3bits.RC2IF)   //接收缓冲区满标志位
	{
		PIR3bits.RC2IF = 0;    //标志位自动清零
		RS485.RCBUF[0] = RCREG2;   //串口准备接收的数据个数
		for(temp = 1; temp < 29; temp++)
		{
			i_delay = 10000l;
			while(!PIR3bits.RC2IF && i_delay)
			{
				i_delay--;
				if( !i_delay )
				goto RC_Failure;
			}
			PIR3bits.RC2IF = 0;
			RS485.RCBUF[temp] = RCREG2;
		}
		
		b_RS485_Success = 1;
		return;

	RC_Failure: 
		b_RS485_Success = 0;
	//	SPK_IO=1;
	}
}
	
///////////////////////////////////////////////////////////////////////////////////////cct 发数据到485
void RS485_SendRequest(void)
{ 
			int rsint  = 0;
			RS485_DIR_IO = Send;
			DelayMs(1);

#if defined(POWER_SUPPLY_12)||defined(POWER_SUPPLY_12_1602)
	
			if(rs486send[2] == 0x0f)		//CCT每3s自动巡检两个芯片电流电压
				rs486send[2] = 0x0e;
			else if(rs486send[2] == 0x0e)
				rs486send[2] = 0x0f;

#elif defined(POWER_SUPPLY_6)

#endif
			for(rsint=0;rsint<7;rsint++){
				RS485_Putch(rs486send[rsint]);
				Nop();
			}
			RS485_DIR_IO = Receive;		
}
////////////////////////////////////////////////////////////////////cct 总电流
//格式{' ','0','.','0','0',0x00} 电流
void ApSum(void)
{
	WORD sum=0,sumX=0;
    BYTE ctemp,x;
	DataArray1[0]=0;
	for(x = 0;x <= 11;x++)
	{
		sumX = AVs[x][2];
		sumX<<=8;
		sumX += AVs[x][3];
		if(sumX>0)
			DataArray1[0]+=1;	
		sum += sumX;
	}

	ctemp=sum/10000;
	sum = sum %10000;

	ctemp=sum/1000;
	sum = sum %1000;
	ctemp+='0';
	if(ctemp == '0')
		shareA[0] = ' ';
	else
		shareA[0] = ctemp;

	ctemp=sum/100;
	sum = sum %100;
	ctemp+='0';
	shareA[1] = ctemp;	

	shareA[2] = '.';

	ctemp=sum/10;
	sum = sum %10;
	ctemp+='0';		
	shareA[3] = ctemp;
	
	shareA[4] = sum+'0';
	shareA[5] = '\0';
//////mayee ：四舍五入小数点后一位////格式{' ','0','.','0','0',0x00} 电流
	if(shareA[4] > '5')
	{
		if(shareA[3] != '9')
			shareA[3] = shareA[3]+1;
		if(shareA[3] == '9')
		{
			shareA[3] = '0';
			if(shareA[1] != '9')
				shareA[1] = shareA[1]+1;
			if(shareA[1] == '9')
			{
				shareA[1] = '0';
				shareA[0] = shareA[0]+1;
			}	
		}
			
	}
	shareA[4] = '\0';
}

/////////////////////////////////////////////////////////////////////////////////////////cct 总功率
QWORD PowerSum(void)
{	
	QWORD pSum = 0;
	BYTE x;
	for(x = 0;x < 12;x++)
	{
		pSum += (DWORD)((AVs[x][0]<<8ul|AVs[x][1])*(AVs[x][2]<<8ul|AVs[x][3]));
	}
	return pSum;
}
//////////////////////////////////////////////////////////////////////
///mayee 由于改版以后每个采集板只有一个电压互感，只采集总电压
//函数功能：将采集的电压数据存储到 BYTE testV[6]; 数组			//格式{'2','3','0','.','4',0x00} 电压
//注:老版屏幕显示数据都写到LCDText[]数组中，由于CCT的改后的程序，他为了不影响数据结构，老屏的程序一直在保留，
//并运行。次函数的目的就是为了脱离老屏程序，最终把老屏删掉
//2013-5-24
////////////////////////////////////////////////////////////////
void PowerVtoDis(void)
{
	WORD sum = 0x0000;
	BYTE ctemp;
	if(AVs[0][0] == 0x00&&AVs[6][0] == 0x00)
		return;
	if(AVs[0][0] != 0x00)
	{
		sum = ((WORD)AVs[0][0]) << 8;
		sum = sum|(WORD)AVs[0][1];
		ctemp = (BYTE)(sum/10000); //mayee 电压百位
		ctemp = ctemp + '0'; 
		if(ctemp == '0')
			testV[0] = ' ';
		else	
			testV[0] = ctemp; 
	
		sum = sum % 10000;			//mayee 电压十位
		ctemp = (BYTE)(sum/1000); 
		ctemp = ctemp + '0'; 
			if(ctemp == '0')
			testV[1] = ' ';
		else	
			testV[1] = ctemp;
	
		sum = sum % 1000;			//mayee 电压个位
		ctemp = (BYTE)(sum/100); 
		ctemp = ctemp + '0'; 	
		testV[2] = ctemp;
	
		testV[3] = '.';				//小数点
	
		sum = sum % 100;			//mayee 电压小数点后一位位
		ctemp = (BYTE)(sum/10); 
		ctemp = ctemp + '0'; 
		testV[4] = ctemp;
		testV[5] = '\0';	

		sum = sum/100;
	}
#if defined(POWER_SUPPLY_12)//(POWER_IN3322)
	if(AVs[6][0] != 0x00)
	{
		sum = ((WORD)AVs[6][0]) << 8;
		sum = sum|(WORD)AVs[6][1];
		ctemp = (BYTE)(sum/10000); //mayee 电压百位
		ctemp = ctemp + '0'; 
		if(ctemp == '0')
			testV[0] = ' ';
		else	
			testV[0] = ctemp; 
	
		sum = sum % 10000;			//mayee 电压十位
		ctemp = (BYTE)(sum/1000); 
		ctemp = ctemp + '0'; 
			if(ctemp == '0')
			testV[1] = ' ';
		else	
			testV[1] = ctemp;
	
		sum = sum % 1000;			//mayee 电压个位
		ctemp = (BYTE)(sum/100); 
		ctemp = ctemp + '0'; 	
		testV[2] = ctemp;
	
		testV[3] = '.';				//小数点
	
		sum = sum % 100;			//mayee 电压小数点后一位位
		ctemp = (BYTE)(sum/10); 
		ctemp = ctemp + '0'; 
		testV[4] = ctemp;
		testV[5] = '\0';	

		sum = sum/100;
	}
#endif
//	return ;
}
////////////////////////////////////////////////////////////////////////////////////////////cct LCD显示总功率
void PowerToLCD(void)
{
	QWORD AVSum,z=10000;
	BYTE x,y;
	AVSum = PowerSum();
	strcpypgm2ram((char *)&LCDText[32],"输出功率：");///////
	for(y = 0; y < 5 ;y++)
	{
		x = AVSum/z;
		AVSum %=z;
//		if(x==0x00)
//			LCDText[42+y] = ' ';
//		else
			LCDText[42+y] = '0'+x;	
		z /=10;	 
	}
	LCDText[47] = 'W';
}
#endif







