/*****************************************************
*���ߣ�   CCT
*ʱ�䣺   2010/11/09-
*���ܣ�   ����������У���Ҫʵ����ʱ��ĵ��ȣ����а���
		  ��ʾ��������485�����ӳ���ȵȡ�����������
		  �ٺ�



******************************************************/

#include "TCPIP Stack/TCPIP.h"
#include "MainDemo.h"

BOOL b_switch ;
void TmTsk(void)
{
	static int ca = 0;
	PowerVtoDis();			//mayee ��д�ĵ���Ʒ��ѹ��ʾ���ݴ�������2012-5-29
#if defined(POWER_SUPPLY_12)
		switch(ca)
		{
			case 0:
				memset(LCDText,' ',64);
				DisplayTime();						//��һ�� ��ʾʱ��
				DisplayIPValue(AppConfig.MyIPAddr);	       // 2����ʾIP
				TPHIConvert();
				DisplayTPHI();		
				LCDUpdate(4);
				ca++;
				break;
			case 1:

				LCDshowAV();						//�ڶ���1-4·������ѹ
				ca++;
				break;
			case 2:

				LCDshowAV();						//������5-8·������ѹ						
				ca++;
				break;
			case 3:
	
				LCDshowAV();						//������9-12·������ѹ
				ca++;
				break;
			case 4:
				RS485_CheckSTCs();
				ConnectToLCD();
		//		PowerToLCD();                  //CCT ����ط� ��û��Ū���ء� Ҫ�ȵ��ˡ�
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
				DisplayTime();						//CCT��һ�� ��ʾʱ��
				DisplayIPValue(AppConfig.MyIPAddr);	       // 2����ʾIP		
				LCDUpdate(2);
				ca++;
				break;
            case 1:
				memset(LCDText,' ',32);           
				TPHIConvert();                      //CCT�ڶ��� ��ʾ��ʪ��
				DisplayTPHI();
				LCDUpdate(2);
				ca++;
				break;
			case 2:
				LCDshowAV();						//CCT������1-2·������ѹ
				ca++;
				break;
			case 3:
				LCDshowAV();						//CCT������3-4·������ѹ						
				ca++;
				break;
			case 4:
				LCDshowAV();					     //CCT������5-6·������ѹ	
				ca++;
				break;
			case 5:
				RS485_CheckSTCs();                   //CCT�������������������״̬
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
				DisplayTime();						//CCT��һ�� ��ʾʱ��
				DisplayIPValue(AppConfig.MyIPAddr);	       // 2����ʾIP		
				LCDUpdate(2);
				ca++;
				break;
            case 1:
				memset(LCDText,' ',32);           
				TPHIConvert();                      //CCT�ڶ��� ��ʾ��ʪ��
				DisplayTPHI();
				LCDUpdate(2);
				ca++;
				break;
			case 2:
				LCDshowAV();						//CCT������1-2·������ѹ
				ca++;
				break;
			case 3:
				LCDshowAV();						//CCT������3-4·������ѹ						
				ca++;
				break;
			case 4:
				LCDshowAV();					     //CCT������5-6·������ѹ	
				ca++;
				break;
			case 5:
				LCDshowAV();					     //CCT������7-8·������ѹ	
				ca++;
				break;
			case 6:
				LCDshowAV();					     //CCT������9-10·������ѹ	
				ca++;
				break;
			case 7:
				LCDshowAV();					     //CCT������11-12·������ѹ	
				ca++;
				break;
			case 8:
				RS485_CheckSTCs();                   //CCT�������������������״̬
				ConnectToLCD();
				LCDUpdate(2);
				ca=0;	
				break;
		}	
#endif
}









