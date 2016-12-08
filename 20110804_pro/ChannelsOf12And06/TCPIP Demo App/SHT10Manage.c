/*****************************************************
*作者：   CCT
*时间：   2010/12/16
*功能：   处理并把信息显示到液晶屏上

*修改：   2011/01/09   cct 修改TPHIConvert()使支持FLOAT型数据

		 
		  



******************************************************/

#include "TCPIP Stack/TCPIP.h"
#include "MainDemo.h"

//extern unsigned int humi_val,temp_val;
extern unsigned float disp_humi,disp_temp;
extern BYTE TCPSendTempHumi[10];
BYTE TPHIString[10];
BYTE TP[6];
BYTE HI[6];
//////////////////////////////////////////////////////////////////////////////////////cct 转换校正过的温湿度信息到BYTE XX[10];
void TPHIConvert(void)
{
	WORD digit,humitemp,temptemp;
	humitemp = (WORD)(disp_humi*100);	//CCT  把浮点数*100转换为整数（强行转换比运算优先级要高）
	temptemp = (WORD)(disp_temp*100);
	disp_humi=0;
	disp_temp=0;
	digit=temptemp/1000;
	if(digit)
		TPHIString[0] = '0'+digit;
	else
		TPHIString[0] = ' ';
	temptemp -= digit*1000;
	
	digit=temptemp/100;
	TPHIString[1] = '0'+digit;	
	temptemp -= digit*100;
	
	TPHIString[2] = '.';

	digit=temptemp/10;
	TPHIString[3] = '0'+digit;	
	temptemp -= digit*10;

	TPHIString[4] = '0'+temptemp;


	digit=humitemp/1000;
	if(digit)
		TPHIString[5] = '0'+digit;
	else
		TPHIString[5] = ' ';
	humitemp -= digit*1000;
	
	digit=humitemp/100;
	TPHIString[6] = '0'+digit;	
	humitemp -= digit*100;
	
	TPHIString[7] = '.';

	digit=humitemp/10;
	TPHIString[8] = '0'+digit;	
	humitemp -= digit*10;

	TPHIString[9] = '0'+humitemp;
}

//////////////////////////////////////////////////////////////////////////////////////cct 把BYTE XX[10]在液晶屏上显示;
void DisplayTPHI(void)
{
#if defined(POWER_SUPPLY_12)
   strcpypgm2ram(((char*)LCDText)+32, "温度:");
	LCDText[37] = TPHIString [0];
	LCDText[38] = TPHIString [1];
	LCDText[39] = TPHIString [2];
	LCDText[40] = TPHIString [3];
	LCDText[41] = TPHIString [4];
    LCDText[42] = 0xA1;
	LCDText[43] = 0xe6;
    
   strcpypgm2ram(((char*)LCDText)+48, "湿度:");
	LCDText[53] = TPHIString [5];
	LCDText[54] = TPHIString [6];
	LCDText[55] = TPHIString [7];
	LCDText[56] = TPHIString [8];
	LCDText[57] = TPHIString [9];
    LCDText[58] = 0xA3;
    LCDText[59] = 0xA5;

#elif defined(POWER_SUPPLY_6)||defined(POWER_SUPPLY_12_1602)
	memset(LCDText,' ',32);
    strcpypgm2ram((char*)LCDText, "TEMP:");

	LCDText[5] = TPHIString [0];
	LCDText[6] = TPHIString [1];
	LCDText[7] = TPHIString [2];
	LCDText[8] = TPHIString [3];
	LCDText[9] = TPHIString [4];
	LCDText[10] = 0XDF;
	LCDText[11] = 'C';

   strcpypgm2ram(((char*)LCDText)+16, "HUMI:");
	LCDText[21] = TPHIString [5];
	LCDText[22] = TPHIString [6];
	LCDText[23] = TPHIString [7];
	LCDText[24] = TPHIString [8];
	LCDText[25] = TPHIString [9];
	LCDText[26] = ' ';
	LCDText[27] = '%';
#endif

 	strcpy((char*)TCPSendTempHumi, (char*)TPHIString);      //CCT SCOKET 发送温度湿度

    strncpy((char*)TP,(char*)TPHIString,5);                 //CCT 网页改善温湿度
    TP[5]='\0';
	strncpy((char*)HI,&TPHIString[5],5); 
	HI[5]='\0'; 
	memset(TPHIString,'\0',10);
}


