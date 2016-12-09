

#include "STC12C5620ad_H.h"
#include "intrins.h"

#define ISP_IAP_BYTE_READ    0x01
#define ISP_IAP_BYTE_PROGRAM 0x02
#define ISP_SECTOR_ERASE     0x03
#define ENABLE_ISP           0x85

/*=======================================================================
�������ƣ�ISP_Disable
�������ܣ���ֹIAP����
���������
�����������
�� �� ֵ��
����˵����
=======================================================================*/
void ISP_Disable( void ){

    ISP_CONTR = 0x00;
    ISP_CMD   = 0x00;
    ISP_TRIG  = 0x00;
    ISP_ADDRH = 0xFF;
    ISP_ADDRL = 0xFF;

}

/*=======================================================================
�������ƣ�ISP_Byte_Read
�������ܣ���EEPROM�е�һ���ֽڣ���Ҫ���ݵ�ַ
���������
�����������
�� �� ֵ����ȡ������
����˵����
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
    // �ӳ�
    return Temp_Data;

}

/*=======================================================================
�������ƣ�ISP_Byte_Write
�������ܣ�д��EEPROM�е�һ���ֽڣ���Ҫ���ݵ�ַ��Ҫд�������
���������Ҫ������������ַ
�����������
�� �� ֵ����
����˵����
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
�������ƣ�Sector_Erase
�������ܣ���������
���������Ҫ������������ַ
�����������
�� �� ֵ����
����˵����
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














