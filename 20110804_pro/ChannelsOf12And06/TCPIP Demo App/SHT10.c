/*****************************************************
*作者：   谭代华， CCT
*时间：   2010/12/15-
*功能：   在这个程序中，主要实现了继电器的延时启动功能温湿度
		  采集，处理

*修改	  2011/01/09 CCT 增加CalcSht10（）函数
						 完善GetTPHI（）复位问题 调度问题 删除未用函数	  



******************************************************/

#include "TCPIP Stack/TCPIP.h"

#define noACK 0
#define ACK 1
#define STATUS_REG_W 0x06 //000 0011 0
#define STATUS_REG_R 0x07 //000 0011 1
#define MEASURE_TEMP 0x03 //000 0001 1
#define MEASURE_HUMI 0x05 //000 0010 1
#define RESET 0x1e        //000 1111 0
unsigned int humi_val = 0,temp_val = 0;
unsigned float  disp_humi = 0,disp_temp = 0;
unsigned char error = 0,p_checksum = 0;

//----------------------------------------------------------------------------------
char s_write_byte(unsigned char value)
//----------------------------------------------------------------------------------
// writes a byte on the Sensibus and checks the acknowledge
   {
    unsigned char i,error=0;
    
	THemp_SDA_TRIS = 0;
	THemp_SDA_OUT = 0;  
    for (i=0x80;i>0;i/=2) //shift bit for masking
       { if (i & value)
           THemp_SDA_OUT=1; //masking value with i , write to SENSI-BUS
         else
           THemp_SDA_OUT=0;
         THemp_SCL_IO = 1; //clk for SENSI-BUS
		 Delay10us(1);
         THemp_SCL_IO=0;
		 Delay10us(1);
       }
    THemp_SDA_OUT = 1; //release DATA-line
    THemp_SCL_IO = 1; //clk #9 for ack
	Delay10us(1);

	THemp_SDA_TRIS = 1;
    error=THemp_SDA_IN; //check ack (DATA will be pulled down by SHT11)
    THemp_SCL_IO=0;
	Delay10us(1);
return error; //error=1 in case of no acknowledge
}
//----------------------------------------------------------------------------------
char s_read_byte(unsigned char ack)
//----------------------------------------------------------------------------------
// reads a byte form the Sensibus and gives an acknowledge in case of "ack=1"
  {
     unsigned char i,val=0;
     THemp_SDA_IN = 1; //release DATA-line
	 THemp_SDA_TRIS = 1;

     for (i=0x80;i>0;i/=2) //shift bit for masking
     { 
		 THemp_SCL_IO=1; //clk for SENSI-BUS
         Delay10us(1);
       if (THemp_SDA_IN) val=(val | i); //read bit
       THemp_SCL_IO=0;
         Delay10us(1);
     }
	 THemp_SDA_TRIS = 0;     //在这里理论是输出
     THemp_SDA_OUT=!ack; //in case of "ack==1" pull down DATA-Line
     THemp_SCL_IO=1; //clk #9 for ack
         Delay10us(1);
 //pulswith approx. 5 us
     THemp_SCL_IO=0;
         Delay10us(1);
     THemp_SDA_OUT=1; //release DATA-line
     return val;
 }


//----------------------------------------------------------------------------------
void s_transstart(void)
//----------------------------------------------------------------------------------
// generates a transmission start
// _____ ________
// DATA: |_______|
// ___ ___
// SCK : ___| |___| |______
  {
	THemp_SDA_TRIS = 0;
    THemp_SDA_OUT=1;
    THemp_SCL_IO=0; //Initial state
         Delay10us(1);
    THemp_SCL_IO=1;
                  Delay10us(1);
    THemp_SDA_OUT=0;
    THemp_SCL_IO=0;
                 Delay10us(1);
    THemp_SCL_IO=1;
                 Delay10us(1);
    THemp_SDA_OUT=1;
    THemp_SCL_IO=0;         Delay10us(1);

}




//----------------------------------------------------------------------------------
void s_connectionreset(void)                   //重启1步
//----------------------------------------------------------------------------------
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
// _____________________________________________________ ________
// DATA: |_______|
// _ _ _ _ _ _ _ _ _ ___ ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______| |___| |______
  {
	
    unsigned char i;
	THemp_SCL_TRIS = 0;
   	THemp_SDA_OUT=1;
	THemp_SDA_TRIS = 0;
 
    THemp_SCL_IO=0; //Initial state
         Delay10us(1);
    for(i=0;i<9;i++) //9 SCK cycles
    {
		 THemp_SCL_IO=1;
         Delay10us(1);
      	 THemp_SCL_IO=0;
         Delay10us(1);
    }
  s_transstart(); //transmission start
  }
/*

//----------------------------------------------------------------------------------
 char s_softreset(void)
//----------------------------------------------------------------------------------
// resets the sensor by a softreset
  {
    unsigned char error=0;
    s_connectionreset(); //reset communication
    error+=s_write_byte(RESET); //send RESET-command to sensor
    return error; //error=1 in case of no response form the sensor
  }



//----------------------------------------------------------------------------------
char s_read_statusreg(unsigned char *p_value, unsigned char *p_checksum)
//----------------------------------------------------------------------------------
// reads the status register with checksum (8-bit)
 {
  unsigned char error=0;
         //SHTxx Application Note Sample Code
         //www.dabeco.com.cn Rev 2.01 3/4
  s_transstart(); //transmission start
  error=s_write_byte(STATUS_REG_R); //send command to sensor
  *p_value=s_read_byte(ACK); //read status register (8-bit)
  *p_checksum=s_read_byte(noACK); //read checksum (8-bit)
   return error; //error=1 in case of no response form the sensor
 }


//----------------------------------------------------------------------------------
char s_write_statusreg(unsigned char *p_value)
//----------------------------------------------------------------------------------
// writes the status register with checksum (8-bit)
  {
    unsigned char error=0;
    s_transstart(); //transmission start
    error+=s_write_byte(STATUS_REG_W);//send command to sensor
    error+=s_write_byte(*p_value); //send value of status register
    return error; //error>=1 in case of no response form the sensor
  }

*/
/////////////////////////////////////////////////////////////////////////////////CCT 校正温湿度
void CalcSht10(float *p_humi,float *p_temp)
{
	float rh = *p_humi;
	float t  = *p_temp;
	float rh_line;
	float rh_ture;
	float t_c;
	float c1 = -2.0468;
	float c2 = 0.0367;
	float c3 = -0.0000015955;
	float t1 = 0.01;
	float t2 = 0.00008;
	
	rh_line = c1+c2*rh+c3*rh*rh;
	t_c = 0.01*t-40.1;
	rh_ture = (t_c-25)*(t1+t2*rh)+rh_line;
	if(rh_ture>100)
		rh_ture = 100;
	if(rh_ture<0.1)
		rh_ture = 0.1;
	*p_humi = rh_ture;
	*p_temp = t_c;
}
/////////////////////////////////////////////////////////////////////////////////CCT ，谭代华，这个地方是中断函数。。。从485发来的实现了
///////////////////////////////////////////////////////////////////////////////////////////////SHT10的调度。
void GetTPHI(void)
{
	static enum 
	{
		SHTINIT,
		TEMPWRITE,
		TEMPREAD,
		HUMIWRITE,
		HUMIREAD,
		TEMPCONVERT,
		HUMICONVERT,
		CHECKERROR,
		CORRECTION
	}SHT10NOW = TEMPWRITE;	

	switch(SHT10NOW)		                    
	{
   		 case SHTINIT:                               //CCT初始化
			  s_connectionreset();
			  error = 0;
			  SHT10NOW = TEMPWRITE;  
			  break;

		 case TEMPWRITE:                             //CCT温度检测命令
			  error = 0;
			  s_transstart(); 
			  error+=s_write_byte(MEASURE_TEMP);
			    SHT10NOW = TEMPREAD;  
			  break;		
   		 case TEMPREAD:                               //CCT温度读
      		 temp_val=s_read_byte(ACK); //read the first byte (MSB)
			temp_val<<=8;
      		temp_val+=s_read_byte(ACK); //read the second byte (LSB)
      		p_checksum =s_read_byte(noACK); //read checksum  			
			  SHT10NOW = HUMIWRITE;
			  break;
   		 case HUMIWRITE:                              //CCT湿度检测命令
			  s_transstart(); 
			  error+=s_write_byte(MEASURE_HUMI);
			    SHT10NOW = HUMIREAD; 
			  break;
  		 case HUMIREAD :                            //单片机读湿度
      		 humi_val=s_read_byte(ACK); //read the first byte (MSB)
			humi_val<<=8;
      		humi_val+=s_read_byte(ACK); //read the second byte (LSB)
      		p_checksum =s_read_byte(noACK); //read checksum
			  SHT10NOW = CHECKERROR;
			  break;
		 case CHECKERROR :
			if(error!=0)
				SHT10NOW=SHTINIT;
			else
				SHT10NOW = CORRECTION;
			break;
		 case CORRECTION :
			disp_humi=(float)humi_val;humi_val=0;
			disp_temp=(float)temp_val;temp_val=0;
			CalcSht10(&disp_humi,&disp_temp);
			SHT10NOW = TEMPWRITE;
			break;
		
						
	}
}


        

