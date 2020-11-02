//********************************************************* 
/*	�ļ�����TEST_FT62F21x_PA_INT.c
*	���ܣ�	 FT62F21x-PA�ڵ�ƽ�жϹ�����ʾ
*	IC:		 FT62F211 SOP8
*	����   16M/4T                    
*	˵����   ������DemoPortOut(PA4)���100֡50HZ��ռ�ձ�Ϊ50%�ķ�����,MCU����˯��,�ȴ��жϵķ���;
*		   		 ��ÿ��PA2��ƽ�仯�жϴ������ظ���������;
*
*                  FT62F211 SOP8 
*                 ----------------
*  led1----------|1(PA4)     (PA3)8|------------led2     
*  NC-----------|2(TKCAP)  (PA0)7|------------NC
*  VDD----------|3(VDD)     (PA1)6|------------NC
*  GND----------|4(VSS)     (PA2)5|------------NC
*			      ----------------
*/
//*********************************************************
#include	"SYSCFG.h";
#include 	"FT62F21X.h";
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
#define  DemoPortOut	RA4   
 
unchar FCount;

unchar ReadAPin;
/*-------------------------------------------------
 *	��������interrupt ISR
 *	���ܣ�  �жϴ�������
 *	���룺  ��
 *	�����  ��
 --------------------------------------------------*/
void interrupt ISR(void)	//PIC_HI-TECHʹ��
{ 
  //PA��ƽ�仯�ж�**********************
	 if(PAIE && PAIF)		
    {
		ReadAPin = PORTA; 	//��ȡPORTA������PAIF��־
		PAIF = 0;  			//��PAIF��־λ
		PAIE = 0;  			//���Ƚ�ֹPA0�ж�
		IOCA2 =0;  			//��ֹPA0��ƽ�仯�ж�
		 
    }
} 
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
	TRISA = 0B00000100;				//PA������� 0-��� 1-����  
	WPUA = 0B00000100;     		//PA�˿��������� 1-������ 0-������

	OPTION = 0B00001000;			//Bit3=1 WDT MODE,PS=000=1:1 WDT RATE
													//Bit7(PAPU)=0 ��WPUA�����Ƿ�����
	MSCON = 0B00000000;		                             
}
 /*-------------------------------------------------
 *  �������ƣ�DelayUs
 *  ���ܣ�    ����ʱ���� --16M-4T--��ſ�1%����.
 *  ���������Time ��ʱʱ�䳤�� ��ʱʱ��Time*2 Us
 * 	���ز������� 
 -------------------------------------------------*/
void DelayUs(unsigned char Time)
{
	unsigned char a;
	for(a=0;a<Time;a++)
	{
		NOP();
	}
}                  
/*------------------------------------------------- 
 * 	�������ƣ� DelayMs
 * 	���ܣ�    ����ʱ����
 * 	���������Time ��ʱʱ�䳤�� ��ʱʱ��Time ms
 * 	���ز������� 
 -------------------------------------------------*/
void DelayMs(unsigned char Time)
{
	unsigned char a,b;
	for(a=0;a<Time;a++)
	{
		for(b=0;b<5;b++)
		{
		 	DelayUs(98);               //��1%
		}
	}
}
/*------------------------------------------------- 
 * 	�������ƣ�DelayS
 * 	���ܣ�   ����ʱ����
 * 	���������Time ��ʱʱ�䳤�� ��ʱʱ��Time S
 * 	���ز������� 
 -------------------------------------------------*/
void DelayS(unsigned char Time)
{
	unsigned char a,b;
	for(a=0;a<Time;a++)
	{
		for(b=0;b<10;b++)
		{
		 	DelayMs(100); 
		}
	}
}
/*-------------------------------------------------
 *	������: PA2_Level_Change_INITIAL
 *	���ܣ�  PA�˿�(PA2)��ƽ�仯�жϳ�ʼ��
 *	���룺  ��
 *	�����  ��
--------------------------------------------------*/
void PA2_Level_Change_INITIAL(void)
{
 
	TRISA2 =1; 			     //SET PA2 INPUT
	ReadAPin = PORTA;	 //��PA��ƽ�仯�ж�
	PAIF =0;   			     //��PA INT�жϱ�־λ
    IOCA2 =1;  			     //ʹ��PA2��ƽ�仯�ж�
	PAIE =1;   			     //ʹ��PA INT�ж�
    //GIE =1;    				 //ʹ��ȫ���ж�
}
/*-------------------------------------------------
 *	������: main 
 *	���ܣ�  ������
 *	���룺  ��
 *	�����  ��
 --------------------------------------------------*/
void main()
{
	POWER_INITIAL();						//ϵͳ��ʼ��
 
	while(1)
	{
		for(FCount=0;FCount<100;FCount++)	//���100�β���	
		{
			DemoPortOut = 1; 				
			DelayMs(10);  					
			DemoPortOut = 0;
			DelayMs(10); 
		}
		PA2_Level_Change_INITIAL();			//��ʼ��PA��ƽ�仯�ж�
		GIE = 1;											//�����ж�
        NOP();
        NOP();
        
		SLEEP(); 											//˯��
        NOP();
        NOP();
	}
}