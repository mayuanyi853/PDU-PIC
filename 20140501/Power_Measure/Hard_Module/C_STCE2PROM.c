

#include "STC12C5620ad_H.h"
#include "intrins.h"

#define ISP_IAP_BYTE_READ    0x01
#define ISP_IAP_BYTE_PROGRAM 0x02
#define ISP_SECTOR_ERASE     0x03
#define ENABLE_ISP           0x85

/*=======================================================================
函数名称：ISP_Disable
函数功能：禁止IAP操作
输入参数：
输出参数：无
返 回 值：
其它说明：
=======================================================================*/
void ISP_Disable( void ){

    ISP_CONTR = 0x00;
    ISP_CMD   = 0x00;
    ISP_TRIG  = 0x00;
    ISP_ADDRH = 0xFF;
    ISP_ADDRL = 0xFF;

}

/*=======================================================================
函数名称：ISP_Byte_Read
函数功能：读EEPROM中的一个字节，需要传递地址
输入参数：
输出参数：无
返 回 值：读取的数据
其它说明：
=======================================================================*/
unsigned char ISP_Byte_Read( unsigned int Sector_Address ){

    unsigned char Temp_Data;

    ISP_CONTR = ENABLE_ISP;
    ISP_CMD   = ISP_IAP_BYTE_READ;
    ISP_ADDRH = Sector_Address / 256;
    ISP_ADDRL = Sector_Address % 256;
    //ISP_TRIG  = 0x5A;
    //ISP_TRIG  = 0xA5;
    
    ISP_TRIG  = 0x46;
    ISP_TRIG  = 0xB9;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();

    Temp_Data = ISP_DATA;
    ISP_Disable();
    // 延迟
    return Temp_Data;

}

/*=======================================================================
函数名称：ISP_Byte_Write
函数功能：写入EEPROM中的一个字节，需要传递地址和要写入的数据
输入参数：要擦除的扇区地址
输出参数：无
返 回 值：无
其它说明：
=======================================================================*/
void ISP_Byte_Write( unsigned int Sector_Address, unsigned char Write_Data ){

    ISP_CONTR = ENABLE_ISP;
    ISP_CMD   = ISP_IAP_BYTE_PROGRAM;
    ISP_ADDRH = Sector_Address / 256;
    ISP_ADDRL = Sector_Address % 256;
    ISP_DATA  = Write_Data;
   // ISP_TRIG  = 0x5A;
    //ISP_TRIG  = 0xA5;

    ISP_TRIG  = 0x46;
    ISP_TRIG  = 0xB9;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();

    ISP_Disable();

}

/*=======================================================================
函数名称：Sector_Erase
函数功能：扇区擦除
输入参数：要擦除的扇区地址
输出参数：无
返 回 值：无
其它说明：
=======================================================================*/
void Sector_Erase( unsigned int Sector_Address ){

    ISP_CONTR = ENABLE_ISP;
    ISP_CMD   = 0x03;
    ISP_ADDRH = Sector_Address / 256;
    ISP_ADDRL = Sector_Address % 256;
    //ISP_TRIG  = 0x5A;
    //ISP_TRIG  = 0xA5;

    ISP_TRIG  = 0x46;
    ISP_TRIG  = 0xB9;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();

    ISP_Disable();

}














