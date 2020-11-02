//********************************************************* 
/*	�ļ�����TEST_FT62F21x_IR_Send.c
*	���ܣ�	 FT62F21x-���ⷢ��  ������ʾ
*	IC:		 FT62F211 SOP8
*	����   16M/4T                    
*	˵����   ��ʾ������,IR�����ǲ���6122Э�飬��ʼ�ź���9ms�͵�ƽ����4.5ms�ߵ�ƽ���ٵ���8λ
*          	 �û�ʶ���룬����8λ���û�ʶ���룬8λ�����룬8λ������ķ��롣SendIO��PA4����ʱ
*         		��5���ӣ�����һ�Σ����ն��յ�ң���������������ݺ�У�����ݻ�Ϊ���룬LED�Ὺ�ء�
*
*                  FT62F211 SOP8 
*                 ----------------
*  IRSendIO-----|1(PA4)     (PA3)8|------------NC     
*  NC-----------|2(TKCAP)  (PA0)7|------------NC
*  VDD----------|3(VDD)     (PA1)6|------------NC
*  GND----------|4(VSS)     (PA2)5|------------NC
*			      ----------------
*/
//*********************************************************
#include	"SYSCFG.h";
#include 	"FT62F21X.h";
//***********************�궨��*****************************�
#define  uchar	unsigned char 
#define  uint		unsigned int
#define  ulong	unsigned long

#define  IRSendIO		RA4  								//���ڵķ��ͽ�

#define IRSend_HIGH_1   1  								//560uS
#define IRSend_LOW_1    3 								//1680uS

#define IRSend_HIGH_0   1  								//560uS
#define IRSend_LOW_0    1  							//560uS
	
#define IRSend_PIN_1   T0IE = 1  					//��������  ������ʱ��0
#define IRSend_PIN_0   T0IE = 0 						//�رն�ʱ��0

#define Status_NOSend	0			           			 //�����͵�״̬
#define Status_Head		1			           				//�����������״̬
#define Status_Data		2			            			//�������ݵ�״̬

uchar IRSendStatus;											//����״̬���Ƿ��������뻹������
uchar IRSendData;											//���͵�������ת����
uchar  TxBit=0,TxTime=0; 
uchar Sendbit = 0;
uchar level0,level1;											//һλ�����﷢����رյ�ʱ��ֵ
bit SendLastBit = 0;
uchar SaveLastBit = 0;
uint SYSTime5S = 0;											//ϵͳʱ�䣬5S����һ��

uchar IRData[4] = {0x00,0xff,0x40,0xBf};		//��Ҫ���͵�4������

 /*-------------------------------------------------
 *	��������POWER_INITIAL
 *	���ܣ�  �ϵ�ϵͳ��ʼ��
 *	���룺  ��
 *	�����  ��
 --------------------------------------------------*/	
void POWER_INITIAL (void) 
{	
    OSCCON = 0B01110000;			//WDT 32KHZ IRCF=111=16MHZ/4=4MHZ,0.25US/T

	INTCON = 0;  							//�ݽ�ֹ�����ж�

	PORTA = 0B00000000;				
	TRISA = 0B00000000;				//PA������� 0-��� 1-����  
	WPUA = 0B00000000;     		//PA�˿��������� 1-������ 0-������

	OPTION = 0B00001000;			//Bit3=1 WDT MODE,PS=000=1:1 WDT RATE
													//Bit7(PAPU)=0 ��WPUA�����Ƿ�����
	MSCON = 0B00000000;		                             
}
/*----------------------------------------------------
 *	�������ƣ�TIMER0_INITIAL
 *	���ܣ���ʼ�����ö�ʱ��0
 * ˵����38KHz��������1000000/38000=26.3uS
 *	����TMR0��ʱʱ��13us=(1/16000000)*4*2*26(16M-4T-PSA 1:2- TMR0=255���)	                     
 ----------------------------------------------------*/
void TIMER0_INITIAL (void)  
{
	OPTION = 0B00000000; 		//Ԥ��Ƶ�������Timer0��Ԥ��Ƶ��Ϊ1:2��������   
	//Bit5 T0CS Timer0ʱ��Դѡ�� 
	//1-�ⲿ���ŵ�ƽ�仯T0CKI 0-�ڲ�ʱ��(FOSC/2)
	//Bit4 T0CKI���Ŵ�����ʽ 1-�½��� 0-������
	//Bit3 PSA Ԥ��Ƶ������λ 0-Timer0 1-WDT 
	//Bit2:0 PS2 8��Ԥ��Ƶ�� 011 - 1:16
    						
	TMR0 = 239; 
    T0IF = 0;								//���T0�����ж�
}
/*-------------------------------------------------
 * �������ƣ�TIMER2_INITIAL
 * ���ܣ���ʼ�����ö�ʱ��2
--------------------------------------------------*/
void TIMER2_INITIAL(void)
{ 
	T2CON0 = 0B00000011; 		//Ԥ��Ƶ��Ϊ1:16�����Ƶ��Ϊ1:1  	 
    
	TMR2H = 0;  						//TMR2����ֵ
    TMR2L = 0;
        
	PR2L = 140; 						//����TMR2����Ƚ�ֵ��ʱ560us=(1/16000000)*4*16*140(PR2)
	PR2H = 0;							//16M-4T-4��Ƶ 
	//PR2 = 60; 							//����TMR2����Ƚ�ֵ��ʱ15us=(1/4000000)*60(PR2)
	TMR2IF = 0;							//��TMER2�жϱ�־
	TMR2IE = 1;							//ʹ��TMER2���ж�
	TMR2ON = 1;						//ʹ��TMER2����
	PEIE=1;    							//ʹ�������ж�
}
/*-------------------------------------------------
 *	�������� SendCtrl
 *	���ܣ�   �������ݺ���
 *	���룺   ��
 *	�����   ��
 --------------------------------------------------*/
void SendCtrl(void)
{

	if (IRSendStatus == Status_NOSend)			//�����͵�״̬
	{        
		IRSend_PIN_0;
        Sendbit = 0;
		TxTime = 0;
        
	}	 
	else if (IRSendStatus == Status_Head)		//����������
	{
    	TxTime++;
		if (TxTime < 17)   						//����9mS�ź�
		{
			IRSend_PIN_1;
		}
        else if (TxTime < 24)   				//4.5mS������
		{
			IRSend_PIN_0;
		}
		else
		{
			TxTime = 0;
            IRSendStatus = Status_Data;
		}
        IRSendData = IRData[0];
        TxBit = 0x01;
	}
	else if(IRSendStatus == Status_Data)		//��������
	{
		if (IRSendData & TxBit)  				//1����1:3��ʱ��
		{
			level1 = IRSend_HIGH_1;
			level0 = IRSend_LOW_1;
		}
		else									//0����1:1��ʱ��
		{
			level1 = IRSend_HIGH_0;
			level0 = IRSend_LOW_0;
		}
		TxTime++;
		if (TxTime <= level1)  					//�����ź�
		{
			IRSend_PIN_1;
		}
		else if (TxTime <= (level0+level1)) 	//�������ź�
		{
			IRSend_PIN_0;
		}
		else if (Sendbit < 4)					//����4λ����δ���
		{
			TxTime = 1;
            IRSend_PIN_1;
            SaveLastBit = IRSendData & TxBit;
			TxBit <<= 1;
			if (TxBit == 0x00)  				//������һ���ֽ�
			{
				TxBit = 0x01;
                Sendbit++;
                IRSendData = IRData[Sendbit];
				if (Sendbit > 3)   				//���һλҪע�⣬��Ϊ�������˻�Ҫ��һ������
				{
                    SendLastBit = 1;
				}
			}
		}
        else									//��������ˣ�Ҫ������
        {
        	if(SendLastBit)
 		   	{
		    	TxTime++;
		    	if(SaveLastBit)
		        {
		        	if(TxTime < 3)
		            {
		            	IRSend_PIN_0;
		            }
		            else if(TxTime < 4)
		            {
		            	IRSend_PIN_1;
		            }
		            else
		            {
		            	IRSend_PIN_0;
						IRSendStatus = Status_NOSend;
		                IRSend_PIN_0;
		                SendLastBit = 0;
                        TxBit = 0;
                        TxTime = 0;
		            }
		        }
		        else
		        {
		        	if(TxTime < 5)
		            {
		            	IRSend_PIN_0;
		            }
		            else if(TxTime < 6)
		            {
		            	IRSend_PIN_1;
		            }
		            else
		            {
		            	IRSend_PIN_0;
						IRSendStatus = Status_NOSend;
		                IRSend_PIN_0;
		                SendLastBit = 0;
                        TxBit = 0;
                        TxTime = 0;
		            }
		        }
		    }
        }
	}
    
}
/*-------------------------------------------------
 *	��������interrupt ISR
 *	���ܣ� 	 ��ʱ��0�жϺͶ�ʱ��2�ж�
 *	���룺	 ��
 *	����� 	 ��
 --------------------------------------------------*/
void interrupt ISR(void)			//PIC_HI-TECHʹ��
{ 
   
  //��ʱ��0���жϴ���**********************
	if(T0IE && T0IF)							//13us
	{
		TMR0 = 239;							//ע��:��TMR0���¸�ֵTMR0�����������ڲ��仯	 
		T0IF = 0;    
		IRSendIO = ~IRSendIO; 			//��ת��ƽ  ����38KHz�ź�
	} 
    
    //��ʱ��2���жϴ���**********************
	if(TMR2IE && TMR2IF)			//560us�ж�һ�� ����ÿһλ����560uS�ı���
	{
		TMR2IF = 0;
        SendCtrl();
        SYSTime5S++;
	}
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
    TIMER0_INITIAL();
    TIMER2_INITIAL();
	GIE = 1;										//�����ж�
	while(1)
	{
		if(SYSTime5S >10000)				//��ʱ5S
        {
        	SYSTime5S = 0;
            IRSendStatus = Status_Head;
        }
	}
}