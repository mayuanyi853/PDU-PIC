
/*********************************************************************

*作者：   CCT   （主要是微芯的那些大哥们，因为他们写的函数里要改的东西太多，我就单起用了一个文件）
*时间：   2010/11/15-
*功能：   在这个程序中，主要实现PING的应用，同时想法把送用到LCD屏上

*修改：	  2011/06/08 这个地方从些以后不用了，以后只检测与DELPHI通信 状况
		  



 ********************************************************************/
#define __PINGDEMO_C

#include "TCPIP Stack/TCPIP.h"
#include "MainDemo.h"
#if defined(STACK_USE_ICMP_CLIENT)
 static struct PCo
{
	BYTE timeout ;		
	WORD timesum ;
} PingCount;


/////////////////////////////////////////////////////////////////////////////////////CCT   PING一个地址，把状态给一个数组
void PingCheck(void)
{
	static enum
	{
		SM_HOME = 0,
		SM_GET_RESPONSE
	} PingState = SM_HOME;
	static TICK Timer = 0;
	LONG ret;
	IP_ADDR RemoteIP;

	switch(PingState)
	{
		case SM_HOME:
			// Send a ping request out if the user pushes BUTTON0 (right-most one)

				// Don't ping flood: wait at least 1 second between ping requests
				if(TickGet() - Timer > 8ul*TICK_SECOND)
				{
					// Obtain ownership of the ICMP module
					if(ICMPBeginUsage())
					{
						Timer = TickGet();
						PingState = SM_GET_RESPONSE;
	
						// Send the ping request to 63.217.8.154 (ww1.microchip.com)
						RemoteIP.v[0] = 123;  //
						RemoteIP.v[1] = 125;
						RemoteIP.v[2] = 115;
						RemoteIP.v[3] = 90;
						ICMPSendPing(RemoteIP.Val);
					}
				}
			
			break;

		case SM_GET_RESPONSE:
			// Get the status of the ICMP module

			ret = ICMPGetReply();					
			if(ret == -2)
			{
				// Do nothing: still waiting for echo
				break;
			}
			else if(ret == -1)// Request timed out
			{			
				PingCount.timeout=1;
				PingState = SM_HOME;
			}
			else
			{
				// Echo received.  Time elapsed is stored in ret (units of TICK).

		//		memcpypgm2ram((void*)&LCDText[16], (ROM void *)"Reply: ", 7);
		//		uitoa((WORD)TickConvertToMilliseconds((DWORD)ret), &LCDText[16+7]);
				PingCount.timeout = 0;
				PingCount.timesum = (WORD)ret;
	//			uitoa((WORD)TickConvertToMilliseconds((DWORD)ret), &PingCount.timesum);
		//		strcatpgm2ram((char*)&LCDText[16+7], (ROM char*)"ms");


				PingState = SM_HOME;
			}
			
			// Finished with the ICMP module, release it so other apps can begin using it
			ICMPEndUsage();
			break;
	}
}

#if defined(POWER_SUPPLY_12)
void PingToLCD(void)
{
	if(PingCount.timeout == 1)
	{
	strcpypgm2ram(((char*)LCDText)+16, "网络状态:   异常");///////////////
	return;
	}
	else if(PingCount.timeout == 0)
	{
		if(PingCount.timesum<10)
		{
			strcpypgm2ram(((char*)LCDText)+16, "网络状态:   很好");///////////////
			return;
		}
		else if((PingCount.timesum>=10)&&(PingCount.timesum<=150))
		{
			strcpypgm2ram(((char*)LCDText)+16, "网络状态:   一般");///////////////
			return;
		}
		else if((PingCount.timesum>150)&&(PingCount.timesum<=350))
		{
			strcpypgm2ram(((char*)LCDText)+16, "网络状态:   很慢");///////////////
			return;
		}else
		{
			strcpypgm2ram(((char*)LCDText)+16, "网络状态:   很差");///////////////
			return;
		}
		
	}
}
#elif defined(POWER_SUPPLY_6)
void PingToLCD(void)
{
	if(PingCount.timeout == 1)
	{
	strcpypgm2ram(((char*)LCDText)+16, "network   :ERROR");///////////////
	return;
	}
	else if(PingCount.timeout == 0)
	{
		if(PingCount.timesum<10)
		{
			strcpypgm2ram(((char*)LCDText)+16, "network   :OK");///////////////
			return;
		}
		else if((PingCount.timesum>=10)&&(PingCount.timesum<=150))
		{
			strcpypgm2ram(((char*)LCDText)+16, "network   :OK");///////////////
			return;
		}
		else if((PingCount.timesum>150)&&(PingCount.timesum<=350))
		{
			strcpypgm2ram(((char*)LCDText)+16, "network   :OK");///////////////
			return;
		}else
		{
			strcpypgm2ram(((char*)LCDText)+16, "network   :OK");///////////////
			return;
		}
		
	}
}
#endif  //#if defined(POWER_SUPPLY)
#endif	//#if defined(STACK_USE_ICMP_CLIENT)

