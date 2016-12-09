   
/*======================================================================
  ��Ȩ���У�2010--2099, �ӱ��Ƽ���ѧ  ��Ϣ��ѧ�빤��ϵ Ӧ�õ��� ��־��
  �� �� ����STC12C5404AD_CMdu.c
  ��    ��: ��־��
  ��    ��: V1.0
  ��    ��: 2010��7��26��
  ������Ϣ: STC12C5404AD��C����ģ�� ���Ӳ��ģ����Դ�ĳ�ʼ������
  ����˵��: ��ģ��ֻ�ǹ�����ϵͳ��ײ��Ӳ�����
            �������ݿɸ�����Ҫ����Ӧ�޸�
			ϵͳʱ�Ӳ���11.0592MHz            
  �����б�: 
  			1���˿ڳ�ʼ��
			2��ϵͳʱ�ӳ�ʼ��
			3����ʱ��T0��1��ʼ��
			4��ADC0��ʼ��
			5�����пڳ�ʼ��
			6��DAC0��ʼ��
			7��ϵͳ�жϳ�ʼ��
			8���Ƚ�����ʼ��
  ��ʷ��¼:   
            1����    ��:
               ��    ��:
               �޸�����:
=======================================================================*/

/***********************************************************************
              ͷ�ļ���������
***********************************************************************/
#include "STC12C5620AD_H.h"

/*=======================================================================
�������ƣ�Init_Port
�������ܣ�1���˿�ģʽ��ʼ������������������©��
          2���˿ڷ����ʼ�������롢�����
		  3���˿ڳ�ʼֵ	   ���ߵ�ƽ���͵�ƽ��
�����������
�����������
�� �� ֵ����
����˵������
=======================================================================*/
void Init_Port( void ){

	// �˿�ģʽ���ã���������������©�� 
    
	// �˿ڷ�������

	// �˿ڳ�ʼֵ����
    P0 = 0xFF; 
	P1 = 0xFF;
	P2 = 0xFF;
	P3 = 0xFF;

	/*=================================================================
	PnM0[7:0] PnM1[7:0]
	     0      0        ׼˫���
		 0      1        �������
		 1      0        �������� 
		 1      1        ��©���
	=================================================================*/
//	P0M0 = 0; P0M1 = 0;  // Ĭ��Ϊ׼˫���
//	P1M0 = 0; P1M1 = 0;  // Ĭ��Ϊ׼˫���
//	P2M0 = 0; P2M1 = 0;  // Ĭ��Ϊ׼˫���
//	P3M0 = 0; P3M1 = 0;  // Ĭ��Ϊ׼˫���


}


/*=======================================================================
�������ƣ�Init_INT0
�������ܣ�1���ⲿ�ж�0������ʽ��ʼ��
          2�����ȼ��趨
		  3�������ⲿ�ж�0�ж�
�����������
�����������
�� �� ֵ����
����˵������
=======================================================================*/
void Init_INT0( void ){

    IT0   = 1;   // �����ⲿ�ж�0Ϊ�½����ж�
	PX0   = 1;   // �ⲿ�ж�0���ȼ����
	EX0   = 0;	 // ���ⲿ�ж�0	 

}

/*=======================================================================
�������ƣ�Init_Timer0
�������ܣ�1����ʱ��T0����ģʽ��ʼ��
          2����ʱ��T0��ֵ�趨
		  3����ʱ��T0�ж�ģʽ����
		  4��6MHz����  10ms�ж�һ��	�����ж�
�����������
�����������
�� �� ֵ����
����˵������
=======================================================================*/
void Init_Timer0( void ){

	/*================================================
	      D7     D6    D5   D4    D3     D2    D1   D0
	TMOD: Gate  C/nT   M1   M0   Gate   C/nT   M1   M0
          Gate: �������ƶ�
		    1:  ���Ÿߵ�ƽʱ����   
			0:  ���ɼ���
		  C/nT: ����ʱ��Դѡ��
		    1:  �ⲿ���ż���       
			0:  �ڲ���ʱ
		  M1 M0: ������ʽѡ��
		  0   0: 13λ����
		  0   1: 16λ����
		  1   0: 8λ�Զ���װ��ģʽ
		  1   1: T0Ϊ˫8λ����
	================================================*/
    TMOD |= 0x01;
	TH0   = 0xD5;
	TL0   = 0x9D;
	
	TR0   = 1;   // ������ʱ��T0
	ET0   = 1;	 // ����ʱ��T0�ж�
	PT0   = 0;   // ��ʱ��T0���ȼ�����

}

/*=======================================================================
�������ƣ�Init_Timer1
�������ܣ�1����ʱ��T1����ģʽ��ʼ��
          2����ʱ��T1��ֵ�趨
		  3����ʱ��T1�ж�ģʽ����
		  4��16λ���� �ø�8λ��ʾ�����ߵļ���ֵ �������ж�
�����������
�����������
�� �� ֵ����
����˵������
=======================================================================*/
void Init_Timer1( void ){

    TMOD |= 0x20;
	TH1   = 0xFD;
	TL1   = 0xFD;	// 9600�Ĳ�����

	TR1   = 1;   // ������ʱ��T1
	ET1   = 0;	 // ��ֹ��ʱ��T1�ж�
	PT1   = 0;   // ��ʱ��T1���ȼ�����

}

/*=======================================================================
�������ƣ�Init_Serial0
�������ܣ�1�����п�0����ģʽ��ʼ��
          2�����пڲ������趨
		  3�����п��ж�ģʽ��ʼ��
�����������
�����������
�� �� ֵ����
����˵������
=======================================================================*/
void Init_UART0( void ){

	// ���п�0����ģʽ��ʼ�� 
    SCON  = 0x50; // 9600������

	PS    = 1; // ���п�0�ж����ȼ�����  Ϊ��� 
	ES    = 1; // �������п�0�ж�

	TI    = 0; // ������п�0�����жϱ�־
	RI    = 0; // ������п�0�����жϱ�־

}

/*=======================================================================
�������ƣ�Init_PCA
�������ܣ�1��
          2��
		  3��
�����������
�����������
�� �� ֵ����
����˵������
=======================================================================*/
/*
void Init_PCA( void ){

	CMOD   = 0x00; // �������� ʱ��ԴΪfosc/12
	CCAPM0 = 0x49; // 16λ������
	CCON   = 0x40; // ����PCA0

}
*/

/*=======================================================================
�������ƣ�Init_WDT
�������ܣ���ʼ�����Ź�
�����������
�����������
�� �� ֵ����
����˵������ֹ���Ź�
=======================================================================*/
void Init_WDT( void ){


	WDT_CONTR = 0x07;

}

/*=======================================================================
�������ƣ�Init_System
�������ܣ�ϵͳ��ʼ��
�����������
�����������
�� �� ֵ����
����˵����
=======================================================================*/
void Init_System( void ){

	Init_WDT();
	Init_Port();
	Init_Timer0();
	Init_Timer1();
	Init_INT0();
	Init_UART0();   


	EA = 1;

}

