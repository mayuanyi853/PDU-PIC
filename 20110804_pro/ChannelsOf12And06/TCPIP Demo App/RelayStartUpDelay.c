/*****************************************************
*���ߣ�   CCT
*ʱ�䣺   2010/11/29-
*���ܣ�   ����������У���Ҫʵ���˼̵�������ʱ��������
*�޸ģ�   2010/01/27 CCT  		  



******************************************************/
#include "TCPIP Stack/TCPIP.h"
#include "GenericTypeDefs.h"

extern BYTE relaySet[13];
extern BYTE oldRelay[12]; 
extern BYTE DataArray1[3]; //mayee �������Ҳµ����Ǹ�ʲô���ˣ�ͨ��״̬����[0]:�и��ص�ͨ����[1]:��ͨͨ����[2]:�ر�ͨ����������ֻ����12·�ģ�û��6·�ģ�

void RelayControl(void)
{
    BYTE x;
	static BYTE relayNow = 0;
    if(relaySet[12]==2)
    {  
        relayNow = 0;
        relaySet[12]=1;
    } 
	switch (relayNow)
	{
		case 0 :
			if(relaySet[relayNow] == oldRelay[relayNow])
			{
				RELAY1_IO = relaySet[relayNow];
				relayNow++;
			}
			else
			{
				RELAY1_IO = relaySet[relayNow];
				oldRelay[relayNow] = relaySet[relayNow];
				relayNow ++;
				break;
			}			
		case 1 :
			if(relaySet[relayNow] == oldRelay[relayNow])
			{
				RELAY2_IO = relaySet[relayNow];
				relayNow++;
			}
			else
			{
				RELAY2_IO = relaySet[relayNow];
				oldRelay[relayNow] = relaySet[relayNow];
				relayNow ++;
				break;
			}
		case 2 :
			if(relaySet[relayNow] == oldRelay[relayNow])
			{
				RELAY3_IO = relaySet[relayNow];
				relayNow++;
			}
			else
			{
				RELAY3_IO = relaySet[relayNow];
				oldRelay[relayNow] = relaySet[relayNow];
				relayNow ++;
				break;
			}
		case 3 :
			if(relaySet[relayNow] == oldRelay[relayNow])
			{
				RELAY4_IO = relaySet[relayNow];
				relayNow++;
			}
			else
			{
				RELAY4_IO = relaySet[relayNow];
				oldRelay[relayNow] = relaySet[relayNow];
				relayNow ++;
				break;
			}	
		case 4 :
			if(relaySet[relayNow] == oldRelay[relayNow])
			{
				RELAY5_IO = relaySet[relayNow];
				relayNow++;
			}
			else
			{
				RELAY5_IO = relaySet[relayNow];
				oldRelay[relayNow] = relaySet[relayNow];
				relayNow ++;
				break;
			}
		case 5 :
			if(relaySet[relayNow] == oldRelay[relayNow])
			{
				RELAY6_IO = relaySet[relayNow];
				relayNow++;
			}
			else
			{
				RELAY6_IO = relaySet[relayNow];
				oldRelay[relayNow] = relaySet[relayNow];
				relayNow ++;
				break;
			}
		case 6 :
			if(relaySet[relayNow] == oldRelay[relayNow])
			{
				RELAY7_IO = relaySet[relayNow];
				relayNow++;
			}
			else
			{
				RELAY7_IO = relaySet[relayNow];
				oldRelay[relayNow] = relaySet[relayNow];
				relayNow ++;
				break;
			}
		case 7 :
			if(relaySet[relayNow] == oldRelay[relayNow])
			{
				RELAY8_IO = relaySet[relayNow];
				relayNow++;
			}
			else
			{
				RELAY8_IO = relaySet[relayNow];
				oldRelay[relayNow] = relaySet[relayNow];
				relayNow ++;
				break;
			}
		case 8 :
			if(relaySet[relayNow] == oldRelay[relayNow])
			{
				RELAY9_IO = relaySet[relayNow];
				relayNow++;
			}
			else
			{
				RELAY9_IO = relaySet[relayNow];
				oldRelay[relayNow] = relaySet[relayNow];
				relayNow ++;
				break;
			}
		case 9 :
			if(relaySet[relayNow] == oldRelay[relayNow])
			{
				RELAYA_IO = relaySet[relayNow];
				relayNow++;
			}
			else
			{
				RELAYA_IO = relaySet[relayNow];
				oldRelay[relayNow] = relaySet[relayNow];
				relayNow ++;
				break;
			}
		case 10 :
			if(relaySet[relayNow] == oldRelay[relayNow])
			{
				RELAYB_IO = relaySet[relayNow];
				relayNow++;
			}
			else
			{
				RELAYB_IO = relaySet[relayNow];
				oldRelay[relayNow] = relaySet[relayNow];
				relayNow ++;
				break;
			}
		case 11 :
			if(relaySet[relayNow] == oldRelay[relayNow])
			{
				RELAYC_IO = relaySet[relayNow];
				relayNow++;
			}
			else
			{
				RELAYC_IO = relaySet[relayNow];
				oldRelay[relayNow] = relaySet[relayNow];
				relayNow ++;
				break;
			}
		case 12 :
			relayNow = 0;
			relaySet[12]=0;
			XEEBeginWrite(0x5400);
			for(x=0;x<12;x++)
			{
				XEEWrite(relaySet[x]);
			}			    
			XEEEndWrite();

			return;
		default :
			relayNow = 0;
			relaySet[12]=0;
			return;

	}
	return;
}
void RelayOnOffCount(void)
{
	BYTE x;
	DataArray1[1]=0;
	DataArray1[2]=0;
#if defined(POWER_SUPPLY_12) // CCTֻ����12·��ͨ��״̬ͳ�ƣ�mayee���6·״̬ͳ��20130620
	for(x=0;x<12;x++)
	{
		if (relaySet[x]=='1')
		{
			DataArray1[1]+=1;						
		}
	}
	DataArray1[2]=12-DataArray1[1];
#elif defined(POWER_SUPPLY_6)
	for(x=0;x<6;x++)
	{
		if (relaySet[x]=='1')
		{
			DataArray1[1]+=1;					
		}
	}
	DataArray1[2]=6-DataArray1[1];
#endif


}






















