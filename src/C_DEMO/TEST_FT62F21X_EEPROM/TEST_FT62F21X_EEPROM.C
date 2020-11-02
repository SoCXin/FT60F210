//********************************************************* 
/*	�ļ�����TEST_FT62F21x_EEPROM.c
*	���ܣ�	 FT62F21x-EEPROM������ʾ
*	IC:		 FT62F211 SOP8
*	����   16M/4T                    
*	˵����   �ó����ȡ0x12��ַ��ֵ,ȡ�������0x13��ַ
*
*                  FT62F211 SOP8 
*                 ----------------
*  NC-----------|1(PA4)      (PA3)8|------------NC    
*  NC-----------|2(TKCAP)  (PA0)7|------------NC
*  VDD----------|3(VDD)     (PA1)6|------------NC
*  GND----------|4(VSS)     (PA2)5|------------NC
*			      ----------------
*/
//*********************************************************
#include	"SYSCFG.h";
//#include 	"FT62F21X.h";
//*********************************************************
#define OSC_16M		0X70
#define OSC_8M		0X60
#define OSC_4M		0X50
#define OSC_2M		0X40
#define OSC_1M		0X30
#define OSC_500K	0X20
#define OSC_250K	0X10
#define OSC_32K		0X00

#define WDT_256K	0X80
#define WDT_32K		0X00
//**********************************************************
//***********************�궨��*****************************
#define  unchar     unsigned char 
#define  unint      	unsigned int
#define  unlong     unsigned long  
 
unchar EEReadData;
 /*-------------------------------------------------
 *	��������POWER_INITIAL
 *	���ܣ�  �ϵ�ϵͳ��ʼ��
 *	���룺  ��
 *	�����  ��
 --------------------------------------------------*/	
void POWER_INITIAL (void) 
{
	OSCCON = WDT_32K|OSC_16M|0X00;	
  //OSCCON = 0B01110000;		//WDT 32KHZ IRCF=111=16MHZ/4=4MHZ,0.25US/T

	INTCON = 0;  							//�ݽ�ֹ�����ж�

	PORTA = 0B00000000;				
	TRISA = 0B00000000;				//PA������� 0-��� 1-����  
	WPUA = 0B00000000;     		//PA�˿��������� 1-������ 0-������

	OPTION = 0B00001000;			//Bit3=1 WDT MODE,PS=000=1:1 WDT RATE�
													//Bit7(PAPU)=0 ��WPUA�����Ƿ�����
	MSCON = 0B00000000;		                             
}
/*-------------------------------------------------
 *	��������EEPROMread
 *	���ܣ�  ��EEPROM����
 *	���룺  ��Ҫ��ȡ���ݵĵ�ַ EEAddr
 *	�����  ��Ӧ��ַ���������� ReEEPROMread
 --------------------------------------------------*/
unchar EEPROMread(unchar EEAddr)
{
	unchar ReEEPROMread;
    EEADR = EEAddr;
    RD=1;
    NOP();
    NOP();
    NOP();
    NOP();
    ReEEPROMread =EEDAT;
    return ReEEPROMread;
}
/*-------------------------------------------------
 *	��������EEPROMwrite
 *	���ܣ�  д���ݵ�EEPROM
 *	���룺  ��Ҫ��ȡ���ݵĵ�ַ EEAddr
				��Ҫд�������		  Data
 *	�����  ��
 --------------------------------------------------*/
void EEPROMwrite(unchar EEAddr,unchar Data)
{
	
	while(GIE)					//�ȴ�GIEΪ0
		{GIE = 0;	}						//д���ݱ���ر��ж�
	EEADR = EEAddr; 	 			//EEPROM�ĵ�ַ
	EEDAT = Data;		 			//EEPROM��д����  EEDATA = Data;
	EEIF = 0;
	EECON1 |= 0x34;				//��λWREN1,WREN2,WREN3��������.
	WR = 1;							//��λWR������
    NOP();
    NOP();
    NOP();
    NOP();
	while(WR);      				//�ȴ�EEд�����
	GIE = 1;
}
/*-------------------------------------------------
 *	������: main 
 *	���ܣ�  ������
 *	���룺  ��
 *	�����  ��
 --------------------------------------------------*/
void main()
{
	POWER_INITIAL();								//ϵͳ��ʼ��
 	EEPROMwrite(0x55,0xaa);	
	EEPROMwrite(0x55,0xaa);					//��δʹ�õ�������һ����ַд����0xAA
 
	EEReadData = EEPROMread(0x12); 		//��ȡ0x12��ַEEPROMֵ 
	EEPROMwrite(0x13,~EEReadData); 		//ȡ��д���ַ0x13
    
	while(1) 
	{
 
		NOP();
	 
	}
}