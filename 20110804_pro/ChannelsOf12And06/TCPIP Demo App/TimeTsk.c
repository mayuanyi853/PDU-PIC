/*****************************************************
*作者：   CCT
*时间：   2010/11/09-
*功能：   在这个程序中，主要实现了时间的调度，其中包括
		  显示屏晶屏，485调用子程序等等。待续，，，
		  嘿嘿



******************************************************/

#include "TCPIP Stack/TCPIP.h"
#include "MainDemo.h"

BOOL b_switch ;
void TmTsk(void)
{
	static int ca = 0;
	PowerVtoDis();			//mayee 新写的的新品电压显示数据处理函数，2012-5-29
#if defined(POWER_SUPPLY_12)
		switch(ca)
		{
			case 0:
				memset(LCDText,' ',64);
				DisplayTime();						//第一屏 显示时间
				DisplayIPValue(AppConfig.MyIPAddr);	       // 2行显示IP
				TPHIConvert();
				DisplayTPHI();		
				LCDUpdate(4);
				ca++;
				break;
			case 1:

				LCDshowAV();						//第二屏1-4路电流电压
				ca++;
				break;
			case 2:

				LCDshowAV();						//第三屏5-8路电流电压						
				ca++;
				break;
			case 3:
	
				LCDshowAV();						//第四屏9-12路电流电压
				ca++;
				break;
			case 4:
				RS485_CheckSTCs();
				ConnectToLCD();
		//		PowerToLCD();                  //CCT 这个地方 还没有弄好呢。 要等等了。
				RunTime();
				LCDUpdate(4);
				ca=0;	
				break;
		}
#elif defined(POWER_SUPPLY_6)
		switch(ca)
		{
			case 0:
				memset(LCDText,' ',32);
				DisplayTime();						//CCT第一屏 显示时间
				DisplayIPValue(AppConfig.MyIPAddr);	       // 2行显示IP		
				LCDUpdate(2);
				ca++;
				break;
            case 1:
				memset(LCDText,' ',32);           
				TPHIConvert();                      //CCT第二屏 显示温湿度
				DisplayTPHI();
				LCDUpdate(2);
				ca++;
				break;
			case 2:
				LCDshowAV();						//CCT第三屏1-2路电流电压
				ca++;
				break;
			case 3:
				LCDshowAV();						//CCT第四屏3-4路电流电压						
				ca++;
				break;
			case 4:
				LCDshowAV();					     //CCT第五屏5-6路电流电压	
				ca++;
				break;
			case 5:
				RS485_CheckSTCs();                   //CCT第四屏电力检测与网络状态
				ConnectToLCD();
				LCDUpdate(2);
				ca=0;	
				break;
		}
#elif defined(POWER_SUPPLY_12_1602)
		switch(ca)
		{
			case 0:
				memset(LCDText,' ',32);
				DisplayTime();						//CCT第一屏 显示时间
				DisplayIPValue(AppConfig.MyIPAddr);	       // 2行显示IP		
				LCDUpdate(2);
				ca++;
				break;
            case 1:
				memset(LCDText,' ',32);           
				TPHIConvert();                      //CCT第二屏 显示温湿度
				DisplayTPHI();
				LCDUpdate(2);
				ca++;
				break;
			case 2:
				LCDshowAV();						//CCT第三屏1-2路电流电压
				ca++;
				break;
			case 3:
				LCDshowAV();						//CCT第四屏3-4路电流电压						
				ca++;
				break;
			case 4:
				LCDshowAV();					     //CCT第五屏5-6路电流电压	
				ca++;
				break;
			case 5:
				LCDshowAV();					     //CCT第五屏7-8路电流电压	
				ca++;
				break;
			case 6:
				LCDshowAV();					     //CCT第五屏9-10路电流电压	
				ca++;
				break;
			case 7:
				LCDshowAV();					     //CCT第五屏11-12路电流电压	
				ca++;
				break;
			case 8:
				RS485_CheckSTCs();                   //CCT第四屏电力检测与网络状态
				ConnectToLCD();
				LCDUpdate(2);
				ca=0;	
				break;
		}	
#endif
}









