/*****************************************************
*作者：   CCT
*时间：   2010/11/11
*功能：   在这个程序中，主要实现了对设备的检测
		 
		  



******************************************************/

#include "TCPIP Stack/TCPIP.h"
#include "MainDemo.h"

extern DS1302_VAL DS1302REG;
BYTE rs485_check1,rs485_check2;

#if defined(POWER_SUPPLY_12)
////////////////////////////////////////////////////////////CCT检测两个巡检芯片是否工作正常

char RS485_CheckSTCs(void)
{
	memset(LCDText,' ',64);
	if(rs485_check1==0&&rs485_check2==1)
	{
		strcpypgm2ram((char*)LCDText, "电力采集:1-6异常"); //                
	}else if(rs485_check1==1&&rs485_check2==0 )
	{
		strcpypgm2ram((char*)LCDText, "电力采集:7-C异常"); //                
	}else if(rs485_check1==1&&rs485_check2==1)
	{
		strcpypgm2ram((char*)LCDText, "电力采集:   正常"); //                 
	}else if(rs485_check1==0&&rs485_check2==0)
	{
		strcpypgm2ram((char*)LCDText, "电力采集:   异常"); //                
	}else
	{
		strcpypgm2ram((char*)LCDText, "电力采集:   测试"); //    
	}

	rs485_check1=0;
	rs485_check2=0;
}

///////////////////////////////////////////////////////////CCT 	检测正常运行时间
void RunTime(void)
{
	static BYTE dhh='0',dh='0',dl='0',hh='0',hl='0',mh='0',ml='0',beforetime = 0,f=0;
	if(f == 0)
	{
	 beforetime=DS1302REG.MIN;
	 f=1;  
    }
	if(f==1)
	{
		
		if(DS1302REG.MIN == beforetime)
		{
			
		}else if(DS1302REG.MIN != beforetime)
		{
			beforetime=DS1302REG.MIN;
			ml+=1;
			if(ml == '0'+10)
			{
				ml='0';				
				mh+=1;
				if(mh == '0'+6)
				{
					mh='0';
					hl+=1 ;
					if(hl == '0'+10)
					{
						hl ='0';
						hh+=1;											
					}
					if((hl >= '0'+4)&&(hh >= '0'+2))
					{
							hl='0';
							hh='0';
							dl+=1;
							if(dl=='0'+10)
							{
								dl='0';
								dh+=1;
								if(dh=='0'+10)
								{
									dh='0';
									dhh+=1;
								}
							}
					}	
				}
			}
		}

	}
 
	if((dl<='0')&&(dh<='0')&&(dhh<='0'))
	{
		strcpypgm2ram(((char*)LCDText)+48, "运行时间：");///////
		 LCDText[58] = hh;
		 LCDText[59] = hl;
		 LCDText[60] = ':';
		 LCDText[61] = mh;	 
		 LCDText[62] = ml;		
	}else
	{
		strcpypgm2ram(((char*)LCDText)+48, "运行:");///////
		 
		if(dhh<='0')
		 LCDText[53] = ' ';
		 else
		 LCDText[53] = dhh;

		if(dhh<='0'&&dh<='0')
		 LCDText[54] = ' ';
		 else
		 LCDText[54] = dh;

		 LCDText[55] = dl;
		strcpypgm2ram(((char*)LCDText)+56, "天");///////			
		 LCDText[58] = hh;
		 LCDText[59] = hl;
		 LCDText[60] = ':';
		 LCDText[61] = mh;	 
		 LCDText[62] = ml;
	}
}
#elif defined(POWER_SUPPLY_6)

////////////////////////////////////////////////////////////CCT检测两个巡检芯片是否工作正常
char RS485_CheckSTCs(void)
{
	memset(LCDText,' ',32);
    if(rs485_check1==0)
    	strcpypgm2ram((char*)LCDText, "collect AC:ERROR");/////////////
    else if(rs485_check1 ==1)
		strcpypgm2ram((char*)LCDText, "collect AC:OK");
    else
        strcpypgm2ram((char*)LCDText, "collect AC:TEST");
	rs485_check1=0;
}
#elif defined(POWER_SUPPLY_12_1602)
char RS485_CheckSTCs(void)
{
	memset(LCDText,' ',32);
    if(rs485_check1==0||rs485_check2==0)
    	strcpypgm2ram((char*)LCDText, "collect AC:ERROR");/////////////
    else if(rs485_check1 ==1&&rs485_check2==1)
		strcpypgm2ram((char*)LCDText, "collect AC:OK");
    else
        strcpypgm2ram((char*)LCDText, "collect AC:TEST");
	rs485_check1=0;
	rs485_check2=0;
}

#endif



