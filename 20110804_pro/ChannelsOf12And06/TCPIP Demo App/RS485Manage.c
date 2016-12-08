/*****************************************************
*���ߣ�   CCT
*ʱ�䣺   2010/11/09-
*���ܣ�   ����������У���Ҫʵ�ֵ���485��������12C5608��
		  ͬʱ����5608���������ݵȵȡ�
		  



******************************************************/
#include "TCPIP Stack/TCPIP.h"
void RS485_SetMAC(void);
void RS485_SetDS1302(void);
#if defined(STACK_USE_RS485)
BOOL b_RS485_Success;
struct _RS485Frame
{
	BYTE DATNum ;		//����׼�����յ����ݸ���
	BYTE RCBUF[29];		//���ڽ��ջ�����
	BYTE TXBUF[7];		//���ڷ��ͻ�����
}RS485;

extern BYTE AVs[12][4];
extern BYTE rs485_check1,rs485_check2;

///�����//////////////////////////////////////////////////////////////////////////////�ܵ������ܵ�ѹ
extern BYTE testV[6]; 			//��ʽ{'2','3','0','.','4',0x00} ��ѹ
extern BYTE shareA[6];			//��ʽ{' ','0','.','0','0',0x00} ����

extern char DataArray1[3];    	//??


BYTE rs486send[]={0x4c,0x57,0x0f,0x03,0x00,0x00,0x0d};

///////////////////////////////////////////////////////////////////////////cct������485�����е�������Ҫ���Ƶ�AVs(AVsΪ������ѹ����)

void RS485_Function(void)        
{
	BYTE i;
	int a,b,c=3;
	if(b_RS485_Success == 0)                            //CCT��ǰ��仰�������ã������ò����ˣ��������Űɣ���ɾ�ˡ�
		return;
	b_RS485_Success = 0;
	if(RS485.RCBUF[0]==0x6c)
	{
#if defined(POWER_SUPPLY_12)||defined(POWER_SUPPLY_12_1602)
				for(a=0;a<6;a++)                       //CCT  AVs12���θ�ֵ
				{
					for(b=0;b<4;b++,c++)
					{
						if(RS485.RCBUF[2] == 0x0e)     //CCT��Ϊ������485���Է�������ȷ��
						{
							AVs[a][b] = RS485.RCBUF[c];
							rs485_check1=1;            //CCT ���ڼ���Ƿ�485ͨ���ж�
						}
						else if(RS485.RCBUF[2] == 0x0f)
						{	AVs[a+6][b] = RS485.RCBUF[c];
							rs485_check2=1;
						}	
					}
				}
#elif defined(POWER_SUPPLY_6)
				for(a=0;a<6;a++)                       //CCT  AVs6���θ�ֵ
				{
					for(b=0;b<4;b++,c++)
					{
							AVs[a][b] = RS485.RCBUF[c];	
					}
				}		   
	        	rs485_check1=1;           			   //CCT ���ڼ���Ƿ�485ͨ���ж�
#endif
	}  else if(RS485.RCBUF[0]==0x6d)
	{
        RS485_SetMAC();
		RS485_SetDS1302();	
	}
}

/////////////////////////////////////////////////////////////////////////////////CCTͨ����������MAC��ַ
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
/////////////////////////////////////////////////////////////////////////////////CCTͨ����������ʱ��
void RS485_SetDS1302(void)
{
	BYTE i;
	RTC_RST_IO = 0;
	RTC_SCL_IO = 0;
	RTC_RST_TRIS = 0;
	RTC_SCL_TRIS = 0;
	RTC_SDA_TRIS = 1;

		WriteDS1302( Add_CONTROL,0x00 );		//�ر�д����
		WriteDS1302( Add_MIN,RS485.RCBUF[10] );	//Ԥ�÷���ʱ��
		WriteDS1302( Add_HR,RS485.RCBUF[11] );	//Ԥ��Сʱ��24Сʱ��
		WriteDS1302( Add_DATE,RS485.RCBUF[12] );//Ԥ������
		WriteDS1302( Add_MONTH,RS485.RCBUF[13] );//Ԥ���·�
		WriteDS1302( Add_DAY,0x05 );			//Ԥ������
		WriteDS1302( Add_YEAR,RS485.RCBUF[14] );//Ԥ�����
		WriteDS1302( Add_CHARGER,0x0a6 );		//д�����ƼĴ�����1010 0110��I=1mA
		WriteDS1302( Add_SEC,0x08 );			//����ʱ��
		WriteDS1302( Add_RAM0,0x50);			//RAM0��Ԫд��0x55����ֹ�ٴγ�ʼ��
		WriteDS1302( Add_CONTROL,0x80 );		//��д���� 
	
	DelayMs(10);
}

/////////////////////////////////////////////////////////////////////////////////CCT����ط����жϺ�����������485������
void RS485_RC_ISR(void)
{
	BYTE temp;
	WORD i_delay;
	if(PIR3bits.RC2IF)   //���ջ���������־λ
	{
		PIR3bits.RC2IF = 0;    //��־λ�Զ�����
		RS485.RCBUF[0] = RCREG2;   //����׼�����յ����ݸ���
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
	
///////////////////////////////////////////////////////////////////////////////////////cct �����ݵ�485
void RS485_SendRequest(void)
{ 
			int rsint  = 0;
			RS485_DIR_IO = Send;
			DelayMs(1);

#if defined(POWER_SUPPLY_12)||defined(POWER_SUPPLY_12_1602)
	
			if(rs486send[2] == 0x0f)		//CCTÿ3s�Զ�Ѳ������оƬ������ѹ
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
////////////////////////////////////////////////////////////////////cct �ܵ���
//��ʽ{' ','0','.','0','0',0x00} ����
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
//////mayee ����������С�����һλ////��ʽ{' ','0','.','0','0',0x00} ����
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

/////////////////////////////////////////////////////////////////////////////////////////cct �ܹ���
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
///mayee ���ڸİ��Ժ�ÿ���ɼ���ֻ��һ����ѹ���У�ֻ�ɼ��ܵ�ѹ
//�������ܣ����ɼ��ĵ�ѹ���ݴ洢�� BYTE testV[6]; ����			//��ʽ{'2','3','0','.','4',0x00} ��ѹ
//ע:�ϰ���Ļ��ʾ���ݶ�д��LCDText[]�����У�����CCT�ĸĺ�ĳ�����Ϊ�˲�Ӱ�����ݽṹ�������ĳ���һֱ�ڱ�����
//�����С��κ�����Ŀ�ľ���Ϊ�����������������հ�����ɾ��
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
		ctemp = (BYTE)(sum/10000); //mayee ��ѹ��λ
		ctemp = ctemp + '0'; 
		if(ctemp == '0')
			testV[0] = ' ';
		else	
			testV[0] = ctemp; 
	
		sum = sum % 10000;			//mayee ��ѹʮλ
		ctemp = (BYTE)(sum/1000); 
		ctemp = ctemp + '0'; 
			if(ctemp == '0')
			testV[1] = ' ';
		else	
			testV[1] = ctemp;
	
		sum = sum % 1000;			//mayee ��ѹ��λ
		ctemp = (BYTE)(sum/100); 
		ctemp = ctemp + '0'; 	
		testV[2] = ctemp;
	
		testV[3] = '.';				//С����
	
		sum = sum % 100;			//mayee ��ѹС�����һλλ
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
		ctemp = (BYTE)(sum/10000); //mayee ��ѹ��λ
		ctemp = ctemp + '0'; 
		if(ctemp == '0')
			testV[0] = ' ';
		else	
			testV[0] = ctemp; 
	
		sum = sum % 10000;			//mayee ��ѹʮλ
		ctemp = (BYTE)(sum/1000); 
		ctemp = ctemp + '0'; 
			if(ctemp == '0')
			testV[1] = ' ';
		else	
			testV[1] = ctemp;
	
		sum = sum % 1000;			//mayee ��ѹ��λ
		ctemp = (BYTE)(sum/100); 
		ctemp = ctemp + '0'; 	
		testV[2] = ctemp;
	
		testV[3] = '.';				//С����
	
		sum = sum % 100;			//mayee ��ѹС�����һλλ
		ctemp = (BYTE)(sum/10); 
		ctemp = ctemp + '0'; 
		testV[4] = ctemp;
		testV[5] = '\0';	

		sum = sum/100;
	}
#endif
//	return ;
}
////////////////////////////////////////////////////////////////////////////////////////////cct LCD��ʾ�ܹ���
void PowerToLCD(void)
{
	QWORD AVSum,z=10000;
	BYTE x,y;
	AVSum = PowerSum();
	strcpypgm2ram((char *)&LCDText[32],"������ʣ�");///////
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







